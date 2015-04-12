//
//  touch_manager.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/12/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#include <rp/ui/touch_manager.h>
#include <rp/ui/touch_event.h>
#include <rp/ui/display_object_container.h>
#include <algorithm>

using namespace std;

namespace rp { namespace ui {
    
    TouchManager::TouchManager()
    {}
    
    TouchManager::~TouchManager()
    {}
    
    void TouchManager::dispatch(std::shared_ptr<DisplayObject> cont, rp::ui::TouchEventType type, rp::ui::Point position) {
        if (type == TouchEventTypeBegan) {
            TouchEvent touchEvent = generateTouchBeganEvent_(cont, position);
            lastEvent_ = touchEvent;
            
            if (touchEvent.target) {
                touchEvent.target->invokeOnTouch(touchEvent);
            }
        } else if (type == TouchEventTypeMove) {
            Point offset = position - lastEvent_.screenPosition;
            
            TouchEvent touchEvent;
            touchEvent.type = TouchEventTypeMove;
            touchEvent.target = lastEvent_.target;
            touchEvent.lastScreenPosition = lastEvent_.screenPosition;
            touchEvent.lastPosition = lastEvent_.position;
            touchEvent.screenPosition = position;
            touchEvent.position = lastEvent_.position + offset;
            
            lastEvent_ = touchEvent;
            touchEvent.target->invokeOnTouch(touchEvent);
        } else if (lastEvent_.target) {
            TouchEvent touchEvent = lastEvent_;
            touchEvent.type = type;
            touchEvent.lastPosition = touchEvent.position;
            touchEvent.lastScreenPosition = touchEvent.lastScreenPosition;
            
            lastEvent_ = touchEvent;
            lastEvent_.target = nullptr;
            touchEvent.target->invokeOnTouch(touchEvent);
        }
    }
    
    void TouchManager::dispatch(std::shared_ptr<DisplayObject> cont, bool touching, rp::ui::Point position) {
        if (touching) {
            if (!lastEvent_.target) {
                // touch began
                dispatch(cont, TouchEventTypeBegan, position);
            } else {
                // touch move
                dispatch(cont, TouchEventTypeMove, position);
            }
        } else {
            dispatch(cont, TouchEventTypeEnd, position);
        }
    }
    
    TouchEvent TouchManager::generateTouchBeganEvent_(std::shared_ptr<DisplayObject> cont, Point screenPosition) {
        TouchEvent output;
        output.type = TouchEventTypeBegan;
        output.lastScreenPosition = screenPosition;
        output.screenPosition = screenPosition;
        
        if (!generateTouchBeganEvent_(cont, output, screenPosition)) {
            output.target = nullptr;
        }
        
        return output;
    }
    
    bool TouchManager::generateTouchBeganEvent_(shared_ptr<DisplayObject> displayObject, TouchEvent& touchEvent, Point position) {
        if (!displayObject) {
            return false;
        }
        
        if (!displayObject->getVisible())
            return false;
        
        Point offset = position - displayObject->getPosition();
        
        if (displayObject->getTouchChildren() && displayObject->isDisplayObjectContainer()) {
            shared_ptr<DisplayObjectContainer> container = dynamic_pointer_cast<DisplayObjectContainer>(displayObject);
            
            if (container) {
                auto children = container->getChildren();
                reverse(children.begin(), children.end());
                
                for (auto& child : children) {
                    if (generateTouchBeganEvent_(child, touchEvent, offset)) {
                        return true;
                    }
                }
            }
        }
        
        if (displayObject->getTouchEnabled()) {
            if (isIntersect(position, displayObject->getFrame())) {
                touchEvent.target = displayObject;
                touchEvent.position = offset;
                touchEvent.lastPosition = offset;
                return true;
            }
        }
        
        return false;
    }
    
}}
