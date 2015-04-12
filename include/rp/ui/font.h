//
//  font.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/3/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/util/noncopyable.h>
#include <rp/ui/geometry.h>
#include <rp/ui/color.h>
#include <string>
#include <memory>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace rp { namespace ui {
    
    class Image;
    
    class Font : private rp::util::noncopyable {
    public:
        Font(const std::string& fontFace, float fontSize);
        ~Font();
        
        Size measure(const std::string& text);
        std::shared_ptr<Image> render(const std::string& text, Color color);
        std::shared_ptr<Image> render(const std::string& text, Color color, Size size);
        
    private:
        FT_Face font_;
        float size_;
    };
    
}}
