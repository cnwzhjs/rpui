//
//  pane.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/16/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <rp/ui/pane.h>

using namespace std;

namespace rp { namespace ui {

    Pane::Pane() {}
    Pane::~Pane() {}
    
    Color Pane::getBackgroundColor() const {
        return backgroundColor_;
    }
    
    void Pane::setBackgroundColor(Color c) {
        if (c != backgroundColor_) {
            backgroundColor_ = c;
            markDirty(getFrame());
        }
    }
    
    void Pane::apply(shared_ptr<IDisplayObjectVisitor> visitor) {
        visitor->visit(dynamic_pointer_cast<Pane>(shared_from_this()));
    }
    
}}

