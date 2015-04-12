//
//  ui_manager.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/23/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <mutex>
#include <atomic>
#include <algorithm>
#include <rp/ui/ui_manager.h>
#include <rp/ui/scene.h>
#include <rp/ui/device_context.h>
#include <rp/ui/renderer.h>
#include <rp/ui/image_loader_manager.h>
#include <rp/ui/libpng_image_loader.h>
#include <rp/ui/dirty_frame_detector.h>
#include <rp/ui/image.h>
#include <rp/ui/display_object.h>
#include <rp/drivers/display/rpusbdisp/rpusbdisp.h>
#include <rp/deps/libusbx_wrap/libusbx_wrap.h>
#include <rp/ui/touch_manager.h>
#include <iostream>

using namespace std;
using namespace rp::util;
using namespace rp::drivers::display;

namespace rp { namespace ui {
    
    class UiManagerImpl : public std::enable_shared_from_this<UiManagerImpl>, private rp::util::noncopyable {
    public:
        UiManagerImpl()
            : running_(false)
            , dirtyFrameDetector_(new DirtyFrameDetector())
            , lastActive_(chrono::high_resolution_clock::now())
            , currentBacklightLevel_(1023)
            , targetBacklightLevel_(1023)
        {
            registerImageLoaders_();
        }
        
        ~UiManagerImpl() {
            stop();
            if (recoverThread_.joinable())
                recoverThread_.join();
        }
        
        shared_ptr<Scene> getScene() {
            lock_guard<mutex> guard(lock_);
            return scene_;
        }
        
        void setScene(shared_ptr<Scene> scene) {
            lock_guard<mutex> guard(lock_);
            scene_ = scene;
        }
        
        void setOnUpdate(function<void()> callback) {
            onUpdateCallback_ = callback;
        }
        
        void start() {
            if (running_) {
                return;
            }
            
            running_ = true;
            while (running_) {
                tick_();
                this_thread::sleep_for(chrono::milliseconds(1000/60));
            }
        }
        
        void stop() {
            if (!running_) {
                return;
            }
            
            running_ = false;
        }
        
        int run() {
            try {
                waitForDisplay_();
                initializeDisplay_();
                start();
                return 0;
            } catch (const Exception& e) {
                e.printToConsole();
                return 1;
            }
        }
        
    private:
        void registerImageLoaders_() {
            ImageLoaderManager::defaultManager()->registerLoader(shared_ptr<IImageLoader>(new LibpngImageLoader()));
        }
        
        void waitForDisplay_() {
            fprintf(stderr, "Waiting for display to connect...\n");
            
            while (true) {
                auto display = RoboPeakUsbDisplayDevice::openFirstDevice();
                
                if (display) {
                    fprintf(stderr, "Display #%s found, %dx%d\n", display->getDevice()->getSerialNumber().c_str(), display->getWidth(), display->getHeight());
                    
                    display_ = display;
                    display->enable();
                    
                    while (display->getStatus().display_status & RPUSBDISP_DISPLAY_STATUS_DIRTY_FLAG) {
                        display->fill(colors::black.toBGR565());
                        this_thread::sleep_for(chrono::milliseconds(1000));
                    }
                    
                    return;
                }
                
                this_thread::sleep_for(chrono::microseconds(100));
            }
        }
        
        void initializeDisplay_() {
            fprintf(stderr, "Initializing display...\n");
            display_->setStatusUpdatedCallback(bind(&UiManagerImpl::onDisplayStatusUpdated, shared_from_this(), placeholders::_1));
            
            targetDc_ = shared_ptr<DeviceContext>(new DeviceContext(display_));
            
            if (!backBufferImage_) {
                fprintf(stderr, "Initializing back buffer...\n");
                backBufferImage_ = shared_ptr<Image>(new Image(display_->getWidth(), display_->getHeight(), PixelFormatBGR565));
                backBufferDeviceContext_ = shared_ptr<DeviceContext>(new DeviceContext(backBufferImage_));
                
                renderer_ = shared_ptr<Renderer>(new Renderer(backBufferDeviceContext_));
            }

            if (display_ && display_->isAlive()) {
                //display_->setBacklightLevel(currentBacklightLevel_);
            }
        }
        
