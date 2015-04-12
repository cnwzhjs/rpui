//
//  label.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/3/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/ui/display_object.h>
#include <rp/ui/color.h>
#include <string>
#include <memory>

namespace rp { namespace ui {
    
    enum TextAlignment {
        TextAlignmentNear,
        TextAlignmentCenter,
        TextAlignmentFar
    };
    
    class Image;
    class Font;
    class IDisplayObjectVisitor;
    
    class Label : public DisplayObject {
    public:
        Label();
        virtual ~Label();
        
        std::string getText() const;
        void setText(const std::string& v);
        
        std::string getFontFace() const;
        void setFontFace(const std::string& v);
        
        float getFontSize() const;
        void setFontSize(float v);
        
        Color getFontColor() const;
        void setFontColor(Color v);
        
        TextAlignment getHorizontalAlignment() const;
        void setHorizontalAlignment(TextAlignment alignment);
        
        TextAlignment getVerticalAlignment() const;
        void setVerticalAlignment(TextAlignment alignment);
        
        std::shared_ptr<Image> getRenderedImage();
        
        virtual void apply(std::shared_ptr<IDisplayObjectVisitor> visitor);
        
    private:
        void clearFont_();
        void clearCachedImage_();
        void markDirty_();
        void recreateFont_();
        
        std::shared_ptr<Font> font_;
        
        std::string text_;
        std::string fontFace_;
        float fontSize_;
        Color fontColor_;
        TextAlignment horizontalAlignment_;
        TextAlignment verticalAlignment_;
        
        std::shared_ptr<Image> cachedImage_;
    };
    
}}
