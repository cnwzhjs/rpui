//
//  pane.h
//  Pane
//
//  Created by Tony Huang on 12/15/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/ui/color.h>
#include <rp/ui/display_object.h>

namespace rp { namespace ui {

    class Pane : public DisplayObject {
    public:
        Pane();
        ~Pane();
        
        Color getBackgroundColor() const;
        void setBackgroundColor(Color c);
        
        virtual void apply(std::shared_ptr<IDisplayObjectVisitor> visitor);
        
    private:
        Color backgroundColor_;
    };
    
}}