        void tick_() {
            processTouchEvents_();
            
            if (onUpdateCallback_) {
                onUpdateCallback_();
            }
            
            if (!scene_)
                return;
            
            scene_->update();
            
            if (!backBufferImage_)
                return;
            
            scene_->apply(dirtyFrameDetector_);
            Rectangle dirtyFrame = dirtyFrameDetector_->getDirtyFrame();
            
            scene_->apply(renderer_);

            auto now = chrono::high_resolution_clock::now();
            auto duration_since_last_active = now - lastActive_;

            if (duration_since_last_active > chrono::seconds(5)) {
                targetBacklightLevel_ = 64;
            }

            bool shouldUpdateBacklight = false;
            if (currentBacklightLevel_ != targetBacklightLevel_) {
                int diff = targetBacklightLevel_ - currentBacklightLevel_;
                if (diff > 16)
                    diff = 16;
                else if (diff < -16)
                    diff = -16;

                currentBacklightLevel_ += diff;
                shouldUpdateBacklight = true;
            }
            
            if (display_ && display_->isAlive() && targetDc_) {
                try {
                    targetDc_->bitblt(backBufferImage_, dirtyFrame, dirtyFrame.position, BitOperationCopy);
                    if (shouldUpdateBacklight) {
                        //display_->setBacklightLevel(currentBacklightLevel_);
                    }
                } catch (const Exception& e) {
                    resetAndRecoverDisplay_();
                }
            } else if (display_ && !display_->isAlive()) {
                resetAndRecoverDisplay_();
            }
        }
        
        void processTouchEvents_() {
            list<rpusbdisp_status_normal_packet_t> touchEventQueue;
            
            {
                lock_guard<mutex> guard(touchEventQueueLock_);
                swap(touchEventQueue, touchEventQueue_);
            }
            
            for (auto& packet : touchEventQueue) {
                processTouchEvent_(packet);
            }

            if (!touchEventQueue.empty()) {
                lastActive_ = chrono::high_resolution_clock::now();
                targetBacklightLevel_ = 1023;
            }
        }
        
        void processTouchEvent_(const rpusbdisp_status_normal_packet_t& packet) {
            Point screenPosition(packet.touch_x, packet.touch_y);
            touchManager_.dispatch(dynamic_pointer_cast<DisplayObject>(scene_), !!packet.touch_status, screenPosition);
        }
        
        void resetAndRecoverDisplay_() {
            targetDc_ = nullptr;
            display_ = nullptr;
            
            recoverDisplay_();
        }
        
        void recoverDisplay_() {
            fprintf(stderr, "recovering display\n");
            if (recoverThread_.joinable()) {
                fprintf(stderr, "recover thread exist...\n");
                recoverThread_.join();
            }
            
            recoverThread_ = thread(bind(&UiManagerImpl::doRecoverDisplay_, this));
        }
        
        void doRecoverDisplay_() {
            while (!display_) {
                try {
                    waitForDisplay_();
                    initializeDisplay_();
                    targetDc_->bitblt(backBufferImage_, Rectangle(0, 0, backBufferImage_->getWidth(), backBufferImage_->getHeight()), Point(0, 0), BitOperationCopy);
                    targetDc_->bitblt(backBufferImage_, Rectangle(0, 0, backBufferImage_->getWidth(), backBufferImage_->getHeight()), Point(0, 0), BitOperationCopy);
                    //display_->setBacklightLevel(currentBacklightLevel_);
                } catch(const Exception& e) {
                    e.printToConsole();
                }
            }
        }
        
        void onDisplayStatusUpdated(const rpusbdisp_status_normal_packet_t& status) {
            lock_guard<mutex> guard(touchEventQueueLock_);
            touchEventQueue_.push_back(status);
        }
        
        mutex lock_;
        atomic<bool> running_;
        shared_ptr<Scene> scene_;
        thread recoverThread_;
        
        shared_ptr<RoboPeakUsbDisplayDevice> display_;
        shared_ptr<DeviceContext> targetDc_;
        
        TouchManager touchManager_;
        shared_ptr<Image> backBufferImage_;
        shared_ptr<DeviceContext> backBufferDeviceContext_;
        
        shared_ptr<Renderer> renderer_;
        
        shared_ptr<DirtyFrameDetector> dirtyFrameDetector_;
        
        list<rpusbdisp_status_normal_packet_t> touchEventQueue_;
        mutex touchEventQueueLock_;
        
        std::function<void()> onUpdateCallback_;

        chrono::high_resolution_clock::time_point lastActive_;
        int currentBacklightLevel_;
        int targetBacklightLevel_;
    };
    
    UiManager::UiManager()
    : impl_(new UiManagerImpl())
    {}
    
    UiManager::~UiManager()
    {
        impl_->stop();
    }
    
    shared_ptr<Scene> UiManager::getScene() {
        return impl_->getScene();
    }
    
    void UiManager::setScene(std::shared_ptr<Scene> scene) {
        impl_->setScene(scene);
    }
    
    void UiManager::setOnUpdate(std::function<void ()> callback) {
        impl_->setOnUpdate(callback);
    }
    
    void UiManager::start() {
        impl_->start();
    }
    
    void UiManager::stop() {
        impl_->stop();
    }
    
    int UiManager::run() {
        return impl_->run();
    }
    
    static once_flag once_;
    static shared_ptr<UiManager> sharedUiManager_;
    
    void initSharedUiManager_() {
        sharedUiManager_ = shared_ptr<UiManager>(new UiManager());
    }
    
    shared_ptr<UiManager> UiManager::defaultManager() {
        call_once(once_, &initSharedUiManager_);
        return sharedUiManager_;
    }
    
    
    
}}

