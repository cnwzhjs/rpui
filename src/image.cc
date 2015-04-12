//
//  image.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/21/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <rp/ui/image.h>
#include <rp/util/buffer.h>
#include <rp/util/exception.h>
#include <rp/util/scopes.h>

using namespace std;
using namespace rp::util;

namespace rp { namespace ui {
    
    // TODO: should be moved to seperate file
    static size_t getBytesPerPixel(PixelFormat pixelFormat) {
        switch (pixelFormat) {
            case PixelFormatRGB8:
                return 3;
            case PixelFormatBGR565:
                return 2;
            case PixelFormatRGBA8:
                return 4;
            default:
                throw Exception((int)pixelFormat, "Unknown pixel format");
        }
    }
    
    static bool doesPixelFormatHasAlphaChannel(PixelFormat pixelFormat) {
        return (pixelFormat & PixelFormatMaskAlphaChannel) == PixelFormatMaskAlphaChannel;
    }
    
    class ImageImpl {
    public:
        enum ImageOpaque {
            ImageOpaqueUnknown,
            ImageOpaqueTransparent,
            ImageOpaqueOpaque
        };
        
        ImageImpl(int width, int height, PixelFormat pixelFormat)
        : width_(width), height_(height), pixelFormat_(pixelFormat)
        {
            size_t pixelCount = (size_t)(width*height);
            size_t totalBytes = pixelCount * getBytesPerPixel(pixelFormat);
            
            buffer_ = shared_ptr<Buffer>(new Buffer(totalBytes));
        }
        
        ImageImpl(int width, int height, PixelFormat pixelFormat, shared_ptr<Buffer> buffer)
        : width_(width), height_(height), pixelFormat_(pixelFormat), buffer_(buffer)
        {}
        
        ~ImageImpl() {}
        
        int getWidth() const {
            return width_;
        }
        
        int getHeight() const {
            return height_;
        }
        
        PixelFormat getPixelFormat() const {
            return pixelFormat_;
        }
        
        bool hasAlphaChannel() const {
            return doesPixelFormatHasAlphaChannel(pixelFormat_);
        }
        
        bool isOpaque() {
            if (!hasAlphaChannel())
                return true;
            
            if (opaque_ != ImageOpaqueUnknown)
                return opaque_ == ImageOpaqueOpaque;
            
            if (pixelFormat_ == PixelFormatRGBA8) {
                int pixelCount = width_ * height_;
                
                BufferLockScope lockScope(buffer_);
                
                unsigned char* p = (unsigned char*)lockScope.getBuffer();
                
                for (int i = 0; i < pixelCount; i++, p+=4) {
                    if (p[3] != 0xffu) {
                        opaque_ = ImageOpaqueTransparent;
                        return false;
                    }
                }
            }
            
            opaque_ = ImageOpaqueOpaque;
            return true;
        }
        
        void* lock() {
            return buffer_->lock();
        }
        
        void unlock(void* buffer) {
            buffer_->unlock(buffer);
        }
        
    private:
        int width_;
        int height_;
        ImageOpaque opaque_;
        PixelFormat pixelFormat_;
        std::shared_ptr<Buffer> buffer_;
    };
    
    Image::Image(int width, int height, PixelFormat pixelFormat) : impl_(new ImageImpl(width, height, pixelFormat)) {}
    Image::Image(int width, int height, PixelFormat pixelFormat, shared_ptr<Buffer> buffer) : impl_(new ImageImpl(width, height, pixelFormat, buffer)) {}
    Image::~Image() {}
    
    int Image::getWidth() const {
        return impl_->getWidth();
    }
    
    int Image::getHeight() const {
        return impl_->getHeight();
    }
    
    PixelFormat Image::getPixelFormat() const {
        return impl_->getPixelFormat();
    }
    
    bool Image::hasAlphaChannel() const {
        return impl_->hasAlphaChannel();
    }
    
    bool Image::isOpaque() {
        return impl_->isOpaque();
    }
    
    void* Image::lock() {
        return impl_->lock();
    }
    
    void Image::unlock(void *buffer) {
        impl_->unlock(buffer);
    }
    
}}

