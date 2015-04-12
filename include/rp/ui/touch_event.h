//
//  touch_event.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/3/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#pragma once

#include <memory>
#include <rp/ui/geometry.h>

namespace rp { namespace ui {
    
    class DisplayObject;
    
    enum TouchEventType {
        TouchEventTypeBegan,
        TouchEventTypeEnd,
        TouchEventTypeMove,
        TouchEventTypeCancel
    };
    
    struct TouchEvent {
        std::shared_ptr<DisplayObject> target;
        TouchEventType type;
        Point screenPosition;
        Point position;
        
        Point lastScreenPosition;
        Point lastPosition;
    };
    
}}
