//
//  libpng_image_loader.h
//  Image loader based on libpng
//
//  Created by Tony Huang on 12/21/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/ui/image_loader.h>

namespace rp { namespace ui {
    
    class LibpngImageLoader : public IImageLoader {
    public:
        LibpngImageLoader();
        virtual ~LibpngImageLoader();
        
        virtual bool canLoad(std::shared_ptr<rp::util::Buffer> buffer);
        virtual ImageMetadata loadMetadata(std::shared_ptr<rp::util::Buffer> buffer);
        virtual std::shared_ptr<Image> loadImage(std::shared_ptr<rp::util::Buffer> buffer);
    };
    
}}
