//
//  linear_layout.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/4/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/ui/display_object_container.h>

namespace rp { namespace ui {

    enum LinearLayoutOrientation {
        LinearLayoutOrientationHorizontal,
        LinearLayoutOrientationVertical
    };
    
    class LinearLayout : public DisplayObjectContainer {
    public:
        LinearLayout();
        ~LinearLayout();
        
        void relayout();
        
        LinearLayoutOrientation getOrientation() const;
        void setOrientation(LinearLayoutOrientation orientation);
        
        bool getAdjustBounds() const;
        void setAdjustBounds(bool v);
        
    private:
        LinearLayoutOrientation orientation_;
        bool adjustBounds_;
    };
    
}}
