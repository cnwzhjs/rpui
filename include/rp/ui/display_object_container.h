//
//  display_object_container.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/15/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <rp/ui/display_object.h>

namespace rp { namespace ui {
    
    class DisplayObjectContainer : public DisplayObject {
    public:
        DisplayObjectContainer();
        virtual ~DisplayObjectContainer();
        
        void appendChild(std::shared_ptr<DisplayObject> o);
        void insertChild(size_t position, std::shared_ptr<DisplayObject> o);
        void removeChild(std::shared_ptr<DisplayObject> o);
        void clearChildren();
        
        std::list<std::shared_ptr<DisplayObject>> getChildren();
        
        virtual void clearDirtyStatus();
        virtual void apply(std::shared_ptr<IDisplayObjectVisitor> visitor);
        
        virtual bool isDisplayObjectContainer() const;
        
        virtual void update();
        
    private:
        std::mutex lock_;
        std::list<std::shared_ptr<DisplayObject>> children_;
    };
    
}}
