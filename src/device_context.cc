//
//  device_context.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/15/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <rp/ui/device_context.h>
#include <rp/ui/image.h>
#include <rp/ui/scopes.h>
#include <rp/drivers/display/rpusbdisp/rpusbdisp.h>
#include <rp/util/noncopyable.h>
#include <string.h>

using namespace std;
using namespace rp::drivers::display;
using namespace rp::util;

namespace rp { namespace ui {
    
    static inline shared_ptr<Image> convertImage(shared_ptr<Image> srcImage, PixelFormat destPixelFormat) {
        if (destPixelFormat != PixelFormatBGR565)
            return nullptr;
        
        shared_ptr<Image> outputImage(new Image(srcImage->getWidth(), srcImage->getHeight(), destPixelFormat));
        
        ImageLockScope<uint16_t> outputScope(outputImage);
        
        size_t pixelCount = (size_t)(srcImage->getWidth()*srcImage->getHeight());
        
        uint16_t* pDest = outputScope.getBuffer();

        if (srcImage->getPixelFormat() == PixelFormatRGBA8) {
            ImageLockScope<uint32_t> srcScope(srcImage);
            uint32_t* pSrc = srcScope.getBuffer();
            
            for (size_t i = 0; i < pixelCount; i++, pSrc++, pDest++) {
                *pDest = Color::fromRGBA32(*pSrc).toBGR565();
            }
        } else {
            ImageLockScope<uint8_t> srcScope(srcImage);
            uint8_t* pSrc = srcScope.getBuffer();
            
            for (size_t i = 0; i < pixelCount; i++, pSrc+=3, pDest++) {
                *pDest = Color(pSrc[0], pSrc[1], pSrc[2], 0xffu).toBGR565();
            }
        }
        
        return outputImage;
    }
    
    static inline shared_ptr<Image> cropImage(shared_ptr<Image> srcImage, Rectangle cropRect) {
        if (srcImage->getPixelFormat() != PixelFormatBGR565)
            return nullptr;
        
        shared_ptr<Image> outputImage(new Image(cropRect.size.width, cropRect.size.height, PixelFormatBGR565));
        
        ImageLockScope<uint16_t> srcScope(srcImage);
        ImageLockScope<uint16_t> destScope(outputImage);
        
        memset(destScope.getBuffer(), 0, (size_t)(2*cropRect.size.width*cropRect.size.height));
        
        int startSrcX = cropRect.position.x;
        int startDestX = 0;
        
        int endSrcX = cropRect.getRight() + 1;
        int endDestX = cropRect.size.width;
        
        int startSrcY = cropRect.position.y;
        int startDestY = 0;
        
        int endSrcY = cropRect.getBottom() + 1;
        int endDestY = cropRect.size.height;
        
        if (startSrcX < 0) {
            startDestX -= startSrcX;
            startSrcX = 0;
        }
        
        if (endSrcX > srcImage->getWidth()) {
            endDestX -= endSrcX - srcImage->getWidth();
            endSrcX = srcImage->getWidth();
        }
        
        if (startSrcY < 0) {
            startDestY -= startSrcY;
            startSrcY = 0;
        }
        
        if (endSrcY > srcImage->getHeight()) {
            endDestY -= endSrcY - srcImage->getHeight();
            endSrcY = srcImage->getHeight();
        }
        
        int columns = endSrcX - startSrcX;
        int rows = endSrcY - startSrcY;
        
        if (columns > 0 && rows > 0) {
            size_t rowSize = (size_t)(columns*2);
            for (int row = 0; row < rows; row++) {
                memcpy(destScope.getBuffer(0, startDestY+row), srcScope.getBuffer(startSrcX, startSrcY+row), rowSize);
            }
        }
        
        return outputImage;
    }
    
    class DeviceContextImpl : private noncopyable {
    public:
        DeviceContextImpl() {}
        virtual ~DeviceContextImpl() {}
        
        virtual int getWidth() = 0;
        virtual int getHeight() = 0;
        
        virtual void clear(Color color) = 0;
        virtual void fill(Rectangle rect, Color color) = 0;
        virtual void bitblt(shared_ptr<Image> image, Rectangle srcRect, Point dest, BitOperation bitOperation) = 0;
    };
    
    class UsbDisplayDeviceContextImpl : public DeviceContextImpl {
    public:
        UsbDisplayDeviceContextImpl(shared_ptr<RoboPeakUsbDisplayDevice> device) : device_(device) {
        }
        ~UsbDisplayDeviceContextImpl() {}
        
        int getWidth() {
            return device_->getWidth();
        }
        
        int getHeight() {
            return device_->getHeight();
        }
        
        void clear(Color color) {
            device_->fill(color.toBGR565());
        }
        
