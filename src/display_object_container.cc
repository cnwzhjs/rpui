//
//  display_object_container.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/15/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <rp/ui/display_object_container.h>

using namespace std;

namespace rp { namespace ui {
    
    DisplayObjectContainer::DisplayObjectContainer()
    {
        setTouchChildren(true);
    }
    
    DisplayObjectContainer::~DisplayObjectContainer() {}
    
    void DisplayObjectContainer::appendChild(shared_ptr<DisplayObject> o) {
        lock_guard<mutex> guard(lock_);
        children_.push_back(o);

        markDirty(getFrame());
    }
    
    void DisplayObjectContainer::insertChild(size_t position, std::shared_ptr<DisplayObject> o) {
        lock_guard<mutex> guard(lock_);
        
        auto p = children_.begin();
        for (size_t i = 0; i < position; i++) {
            p++;
        }
        
        children_.insert(p, o);

        markDirty(getFrame());
    }
    
    void DisplayObjectContainer::removeChild(std::shared_ptr<DisplayObject> o) {
        lock_guard<mutex> guard(lock_);
        children_.remove(o);

        markDirty(getFrame());
    }
    
    void DisplayObjectContainer::clearChildren() {
        lock_guard<mutex> guard(lock_);
        children_.clear();

        markDirty(getFrame());
    }
    
    list<shared_ptr<DisplayObject>> DisplayObjectContainer::getChildren() {
        lock_guard<mutex> guard(lock_);
        return children_;
    }
    
    void DisplayObjectContainer::clearDirtyStatus() {
        lock_guard<mutex> guard(lock_);
        
        DisplayObject::clearDirtyStatus();
        
        for (auto& child : children_) {
            child->clearDirtyStatus();
        }
    }
    
    void DisplayObjectContainer::apply(shared_ptr<IDisplayObjectVisitor> visitor) {
        visitor->visit(dynamic_pointer_cast<DisplayObjectContainer>(shared_from_this()));
    }
    
    bool DisplayObjectContainer::isDisplayObjectContainer() const {
        return true;
    }
    
    void DisplayObjectContainer::update() {
        list<shared_ptr<DisplayObject> > children;
        
        {
            lock_guard<mutex> guard(lock_);
            children = children_;
        }
        
        for (auto child : children) {
            child->update();
        }
    }
    
}}
