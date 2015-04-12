//
//  touch_manager.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/12/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/ui/touch_event.h>
#include <rp/ui/geometry.h>
#include <memory>

namespace rp { namespace ui {

    class DisplayObjectContainer;
    
    class TouchManager {
    public:
        TouchManager();
        ~TouchManager();
        
        void dispatch(std::shared_ptr<DisplayObject> cont, TouchEventType type, Point position);
        void dispatch(std::shared_ptr<DisplayObject> cont, bool touching, Point position);
      
    private:
        TouchEvent generateTouchBeganEvent_(std::shared_ptr<DisplayObject> cont, Point position);
        bool generateTouchBeganEvent_(std::shared_ptr<DisplayObject> displayObject, TouchEvent& touchEvent, Point position);
        
        TouchEvent lastEvent_;
    };
    
}}