        void fill(Rectangle rect, Color color) {
            device_->fillrect(rect.position.x, rect.position.y, rect.getRight(), rect.getBottom(), color.toBGR565(), RoboPeakUsbDisplayBitOperationCopy);
        }
        
        void bitblt(shared_ptr<Image> image, Rectangle srcRect, Point dest, BitOperation bitOperation) {
            if (!image) return;
            if (srcRect.isEmpty()) return;
            
//            printf("Bitblting for image (%dx%d)'s area %dx%d at (%d,%d) to dest (%d,%d)\n",
//                   image->getWidth(), image->getHeight(),
//                   srcRect.size.width, srcRect.size.height,
//                   srcRect.position.x, srcRect.position.y,
//                   dest.x, dest.y);
            
            if (image->getPixelFormat() != PixelFormatBGR565) {
                return bitblt(convertImage(image, PixelFormatBGR565), srcRect, dest, bitOperation);
            }
            
            Rectangle srcBounds(0, 0, image->getWidth(), image->getHeight());
            Rectangle newSrcRect = intersects(srcRect, srcBounds);
            
            if (newSrcRect.size != srcRect.size) {
                return bitblt(image, newSrcRect, dest + newSrcRect.position - srcRect.position, bitOperation);
            }
            
            if (srcRect.position.x != 0 || srcRect.position.y != 0 || srcRect.size.width != image->getWidth() || srcRect.size.height != image->getHeight()) {
                return bitblt(cropImage(image, srcRect), Rectangle(Point(0, 0), srcRect.size), dest, bitOperation);
            }
            
            Rectangle targetBounds(0, 0, device_->getWidth(), device_->getHeight());
            Rectangle targetRect = intersects(Rectangle(dest, srcRect.size), targetBounds);
            
//            printf("target rect: %dx%d at (%d,%d)\n", targetRect.size.width, targetRect.size.height, targetRect.position.x, targetRect.position.y);
            
            if (targetRect.isEmpty()) return;
            
            if (targetRect.size != srcRect.size) {
                Point newSrcPosition(srcRect.position.x + targetRect.position.x - dest.x, srcRect.position.y + targetRect.position.y - dest.y);
                return bitblt(image, Rectangle(newSrcPosition, targetRect.size), targetRect.position, bitOperation);
            }
            
            ImageLockScope<uint16_t> lockScope(image);
            device_->bitblt(dest.x, dest.y, srcRect.size.width, srcRect.size.height, (RoboPeakUsbDisplayBitOperation)bitOperation, lockScope.getBuffer());
        }
        
    private:
        shared_ptr<RoboPeakUsbDisplayDevice> device_;
    };
    
    class ImageDeviceContextImpl : public DeviceContextImpl {
    public:
        ImageDeviceContextImpl(shared_ptr<Image> image)
            : image_(image)
        {
        }
        
        ~ImageDeviceContextImpl()
        {}
        
        virtual int getWidth() {
            return image_->getWidth();
        }
        
        virtual int getHeight() {
            return image_->getHeight();
        }
        
        virtual void clear(Color color) {
            if (image_->getPixelFormat() == PixelFormatBGR565) {
                ImageLockScope<uint16_t> lockScope(image_);
                int pixelCount = image_->getWidth() * image_->getHeight();
                uint16_t* pixel = lockScope.getBuffer();
                for (int i = 0; i < pixelCount; i++) {
                    *pixel++ = color.toBGR565();
                }
            } else {
                // TODO: currently only support BGR565 image target
            }
        }
        
        virtual void fill(Rectangle rect, Color color) {
            if (image_->getPixelFormat() == PixelFormatBGR565) {
                Rectangle targetBounds(0, 0, image_->getWidth(), image_->getHeight());
                Rectangle targetRect = intersects(rect, targetBounds);
                
                if (targetRect.size.width <= 0 || targetRect.size.height <= 0)
                    return;
                
                ImageLockScope<uint16_t> lockScope(image_);
                
                for (int j = 0; j < targetRect.size.height; j++) {
                    for (int i = 0; i < targetRect.size.width; i++) {
                        *lockScope.getBuffer(i + targetRect.position.x, j + targetRect.position.y) = color.toBGR565();
                    }
                }
            } else {
                // TODO: currently only support BGR565 image target
            }
        }
        
