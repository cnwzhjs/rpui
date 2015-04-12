//
//  scene.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/14/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <string>
#include <rp/ui/color.h>
#include <rp/ui/display_object_container.h>

namespace rp { namespace ui {
    
    class Scene : public rp::ui::DisplayObjectContainer {
    public:
        Scene(const std::string& name);
        virtual ~Scene();
        
        std::string getName() const;
        void setName(const std::string& newName);
        
        Color getBackgroundColor() const;
        void setBackgroundColor(Color c);
        
        virtual void apply(std::shared_ptr<IDisplayObjectVisitor> visitor);
        
    private:
        Color backgroundColor_;
        std::string name_;
    };
    
}}
