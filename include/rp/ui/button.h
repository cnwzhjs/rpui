//
//  button.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/3/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/ui/stateful.h>
#include <rp/ui/display_object_container.h>
#include <rp/ui/label.h>
#include <rp/ui/picture_box.h>
#include <rp/ui/pane.h>
#include <rp/ui/style_manager.h>
#include <rp/ui/image.h>

#include <functional>

namespace rp { namespace ui {
    
    enum ButtonState {
        ButtonStateNormal,
        ButtonStateActive,
        ButtonStateDisabled
    };
    
    template<class TBase, class TView>
    class Button : public TView, public Stateful<TBase, ButtonState> {
    public:
        typedef TView view_t;

        Button() : TView(), Stateful<TBase, ButtonState>(ButtonStateNormal), enabled_(true) {}
        ~Button() {}
        
        bool getEnabled() const {
            return enabled_;
        }
        
        void setEnabled(bool v) {
            if (enabled_ == v)
                return;
            
            enabled_ = v;
            
            Stateful<TBase, ButtonState>::setState(v ? ButtonStateNormal : ButtonStateDisabled);
        }
        
        virtual void invokeOnTouch(const TouchEvent& ev) {
            if (ev.type == TouchEventTypeBegan) {
                Stateful<TBase, ButtonState>::setState(ButtonStateActive);
            } else if (ev.type == TouchEventTypeEnd) {
                Stateful<TBase, ButtonState>::setState(ButtonStateNormal);
                invokeOnPress();
            } else if (ev.type == TouchEventTypeCancel) {
                Stateful<TBase, ButtonState>::setState(ButtonStateNormal);
            }
            
            TView::invokeOnTouch(ev);
        }
        
        void setOnPress(std::function<void()> callback) {
            onPressCallback_ = callback;
        }
        
        virtual void invokeOnPress() {
            if (onPressCallback_) {
                onPressCallback_();
            }
        }
        
    private:
        friend class Stateful<TBase, ButtonState>;
        
        bool enabled_;
        std::function<void()> onPressCallback_;
    };
    
    template<class TBackground>
    class LabelButton : public Button<LabelButton<TBackground>, DisplayObjectContainer> {
    protected:
        LabelButton(std::shared_ptr<TBackground> background)
        : background_(background)
        , label_(new Label())
        {
            auto style = StyleManager::defaultManager();
            
            std::string defaultFont = style->getString("Font.DefaultFontFace", "Verdana");
            
            fontFace_ = style->getString("LabelButton.FontFace", defaultFont);
            fontSize_ = style->getFloat("LabelButton.FontSize", 9);
            normalColor_ = style->getColor("LabelButton.NormalColor");
            activeColor_ = style->getColor("LabelButton.ActiveColor");
            disabledColor_ = style->getColor("LabelButton.DisabledColor");
            
            label_->setFontFace(fontFace_);
            label_->setFontColor(normalColor_);
            label_->setFontSize(fontSize_);
            label_->setHorizontalAlignment(TextAlignmentCenter);
            label_->setVerticalAlignment(TextAlignmentCenter);
            
            DisplayObjectContainer::appendChild(background_);
            DisplayObjectContainer::appendChild(label_);
            
            DisplayObjectContainer::setTouchChildren(false);
        }

    public:
        ~LabelButton() {}
        
        std::string getText() {
            return label_->getText();
        }
        
        void setText(const std::string& text) {
            label_->setText(text);
        }
        
        void onResizing(const Size& oldSize, const Size& newSize) {
            background_->setSize(newSize);
            label_->setSize(newSize);
            
            Button<LabelButton<TBackground>, DisplayObjectContainer>::onResizing(oldSize, newSize);
        }
        
        virtual void onStateChanging(const ButtonState& oldState, const ButtonState& newState) {
            switch (newState) {
                case ButtonStateNormal:
                    label_->setFontColor(normalColor_);
                    break;
                    
                case ButtonStateActive:
                    label_->setFontColor(activeColor_);
                    break;
                    
                case ButtonStateDisabled:
                    label_->setFontColor(disabledColor_);
                    break;
                    
                default:
                    break;
            }
        }
        
    protected:
        std::shared_ptr<TBackground> background_;
        std::shared_ptr<Label> label_;
        
