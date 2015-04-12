//
//  display_object.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/15/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <rp/ui/display_object.h>
#include <rp/util/noncopyable.h>

using namespace std;
using namespace rp::util;

namespace rp { namespace ui {
    
    IDisplayObjectVisitor::IDisplayObjectVisitor() {}
    IDisplayObjectVisitor::~IDisplayObjectVisitor() {}
    
    DisplayObject::DisplayObject()
    : frame_(0, 0, 0, 0), visible_(true), touchEnabled_(true), touchChildren_(false), isDirty_(false)
    {}
    DisplayObject::~DisplayObject() {}
    
    int DisplayObject::getX() const {
        return frame_.position.x;
    }
    void DisplayObject::setX(int v) {
        if (v != frame_.position.x)
            setPosition(Point(v, frame_.position.y));
    }
    
    int DisplayObject::getY() const {
        return frame_.position.y;
    }
    void DisplayObject::setY(int v) {
        if (v != frame_.position.y)
            setPosition(Point(frame_.position.x, v));
    }
    
    Point DisplayObject::getPosition() const {
        return frame_.position;
    }
    void DisplayObject::setPosition(Point v) {
        if (v != frame_.position)
            setFrame(Rectangle(v, frame_.size));
    }
    
    int DisplayObject::getWidth() const {
        return frame_.size.width;
    }
    void DisplayObject::setWidth(int v) {
        if (v != frame_.size.width)
            setSize(Size(v, frame_.size.height));
    }
    
    int DisplayObject::getHeight() const {
        return frame_.size.height;
    }
    void DisplayObject::setHeight(int v) {
        if (v != frame_.size.height)
            setSize(Size(frame_.size.width, v));
    }
    
    Size DisplayObject::getSize() const {
        return frame_.size;
    }
    void DisplayObject::setSize(Size v) {
        if (v != frame_.size)
            setFrame(Rectangle(frame_.position, v));
    }
    
    Rectangle DisplayObject::getFrame() const {
        return frame_;
    }
    void DisplayObject::setFrame(Rectangle v) {
        lock_guard<mutex> guard(lock_);
        
        if (v == frame_)
            return;
        
        if (v.size != frame_.size) {
            onResizing(frame_.size, v.size);
        }
        
        markDirty(unions(frame_, v));
        
        frame_ = v;
    }
    
    bool DisplayObject::getVisible() const {
        return visible_;
    }
    
    void DisplayObject::setVisible(bool v) {
        lock_guard<mutex> guard(lock_);
        
        if (v != visible_) {
            visible_ = v;
            markDirty(frame_);
        }
    }
    
    bool DisplayObject::getTouchEnabled() const {
        return touchEnabled_;
    }
    void DisplayObject::setTouchEnabled(bool v) {
        lock_guard<mutex> guard(lock_);
        
        touchEnabled_ = v;
    }
    
    bool DisplayObject::getTouchChildren() const {
        return touchChildren_;
    }
    void DisplayObject::setTouchChildren(bool v) {
        lock_guard<mutex> guard(lock_);
        
        touchChildren_ = v;
    }
    
    void DisplayObject::setOnTouch(std::function<void (const TouchEvent &)> callback) {
        onTouchCallback_ = callback;
    }
    
    void DisplayObject::invokeOnTouch(const rp::ui::TouchEvent &ev) {
        if (onTouchCallback_) {
            onTouchCallback_(ev);
        }
    }
    
    void DisplayObject::onResizing(const rp::ui::Size &oldSize, const rp::ui::Size &newSize) {
        
    }
    
    bool DisplayObject::isDirty() const {
        return isDirty_;
    }
    Rectangle DisplayObject::getDirtyFrame() const {
        return isDirty_ ? dirtyFrame_ : Rectangle(0, 0, 0, 0);
    }
    void DisplayObject::clearDirtyStatus() {
        isDirty_ = false;
    }
    
    void DisplayObject::markDirty(Rectangle rect) {
        if (rect.isEmpty())
            return;
        
        if (isDirty_) {
            dirtyFrame_ = unions(dirtyFrame_, rect);
        } else {
            isDirty_ = true;
            dirtyFrame_ = rect;
        }
    }
    
    bool DisplayObject::isDisplayObjectContainer() const {
        return false;
    }
    
    void DisplayObject::update() {
    }
    
}}
