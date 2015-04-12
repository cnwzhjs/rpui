//
//  image_loader_manager.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/21/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <vector>
#include <mutex>
#include <rp/ui/image_loader_manager.h>
#include <rp/ui/image_loader.h>
#include <rp/ui/image.h>
#include <rp/util/buffer.h>
#include <rp/util/exception.h>

using namespace std;
using namespace rp::util;

namespace rp { namespace ui {

    class ImageLoaderManagerImpl : public noncopyable {
    public:
        ImageLoaderManagerImpl() {}
        ~ImageLoaderManagerImpl() {
            lock_guard<mutex> guard(lock_);
        }
        
        void registerLoader(shared_ptr<IImageLoader> loader) {
            lock_guard<mutex> guard(lock_);
            
            loaders_.push_back(loader);
        }
        
        shared_ptr<Image> loadImage(shared_ptr<Buffer> buffer) {
            lock_guard<mutex> guard(lock_);
            
            for (auto loader : loaders_) {
                shared_ptr<Image> image = loader->loadImage(buffer);
                if (image) return image;
            }
            
            return nullptr;
        }
        
    private:
        mutex lock_;
        vector<shared_ptr<IImageLoader>> loaders_;
    };
    
    ImageLoaderManager::ImageLoaderManager() : impl_(new ImageLoaderManagerImpl()) {}
    ImageLoaderManager::~ImageLoaderManager() {}
    
    void ImageLoaderManager::registerLoader(std::shared_ptr<IImageLoader> loader) {
        impl_->registerLoader(loader);
    }
    
    shared_ptr<Image> ImageLoaderManager::loadImage(std::shared_ptr<rp::util::Buffer> buffer) {
        return impl_->loadImage(buffer);
    }
    
    static shared_ptr<ImageLoaderManager> defaultManager_;
    static once_flag onceFlag_;
    
    shared_ptr<ImageLoaderManager> ImageLoaderManager::defaultManager() {
        call_once(onceFlag_, []() { defaultManager_ = shared_ptr<ImageLoaderManager>(new ImageLoaderManager()); });
        return defaultManager_;
    }
    
}}

