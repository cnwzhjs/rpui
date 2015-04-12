//
//  device.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/14/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <functional>
#include <memory>
#include <rp/util/noncopyable.h>
#include <rp/ui/color.h>
#include <rp/ui/geometry.h>
#include <rp/ui/enums.h>

namespace rp { namespace drivers { namespace display {
    
    class RoboPeakUsbDisplayDevice;
    
}}}

namespace rp { namespace ui {
    
    class DeviceContextImpl;
    class Image;
    
    class DeviceContext : public rp::util::noncopyable {
    public:
        // TODO: In order to verify the framework ASAP, just hard coded the RoboPeakUsbDisplayDevice, should replace this with a generic interface
        DeviceContext(std::shared_ptr<rp::drivers::display::RoboPeakUsbDisplayDevice> device);
        DeviceContext(std::shared_ptr<Image> image);
        ~DeviceContext();
        
        int getWidth();
        int getHeight();
        
        void clear(Color color);
        void fill(Rectangle rect, Color color);
        void bitblt(std::shared_ptr<Image> image, Rectangle srcRect, Point dest, BitOperation bitOperation);
        
    private:
        std::unique_ptr<DeviceContextImpl> impl_;
    };
    
}}
