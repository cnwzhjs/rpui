//
//  color.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/16/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <string.h>
#include <rp/ui/color.h>
#include <rp/util/endian.h>

using namespace rp::util;

namespace rp { namespace ui {
    
    Color::Color() : a(0), b(0), g(0), r(0) {}
    Color::Color(const Color& that) : a(that.a), b(that.b), g(that.g), r(that.r) {}
    Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : a(a), b(b), g(g), r(r) {}
    Color::~Color() {}
    
    uint32_t Color::toABGR32() const {
        return *reinterpret_cast<uint32_t*>((void*)this);
    }
    
    uint16_t Color::toBGR565() const {
        return cpu_to_le16((uint16_t)((b & 0xf8u)<<8)|((g & 0xfcu)<<3)|((r & 0xf8u)>>3));
    }
    
    Color Color::fromABGR32(uint32_t c) {
        Color output;
        memcpy(&output, &c, 4);
        return output;
    }
    
    Color Color::fromRGBA32(uint32_t c) {
        c = be32_to_le32(c);
        return fromABGR32(c);
    }
    
    Color Color::fromBGR565(uint16_t c) {
        c = le16_to_cpu(c);
        uint8_t b = (uint8_t)((float)(c >> 11) * 255.0f / 31);
        uint8_t g = (uint8_t)((float)((c >> 5) & 0x3fu) * 255.0f / 63);
        uint8_t r = (uint8_t)((float)(c & 0x1fu) * 255.0f / 31);
        return Color(r, g, b, 0xffu);
    }
    
    bool operator==(const Color& c1, const Color& c2) {
        return (c1.a == c2.a) && (c1.b == c2.b) && (c1.g == c2.g) && (c1.r == c2.r);
    }
    
    bool operator!=(const Color& c1, const Color& c2) {
        return (c1.a != c2.a) || (c1.b != c2.b) || (c1.g != c2.g) || (c1.r != c2.r);
    }
    
    Color operator*(const Color& c, float v) {
        return Color((uint8_t)(c.r*v),(uint8_t)(c.g*v),(uint8_t)(c.b*v),(uint8_t)(c.a*v));
    }
    
    Color operator+(const Color& c1, const Color& c2) {
        return Color(c1.r+c2.r, c1.g+c2.g, c1.b+c2.b, c1.a+c2.a);
    }
    
    Color blend(const Color& bgColor, const Color& fgColor) {
        if (fgColor.a == 0)
            return bgColor;
        
        if (fgColor.a == 255)
            return fgColor;
        
        int alpha = fgColor.a;
        int oneMinusAlpha = 255 - alpha;
        
        int r = (bgColor.r * oneMinusAlpha + fgColor.r * alpha)/255;
        int g = (bgColor.g * oneMinusAlpha + fgColor.g * alpha)/255;
        int b = (bgColor.b * oneMinusAlpha + fgColor.b * alpha)/255;
        int a = (bgColor.a * oneMinusAlpha + fgColor.a * alpha)/255;
        
        return Color((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
    }
    
    namespace colors {
        
        Color white(0xffu, 0xffu, 0xffu, 0xffu);
        Color black(0x00u, 0x00u, 0x00u, 0xffu);
        Color silver(0xccu, 0xccu, 0xccu, 0xffu);
        Color gray(0x33u, 0x33u, 0x33u, 0xffu);
        Color red(0xffu, 0x00u, 0x00u, 0xffu);
        Color green(0x00u, 0xffu, 0x00u, 0xffu);
        Color blue(0x00u, 0x00u, 0xffu, 0xffu);
        Color yellow(0xffu, 0xffu, 0x00u, 0xffu);
        Color purple(0xffu, 0x00u, 0xffu, 0xffu);
        Color cyan(0x00u, 0xffu, 0xffu, 0xffu);
        
    }
    
}}
