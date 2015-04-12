//
//  label.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/3/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#include <rp/ui/label.h>
#include <rp/ui/image.h>
#include <rp/ui/font.h>

using namespace std;
using namespace rp::util;

namespace rp { namespace ui {

    Label::Label() {}
    Label::~Label() {}
    
    string Label::getText() const {
        return text_;
    }
    
    void Label::setText(const std::string &v) {
        if (text_ == v)
            return;
        
        text_ = v;
        clearCachedImage_();
    }
    
    string Label::getFontFace() const {
        return fontFace_;
    }
    
    void Label::setFontFace(const std::string &v) {
        if (fontFace_ == v)
            return;
        
        fontFace_ = v;
        clearFont_();
    }
    
    float Label::getFontSize() const {
        return fontSize_;
    }
    
    void Label::setFontSize(float v) {
        if (fontSize_ == v)
            return;
        
        fontSize_ = v;
        clearFont_();
    }
    
    Color Label::getFontColor() const {
        return fontColor_;
    }
    
    void Label::setFontColor(Color v) {
        if (fontColor_ == v)
            return;
        
        fontColor_ = v;
        clearCachedImage_();
    }
    
    TextAlignment Label::getHorizontalAlignment() const {
        return horizontalAlignment_;
    }
    
    void Label::setHorizontalAlignment(TextAlignment alignement) {
        if (horizontalAlignment_ == alignement)
            return;
        
        horizontalAlignment_ = alignement;
        markDirty_();
    }
    
    TextAlignment Label::getVerticalAlignment() const {
        return verticalAlignment_;
    }
    
    void Label::setVerticalAlignment(TextAlignment alignement) {
        if (verticalAlignment_ == alignement)
            return;
        
        verticalAlignment_ = alignement;
        markDirty_();
    }
    
    shared_ptr<Image> Label::getRenderedImage() {
        if (!font_) {
            recreateFont_();
        }
        
        if (!font_) {
            return nullptr;
        }
        
        if (!cachedImage_ && !text_.empty()) {
            cachedImage_ = font_->render(text_, fontColor_);
        }
        
        return cachedImage_;
    }
    
    void Label::apply(std::shared_ptr<IDisplayObjectVisitor> visitor) {
        visitor->visit(dynamic_pointer_cast<Label>(shared_from_this()));
    }
    
    void Label::clearFont_() {
        font_ = nullptr;
        clearCachedImage_();
    }
    
    void Label::clearCachedImage_() {
        cachedImage_ = nullptr;
        markDirty_();
    }
    
    void Label::markDirty_() {
        markDirty(getFrame());
    }
    
    void Label::recreateFont_() {
        font_ = shared_ptr<Font>(new Font(fontFace_, fontSize_));
    }
    
}}
