//
//  image_loader.h
//  Base class of image loader
//
//  Created by Tony Huang on 12/21/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/util/noncopyable.h>
#include <rp/ui/enums.h>
#include <memory>

namespace rp { namespace util {
    
    class Buffer;
    
}}


namespace rp { namespace ui {
    
    class Image;
    
    struct ImageMetadata {
        int width;
        int height;
        PixelFormat pixelFormat;
    };
    
    class IImageLoader : public rp::util::noncopyable {
    public:
        IImageLoader();
        virtual ~IImageLoader();
        
        virtual bool canLoad(std::shared_ptr<rp::util::Buffer> buffer) = 0;
        virtual ImageMetadata loadMetadata(std::shared_ptr<rp::util::Buffer> buffer) = 0;
        virtual std::shared_ptr<Image> loadImage(std::shared_ptr<rp::util::Buffer> buffer) = 0;
    };
    
}}
