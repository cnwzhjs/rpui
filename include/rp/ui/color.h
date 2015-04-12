//
//  color.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/14/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/util/int_types.h>

namespace rp { namespace ui {
  
    class Color {
    public:
        Color();
        Color(const Color&);
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        ~Color();
        
        uint32_t toABGR32() const;
        uint16_t toBGR565() const;
        
        static Color fromABGR32(uint32_t);
        static Color fromRGBA32(uint32_t);
        static Color fromBGR565(uint16_t);
        
        uint8_t a;
        uint8_t b;
        uint8_t g;
        uint8_t r;
    };
    
    bool operator==(const Color& c1, const Color& c2);
    bool operator!=(const Color& c1, const Color& c2);
    Color operator*(const Color& c, float v);
    Color operator+(const Color& c1, const Color& c2);
    
    Color blend(const Color& bgColor, const Color& fgColor);
    
    namespace colors {
        
        extern Color white;
        extern Color black;
        extern Color silver;
        extern Color gray;
        extern Color red;
        extern Color green;
        extern Color blue;
        extern Color yellow;
        extern Color purple;
        extern Color cyan;
        
    }
    
}}
