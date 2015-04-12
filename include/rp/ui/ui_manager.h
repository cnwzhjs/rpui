//
//  ui_manager.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/22/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <memory>
#include <rp/util/noncopyable.h>

namespace rp { namespace ui {
    
    class UiManagerImpl;
    class Scene;
    
    class UiManager : public rp::util::noncopyable {
    public:
        ~UiManager();
        
        std::shared_ptr<Scene> getScene();
        void setScene(std::shared_ptr<Scene> scene);
        
        void setOnUpdate(std::function<void()> callback);
        
        void start();
        void stop();
        
        int run();
        
        static std::shared_ptr<UiManager> defaultManager();
        
    private:
        friend void initSharedUiManager_();
        
        UiManager();
        std::shared_ptr<UiManagerImpl> impl_;
    };
    
}}
