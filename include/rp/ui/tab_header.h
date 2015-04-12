//
//  tab_header.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/12/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#pragma once

#include "button.h"
#include <vector>

namespace rp { namespace ui {
    
    enum TabHeaderOrientation {
        TabHeaderOrientationHorizontal,
        TabHeaderOrientationVertical
    };

    template<class TTag>
    class TabHeaderButton : public PaneImageButton {
    public:
        TabHeaderButton()
            : PaneImageButton()
            , active_(false)
        {}
        
        ~TabHeaderButton()
        {}
        
        bool getActive() {
            return active_;
        }
        
        void setActive(bool v) {
            if (active_ == v) {
                return;
            }
            
            active_ = v;
            this->setState(v ? ButtonStateActive : ButtonStateNormal);
        }
        
        TTag getTag() {
            return tag_;
        }
        
        void setTag(const TTag& tag) {
            if (tag_ == tag)
                return;
            
            tag_ = tag;
        }
        
        virtual void invokeOnTouch(const TouchEvent& ev) {
            if (active_) {
                this->setState(ButtonStateActive);
                return;
            }
            
            PaneImageButton::invokeOnTouch(ev);
        }
        
    private:
        bool active_;
        TTag tag_;
    };
    
    template<class TTag>
    class TabHeader : public DisplayObjectContainer {
    public:
        typedef TabHeaderButton<TTag> button_t;
        typedef std::shared_ptr<button_t> button_ptr;
        
        TabHeader()
            : DisplayObjectContainer()
            , background_(new Pane())
            , orientation_(TabHeaderOrientationVertical)
        {
            this->appendChild(background_);
        }
        
        ~TabHeader()
        {}
        
        void addTab(std::shared_ptr<Image> normalImage, std::shared_ptr<Image> activeImage, const TTag& tag) {
            auto self = shared_from_this();
            std::weak_ptr<TabHeader> weakSelf = std::dynamic_pointer_cast<TabHeader>(self);
            
            button_ptr tabHeaderButton(new button_t);
            tabHeaderButton->setImages(normalImage, activeImage);
            tabHeaderButton->setTag(tag);
            appendChild(tabHeaderButton);
            headers_.push_back(tabHeaderButton);
            
            tabHeaderButton->setOnPress(std::bind(&TabHeader::onTabHeaderButtonPress_, weakSelf, TTag(tag)));
        }
        
        void addTab(std::shared_ptr<Image> normalImage, const TTag& tag) {
            addTab(normalImage, normalImage, tag);
        }
        
        void clearTabs() {
            for (auto tab : headers_) {
                removeChild(tab);
            }
            
            headers_.clear();
        }
        
        void setOnSelectionChanging(std::function<void(button_ptr oldTab, button_ptr newTab)> callback) {
            onSelectionChangingCallback_ = callback;
        }
        
        void selectTab(const TTag& tag) {
            button_ptr targetTab;
            
            for (auto tab : headers_) {
                if (tab->getTag() == tag) {
                    targetTab = tab;
                    break;
                }
            }
            
            selectTab_(targetTab);
        }
        
        void selectTabAtIndex(size_t index) {
            if (index < headers_.size()) {
                selectTab_(headers_[index]);
            }
        }
        
        TabHeaderOrientation getOrientation() {
            return orientation_;
        }
        
        void setOrientation(TabHeaderOrientation orientation) {
            if (orientation_ == orientation) {
                return;
            }
            
            orientation_ = orientation;
            relayout_(getSize());
        }
        
        void onResizing(const Size& oldSize, const Size& newSize) {
            DisplayObjectContainer::onResizing(oldSize, newSize);
            relayout_(newSize);
        }
        
        Color getBackgroundColor() const {
            return background_->getBackgroundColor();
        }
        
        void setBackgroundColor(Color c) {
            background_->setBackgroundColor(c);
        }
        
    private:
        void relayout_(Size size) {
            background_->setSize(size);
            
            if (orientation_ == TabHeaderOrientationHorizontal) {
                int x = 0;
                int height = size.height;
                
                for (auto tab : headers_) {
                    tab->setFrame(Rectangle(x, 0, height, height));
                    x += height;
                }
            } else {
                int y = 0;
                int width = size.width;
                
                for (auto tab : headers_) {
                    tab->setFrame(Rectangle(0, y, width, width));
                    y += width;
                }
            }
        }
        
        void selectTab_(button_ptr button) {
            if (activeButton_ == button) {
                return;
            }
            
            invokeOnSelectionChanging_(activeButton_, button);
            
            if (activeButton_) {
                activeButton_->setActive(false);
            }
            
            activeButton_ = button;
            
            if (button) {
                button->setActive(true);
            }
        }
        
        void invokeOnSelectionChanging_(button_ptr oldTab, button_ptr newTab) {
            auto callback = onSelectionChangingCallback_;
            
            if (callback) {
                callback(oldTab, newTab);
            }
        }
        
        static void onTabHeaderButtonPress_(std::weak_ptr<TabHeader> self, TTag tag) {
            auto lockedSelf = self.lock();
            
            if (!lockedSelf)
                return;
            
            lockedSelf->selectTab(tag);
        }
        
        std::shared_ptr<Pane> background_;
        std::vector<button_ptr> headers_;
        button_ptr activeButton_;
        
        TabHeaderOrientation orientation_;
        
        std::function<void(button_ptr oldTab, button_ptr newTab)> onSelectionChangingCallback_;
    };
    
}}
