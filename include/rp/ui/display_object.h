//
//  display_object.h
//  RoboPeak UI Display Object
//
//  Created by Tony Huang on 12/14/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <memory>
#include <mutex>
#include <rp/ui/signal.h>
#include <rp/ui/geometry.h>
#include <rp/ui/touch_event.h>
#include <rp/util/noncopyable.h>

namespace rp { namespace ui {
    
    class Graphics;
    class DisplayObject;
    class DisplayObjectContainer;
    class Scene;
    class Pane;
    class PictureBox;
    class Label;
    
    class IDisplayObjectVisitor : public rp::util::noncopyable {
    public:
        IDisplayObjectVisitor();
        virtual ~IDisplayObjectVisitor();
        
        virtual void visit(std::shared_ptr<DisplayObject> o) = 0;
        virtual void visit(std::shared_ptr<DisplayObjectContainer> o) = 0;
        virtual void visit(std::shared_ptr<Scene> o) = 0;
        virtual void visit(std::shared_ptr<Pane> o) = 0;
        virtual void visit(std::shared_ptr<PictureBox> o) = 0;
        virtual void visit(std::shared_ptr<Label> o) = 0;
    };
    
    class DisplayObject : public rp::util::noncopyable, public std::enable_shared_from_this<DisplayObject> {
    public:
        DisplayObject();
        virtual ~DisplayObject();
        
        int getX() const;
        void setX(int v);
        
        int getY() const;
        void setY(int v);
        
        Point getPosition() const;
        void setPosition(Point v);
        
        int getWidth() const;
        void setWidth(int v);
        
        int getHeight() const;
        void setHeight(int v);
        
        Size getSize() const;
        void setSize(Size v);
        
        Rectangle getFrame() const;
        void setFrame(Rectangle v);
        
        bool getVisible() const;
        void setVisible(bool v);
        
        bool getTouchEnabled() const;
        void setTouchEnabled(bool v);
        
        bool getTouchChildren() const;
        void setTouchChildren(bool v);
        
        void setOnTouch(std::function<void(const TouchEvent&)> callback);
        virtual void invokeOnTouch(const TouchEvent& ev);
        virtual void onResizing(const Size& oldSize, const Size& newSize);
        
        virtual bool isDirty() const;
        virtual Rectangle getDirtyFrame() const;
        virtual void clearDirtyStatus();
        
        virtual bool isDisplayObjectContainer() const;
        
        virtual void update();
        
        virtual void apply(std::shared_ptr<IDisplayObjectVisitor> visitor) = 0;
        
    protected:
        void markDirty(Rectangle rect);
        
    private:
        std::mutex lock_;
        
        Rectangle frame_;
        bool visible_;
        bool touchEnabled_;
        bool touchChildren_;

        bool isDirty_;
        Rectangle dirtyFrame_;
        
        std::function<void(const TouchEvent&)> onTouchCallback_;
    };
    
}}
