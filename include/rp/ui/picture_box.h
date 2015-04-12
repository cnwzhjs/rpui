//
//  picture_box.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/22/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/ui/display_object.h>

namespace rp { namespace ui {
    
    class Image;
    
    class PictureBox : public DisplayObject {
    public:
        PictureBox();
        virtual ~PictureBox();
        
        std::shared_ptr<Image> getImage();
        void setImage(std::shared_ptr<Image> image);
        
        virtual void apply(std::shared_ptr<IDisplayObjectVisitor> visitor);
        
    private:
        std::shared_ptr<Image> image_;
    };
    
}}
