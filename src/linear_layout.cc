//
//  linear_layout.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/4/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#include <rp/ui/linear_layout.h>

namespace rp { namespace ui {
    
    LinearLayout::LinearLayout()
    : orientation_(LinearLayoutOrientationVertical)
    , adjustBounds_(false)
    {}
    
    LinearLayout::~LinearLayout()
    {}

    void LinearLayout::relayout() {
        if (orientation_ == LinearLayoutOrientationHorizontal) {
            int currentX = 0;
            
            for (auto child : getChildren()) {
                child->setFrame(Rectangle(currentX, 0, child->getWidth(), getHeight()));
                currentX += child->getWidth();
            }
            
            if (adjustBounds_) {
                setWidth(currentX);
            }
        } else {
            int currentY = 0;
            
            for (auto child : getChildren()) {
                child->setFrame(Rectangle(0, currentY, getWidth(), child->getHeight()));
                currentY += child->getHeight();
            }
            
            if (adjustBounds_) {
                setHeight(currentY);
            }
        }
    }
    
    LinearLayoutOrientation LinearLayout::getOrientation() const {
        return orientation_;
    }
    
    void LinearLayout::setOrientation(rp::ui::LinearLayoutOrientation orientation) {
        orientation_ = orientation;
    }
    
    bool LinearLayout::getAdjustBounds() const {
        return adjustBounds_;
    }
    
    void LinearLayout::setAdjustBounds(bool v) {
        adjustBounds_ = v;
    }
    
}}
