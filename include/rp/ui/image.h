//
//  image.h
//  Image
//
//  Created by Tony Huang on 12/21/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <memory>
#include <rp/ui/enums.h>
#include <rp/util/noncopyable.h>

namespace rp { namespace util {
    
    class Buffer;
    
}}


namespace rp { namespace ui {
    
    class ImageImpl;
    
    class Image : public rp::util::noncopyable {
    public:
        Image(int width, int height, PixelFormat pixelFormat);
        Image(int width, int height, PixelFormat pixelFormat, std::shared_ptr<rp::util::Buffer> buffer);
        ~Image();
        
        int getWidth() const;
        int getHeight() const;
        PixelFormat getPixelFormat() const;
        bool hasAlphaChannel() const;
        
        bool isOpaque();
        
        void* lock();
        void unlock(void* buffer);
        
    private:
        std::unique_ptr<ImageImpl> impl_;
    };
    
}}
