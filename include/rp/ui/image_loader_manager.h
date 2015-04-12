//
//  image_loader_manager.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/21/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <memory>
#include <rp/util/noncopyable.h>

namespace rp { namespace util {
    
    class Buffer;
    
}}


namespace rp { namespace ui {
    
    class Image;
    class IImageLoader;
    class ImageLoaderManagerImpl;
    
    class ImageLoaderManager : public rp::util::noncopyable {
    public:
        ~ImageLoaderManager();
        
        void registerLoader(std::shared_ptr<IImageLoader> loader);
        std::shared_ptr<Image> loadImage(std::shared_ptr<rp::util::Buffer> buffer);
        
        static std::shared_ptr<ImageLoaderManager> defaultManager();
        
    private:
        ImageLoaderManager();
        
        std::unique_ptr<ImageLoaderManagerImpl> impl_;
    };
    
}}
