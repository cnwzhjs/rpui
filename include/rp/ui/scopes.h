//
//  scopes.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/22/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/ui/image.h>
#include <rp/util/noncopyable.h>

namespace rp { namespace ui {
    
    template<typename PixelT>
    class ImageLockScope : public rp::util::noncopyable {
    public:
        ImageLockScope(std::shared_ptr<Image> image) : image_(image) {
            buffer_ = reinterpret_cast<PixelT*>(image->lock());
        }
        ~ImageLockScope() {
            image_->unlock(buffer_);
        }
        
        PixelT* getBuffer() {
            return buffer_;
        }
        
        PixelT* getBuffer(size_t offset) {
            return buffer_ + offset;
        }
        
        PixelT* getBuffer(int x, int y) {
            return getBuffer((size_t)(y*image_->getWidth() + x));
        }
        
    private:
        PixelT* buffer_;
        std::shared_ptr<Image> image_;
    };
    
}}