        virtual void bitblt(shared_ptr<Image> image, Rectangle srcRect, Point dest, BitOperation bitOperation) {
            if (image_->getPixelFormat() == PixelFormatBGR565) {
                Rectangle targetBounds(0, 0, image_->getWidth(), image_->getHeight());
                Rectangle targetRect(dest, srcRect.size);
                targetRect = intersects(targetRect, targetBounds);
                
                if (targetRect.size.width <= 0 || targetRect.size.height <= 0)
                    return;
                
                if (targetRect.size != srcRect.size) {
                    Point newSrcPosition(srcRect.position.x + targetRect.position.x - dest.x, srcRect.position.y + targetRect.position.y - dest.y);
                    return bitblt(image, Rectangle(newSrcPosition, targetRect.size), targetRect.position, bitOperation);
                }
                
                Rectangle srcBounds(0, 0, image->getWidth(), image->getHeight());
                Rectangle newSrcRect = intersects(srcRect, srcBounds);
                
                if (newSrcRect.size != srcRect.size) {
                    return bitblt(image, newSrcRect, dest + newSrcRect.position - srcRect.position, bitOperation);
                }
                
                if (image->getPixelFormat() == PixelFormatBGR565) {
                    ImageLockScope<uint16_t> srcScope_(image);
                    ImageLockScope<uint16_t> destScope_(image_);
                    
                    // TODO: use memcpy to optimize
                    for (int j = 0; j < targetRect.size.height; j++) {
                        for (int i = 0; i < targetRect.size.width; i++) {
                            int srcX = i + srcRect.position.x;
                            int srcY = j + srcRect.position.y;
                            int destX = i + dest.x;
                            int destY = j + dest.y;
                            
                            switch (bitOperation) {
                                case BitOperationAnd:
                                    *destScope_.getBuffer(destX, destY) &= *srcScope_.getBuffer(srcX, srcY);
                                    break;
                                    
                                case BitOperationCopy:
                                    *destScope_.getBuffer(destX, destY) = *srcScope_.getBuffer(srcX, srcY);
                                    break;
                                    
                                case BitOperationOr:
                                    *destScope_.getBuffer(destX, destY) |= *srcScope_.getBuffer(srcX, srcY);
                                    break;
                                    
                                case BitOperationXor:
                                    *destScope_.getBuffer(destX, destY) ^= *srcScope_.getBuffer(srcX, srcY);
                                    break;
                                    
                                default:
                                    break;
                            }
                        }
                    }
                } else if (image->getPixelFormat() == PixelFormatRGBA8) {
                    if (image->isOpaque()) {
                        return bitblt(convertImage(image, PixelFormatBGR565), srcRect, dest, bitOperation);
                    }
                    
                    ImageLockScope<uint32_t> srcScope_(image);
                    ImageLockScope<uint16_t> destScope_(image_);
                    
                    // TODO: use memcpy to optimize
                    for (int j = 0; j < targetRect.size.height; j++) {
                        for (int i = 0; i < targetRect.size.width; i++) {
                            int srcX = i + srcRect.position.x;
                            int srcY = j + srcRect.position.y;
                            int destX = i + dest.x;
                            int destY = j + dest.y;
                            
                            switch (bitOperation) {
                                case BitOperationCopy:
                                {
                                    Color fgColor = Color::fromRGBA32(*srcScope_.getBuffer(srcX, srcY));
                                    
                                    if (fgColor.a == 0)
                                        continue;
                                    
                                    Color bgColor = Color::fromBGR565(*destScope_.getBuffer(destX, destY));
                                    
                                    if (!fgColor.a)
                                        continue;
                                    
                                    Color composedColor = blend(bgColor, fgColor);
                                    
                                    *destScope_.getBuffer(destX, destY) = composedColor.toBGR565();
                                }
                                    break;
                                    
                                default:
                                    break;
                            }
                        }
                    }
                } else {
                    return bitblt(convertImage(image, PixelFormatBGR565), srcRect, dest, bitOperation);
                }
            } else {
                // TODO: currently only support BGR565 image target
            }
        }
        
    private:
        shared_ptr<Image> image_;
    };
    
    DeviceContext::DeviceContext(shared_ptr<RoboPeakUsbDisplayDevice> device) : impl_(new UsbDisplayDeviceContextImpl(device)) {}
    DeviceContext::DeviceContext(shared_ptr<Image> image) : impl_(new ImageDeviceContextImpl(image)) {}
    DeviceContext::~DeviceContext() {}
    
    int DeviceContext::getWidth() {
        return impl_->getWidth();
    }
    
    int DeviceContext::getHeight() {
        return impl_->getHeight();
    }
    
    void DeviceContext::clear(rp::ui::Color color) {
        impl_->clear(color);
    }
    
    void DeviceContext::fill(Rectangle rect, Color color) {
        impl_->fill(rect, color);
    }
    
    void DeviceContext::bitblt(shared_ptr<Image> image, Rectangle srcRect, Point dest, BitOperation bitOperation) {
        return impl_->bitblt(image, srcRect, dest, bitOperation);
    }
    
}}
