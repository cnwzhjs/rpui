//
//  dirty_frame_detector.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/16/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/ui/geometry.h>
#include <rp/ui/display_object.h>

namespace rp { namespace ui {

    class DirtyFrameDetector : public IDisplayObjectVisitor, public rp::util::noncopyable, public std::enable_shared_from_this<DirtyFrameDetector> {
    public:
        DirtyFrameDetector();
        virtual ~DirtyFrameDetector();
        
        virtual void visit(std::shared_ptr<DisplayObject> o);
        virtual void visit(std::shared_ptr<DisplayObjectContainer> o);
        virtual void visit(std::shared_ptr<Scene> o);
        virtual void visit(std::shared_ptr<Pane> o);
        virtual void visit(std::shared_ptr<PictureBox> o);
        virtual void visit(std::shared_ptr<Label> o);
        
        Rectangle getDirtyFrame() const;
        
    private:
        Point basePoint_;
        Rectangle dirtyFrame_;
    };
    
}}
