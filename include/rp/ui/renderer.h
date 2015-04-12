//
//  renderer.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/15/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <memory>
#include <rp/ui/display_object.h>

namespace rp { namespace ui {

    class DeviceContext;
    class DirtyFrameDetector;
    
    class Renderer : public IDisplayObjectVisitor, public std::enable_shared_from_this<Renderer> {
    public:
        Renderer(std::shared_ptr<DeviceContext> context);
        virtual ~Renderer();
        
        virtual void visit(std::shared_ptr<DisplayObject> o);
        virtual void visit(std::shared_ptr<DisplayObjectContainer> o);
        virtual void visit(std::shared_ptr<Pane> o);
        virtual void visit(std::shared_ptr<Scene> o);
        virtual void visit(std::shared_ptr<PictureBox> o);
        virtual void visit(std::shared_ptr<Label> o);
        
    private:
        std::shared_ptr<DeviceContext> device_;
        std::shared_ptr<DirtyFrameDetector> dirtyFrameDetector_;
        Rectangle renderFrame_;
        Rectangle absoluteFrame_;
        Rectangle dirtyFrame_;
    };
    
}}
