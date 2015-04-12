//
//  picture_box.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/22/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <rp/ui/picture_box.h>
#include <rp/ui/image.h>

using namespace std;
using namespace rp::util;

namespace rp { namespace ui {
    
    PictureBox::PictureBox() {}
    PictureBox::~PictureBox() {}
    
    shared_ptr<Image> PictureBox::getImage() {
        return image_;
    }
    
    void PictureBox::setImage(shared_ptr<Image> image) {
        if (image_ == image)
            return;
        
        image_ = image;
        markDirty(getFrame());
    }
    
    void PictureBox::apply(std::shared_ptr<IDisplayObjectVisitor> visitor) {
        visitor->visit(dynamic_pointer_cast<PictureBox>(shared_from_this()));
    }
    
}}

