//
//  renderer.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/16/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <rp/ui/renderer.h>
#include <rp/ui/device_context.h>
#include <rp/ui/display_object.h>
#include <rp/ui/display_object_container.h>
#include <rp/ui/dirty_frame_detector.h>
#include <rp/ui/pane.h>
#include <rp/ui/picture_box.h>
#include <rp/ui/label.h>
#include <rp/ui/scene.h>
#include <rp/ui/image.h>

using namespace std;
using namespace rp::util;

namespace rp { namespace ui {

    class ClipFrameScope : noncopyable {
    public:
        ClipFrameScope(Rectangle& target, Rectangle& absoluteFrame, Rectangle clipFrame)
            : target_(target), absoluteFrame_(absoluteFrame)
            , original_(target), originalAbsoluteFrame_(absoluteFrame)
        {
            target = intersects(target, clipFrame);
            absoluteFrame = clipFrame;
        }
        
        ~ClipFrameScope() {
            target_ = original_;
            absoluteFrame_ = originalAbsoluteFrame_;
        }
        
    private:
        Rectangle& target_;
        Rectangle& absoluteFrame_;
        Rectangle original_;
        Rectangle originalAbsoluteFrame_;
    };
    
    Renderer::Renderer(shared_ptr<DeviceContext> context) : device_(context), dirtyFrameDetector_(new DirtyFrameDetector()) {
        renderFrame_ = Rectangle(0, 0, context->getWidth(), context->getHeight());
    }
    Renderer::~Renderer() {}
    
    void Renderer::visit(std::shared_ptr<DisplayObject> o) {
        // do nothing
    }
    
    void Renderer::visit(shared_ptr<DisplayObjectContainer> c) {
        if (!c->getVisible()) {
            return;
        }
        
        ClipFrameScope scope(renderFrame_, absoluteFrame_, c->getFrame().offset(absoluteFrame_.position));
        
        if (renderFrame_.isEmpty()) {
            return;
        }
        
        Rectangle drawFrame = intersects(renderFrame_, dirtyFrame_);
        
        if (drawFrame.isEmpty()) {
            return;
        }
        
        for (auto& child : c->getChildren()) {
            child->apply(shared_from_this());
        }
    }
    
    void Renderer::visit(shared_ptr<Pane> o) {
        if (!o->getVisible()) {
            return;
        }
        
        ClipFrameScope scope(renderFrame_, absoluteFrame_, o->getFrame().offset(absoluteFrame_.position));
        
        if (renderFrame_.isEmpty()) {
            return;
        }
        
        Rectangle drawFrame = intersects(renderFrame_, dirtyFrame_);
        
        if (!drawFrame.isEmpty()) {
            device_->fill(drawFrame, o->getBackgroundColor());
        }
    }
    
    void Renderer::visit(shared_ptr<Scene> o) {
        o->apply(dirtyFrameDetector_);
        dirtyFrame_ = dirtyFrameDetector_->getDirtyFrame();
        o->clearDirtyStatus();
        
        if (dirtyFrame_.isEmpty()) return;
        
//        printf("Dirty frame: (%d, %d) %dx%d\n", dirtyFrame_.position.x, dirtyFrame_.position.y, dirtyFrame_.size.width, dirtyFrame_.size.height);
        
        Rectangle clearFrame = intersects(o->getFrame(), dirtyFrame_);
        if (!clearFrame.isEmpty()) {
            device_->fill(clearFrame, o->getBackgroundColor());
        }
        visit(dynamic_pointer_cast<DisplayObjectContainer>(o));
    }
    
    void Renderer::visit(shared_ptr<PictureBox> o) {
        if (!o->getVisible()) {
            return;
        }
        
        if (!o->getImage()) {
            return;
        }
        
        ClipFrameScope scope(renderFrame_, absoluteFrame_, o->getFrame().offset(absoluteFrame_.position));
        
        if (renderFrame_.isEmpty()) {
            return;
        }
        
        Rectangle drawFrame = intersects(renderFrame_, dirtyFrame_);
        
        if (!drawFrame.isEmpty()) {
            device_->bitblt(o->getImage(), drawFrame.offset(-absoluteFrame_.position), drawFrame.position, BitOperationCopy);
        }
    }
    
    static int calcLabelOffset(int containerSize, int contentSize, TextAlignment alignment) {
        switch (alignment) {
            case TextAlignmentNear:
                return 0;
            case TextAlignmentCenter:
                return (containerSize - contentSize)/2;
            case TextAlignmentFar:
                return containerSize - contentSize;
            default:
                break;
        }
    }
    
    void Renderer::visit(shared_ptr<Label> o) {
        if (!o->getVisible())
            return;
        
        auto cachedImage = o->getRenderedImage();
        
        if (!cachedImage)
            return;
        
        ClipFrameScope scope(renderFrame_, absoluteFrame_, o->getFrame().offset(absoluteFrame_.position));
        
        if (renderFrame_.isEmpty())
            return;
        
        Rectangle drawFrame = intersects(renderFrame_, dirtyFrame_);
        if (drawFrame.isEmpty())
            return;
        
        int offsetX = calcLabelOffset(o->getWidth(), cachedImage->getWidth(), o->getHorizontalAlignment());
        int offsetY = calcLabelOffset(o->getHeight(), cachedImage->getHeight(), o->getVerticalAlignment());
        
        if (!drawFrame.isEmpty()) {
            device_->bitblt(cachedImage, drawFrame.offset(-absoluteFrame_.position).offset(Size(-offsetX, -offsetY)), drawFrame.position, BitOperationCopy);
        }
    }
    
}}
