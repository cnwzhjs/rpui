//
//  enums.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/14/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

namespace rp { namespace ui {

    enum BitOperation {
        BitOperationCopy = 0,
        BitOperationXor = 1,
        BitOperationOr = 2,
        BitOperationAnd = 3
    };
    
    enum PixelFormat {
        PixelFormatRGB8 = 0x0000,
        PixelFormatBGR565 = 0x0001,
        PixelFormatRGBA8 = 0x1000,
        
        // TODO: other pixel formats should be added
        
        PixelFormatMaskAlphaChannel = 0x1000,
        
        PixelFormatInvalid = 0xffffu
    };

}}