    private:
        std::string fontFace_;
        float fontSize_;
        Color normalColor_;
        Color activeColor_;
        Color disabledColor_;
    };
    
    template<class TBackground>
    class ImageButton : public Button<ImageButton<TBackground>, DisplayObjectContainer> {
    protected:
        ImageButton(std::shared_ptr<TBackground> background)
        : background_(background)
        , pictureBox_(new PictureBox)
        {
            if (background_) {
                this->appendChild(background_);
            }
            this->appendChild(pictureBox_);

            DisplayObjectContainer::setTouchChildren(false);
        }
        
    public:
        ~ImageButton()
        {}
        
        void onResizing(const Size& oldSize, const Size& newSize) {
            background_->setSize(newSize);
            centralizeImage_(newSize);
            
            Button<ImageButton<TBackground>, DisplayObjectContainer>::onResizing(oldSize, newSize);
        }
        
        virtual void onStateChanging(const ButtonState& oldState, const ButtonState& newState) {
            auto it = stateImages_.find(newState);
            
            if (it != stateImages_.end()) {
                setImage_(it->second);
            }
        }
        
        void setImageForState(ButtonState state, std::shared_ptr<Image> image) {
            stateImages_[state] = image;
            if (this->getState() == state) {
                setImage_(image);
            }
        }
        
        void setImages(std::shared_ptr<Image> normalImage, std::shared_ptr<Image> activeImage, std::shared_ptr<Image> disabledImage) {
            setImageForState(ButtonStateNormal, normalImage);
            setImageForState(ButtonStateActive, activeImage);
            setImageForState(ButtonStateDisabled, disabledImage);
        }
        
        void setImages(std::shared_ptr<Image> normalImage, std::shared_ptr<Image> activeImage) {
            setImages(normalImage, activeImage, normalImage);
        }
        
        void setImages(std::shared_ptr<Image> normalImage) {
            setImages(normalImage, normalImage);
        }
        
    protected:
        void setImage_(std::shared_ptr<Image> image) {
            pictureBox_->setImage(image);
            if (image) {
                pictureBox_->setSize(Size(image->getWidth(), image->getHeight()));
                centralizeImage_(this->getSize());
            }
        }
        
        void centralizeImage_(Size size) {
            int x = (size.width - pictureBox_->getWidth())/2;
            int y = (size.height - pictureBox_->getHeight())/2;
            pictureBox_->setPosition(Point(x, y));
        }
        
        std::shared_ptr<TBackground> background_;
        std::shared_ptr<PictureBox> pictureBox_;
        
        std::map<ButtonState, std::shared_ptr<Image> > stateImages_;
    };
    
    template<class TForegroundButton>
    class PaneBackgroundButton : public TForegroundButton {
    public:
        PaneBackgroundButton()
        : TForegroundButton(std::shared_ptr<Pane>(new Pane()))
        {
            auto style = StyleManager::defaultManager();
            
            normalColor_ = style->getColor("PaneBackgroundButton.NormalColor", colors::silver);
            activeColor_ = style->getColor("PaneBackgroundButton.ActiveColor", colors::white);
            disabledColor_ = style->getColor("PaneBackgroundButton.DisabledColor", colors::gray);
            
            TForegroundButton::background_->setBackgroundColor(normalColor_);
        }
        
        ~PaneBackgroundButton()
        {}
        
    public:
        void onStateChanging(const ButtonState& oldState, const ButtonState& newState) {
            TForegroundButton::onStateChanging(oldState, newState);
            
            switch (newState) {
                case rp::ui::ButtonStateNormal:
                    TForegroundButton::background_->setBackgroundColor(normalColor_);
                    break;
                    
                case rp::ui::ButtonStateActive:
                    TForegroundButton::background_->setBackgroundColor(activeColor_);
                    break;
                    
                case rp::ui::ButtonStateDisabled:
                    TForegroundButton::background_->setBackgroundColor(disabledColor_);
                    break;
                    
                default:
                    break;
            }
        }
        
    private:
        Color normalColor_;
        Color activeColor_;
        Color disabledColor_;
    };
    
    class PaneLabelButton : public PaneBackgroundButton<LabelButton<Pane> >
    {};
    
    class PaneImageButton : public PaneBackgroundButton<ImageButton<Pane> >
    {};
    
    
}}
