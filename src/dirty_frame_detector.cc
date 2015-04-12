//
//  dirty_frame_detector.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/16/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <rp/ui/dirty_frame_detector.h>
#include <rp/ui/display_object.h>
#include <rp/ui/display_object_container.h>
#include <rp/ui/scene.h>
#include <rp/ui/pane.h>
#include <rp/ui/picture_box.h>
#include <rp/ui/label.h>

using namespace std;

namespace rp { namespace ui {
    
    class BasePointScope {
    public:
        BasePointScope(Point& target, Point offset) : target_(target), original_(target) {
            target_ = target + offset;
        }
        ~BasePointScope() {
            target_ = original_;
        }
        
    private:
        Point& target_;
        Point original_;
    };

    DirtyFrameDetector::DirtyFrameDetector() {}
    DirtyFrameDetector::~DirtyFrameDetector() {}
    
    void DirtyFrameDetector::visit(shared_ptr<DisplayObject> o) {
        if (o->isDirty()) {
            dirtyFrame_ = unions(dirtyFrame_, o->getDirtyFrame().offset(basePoint_));
        }
    }
    
    void DirtyFrameDetector::visit(shared_ptr<DisplayObjectContainer> o) {
        if (o->isDirty()) {
            dirtyFrame_ = unions(dirtyFrame_, o->getDirtyFrame().offset(basePoint_));
        } else {
            BasePointScope scope(basePoint_, o->getPosition());
            
            for (auto& child : o->getChildren()) {
                child->apply(shared_from_this());
            }
        }
    }
    
    void DirtyFrameDetector::visit(shared_ptr<Scene> o) {
        basePoint_ = Point(0, 0);
        dirtyFrame_ = Rectangle();
        
        visit(dynamic_pointer_cast<DisplayObjectContainer>(o));
    }
    
    void DirtyFrameDetector::visit(shared_ptr<Pane> o) {
        visit(dynamic_pointer_cast<DisplayObject>(o));
    }
    
    void DirtyFrameDetector::visit(shared_ptr<PictureBox> o) {
        visit(dynamic_pointer_cast<DisplayObject>(o));
    }
    
    void DirtyFrameDetector::visit(shared_ptr<Label> o) {
        visit(dynamic_pointer_cast<DisplayObject>(o));
    }
    
    Rectangle DirtyFrameDetector::getDirtyFrame() const {
        return dirtyFrame_;
    }
    
}}

