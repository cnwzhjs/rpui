//
//  signal.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/14/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <mutex>
#include <functional>
#include <list>
#include <rp/util/noncopyable.h>

namespace rp { namespace ui {

    template<typename... ParametersT>
    class Signal : public rp::util::noncopyable {
    public:
        typedef std::function<void(ParametersT...)> handler_t;
        
        Signal() {}
        ~Signal() {}
        
        void trigger(ParametersT... parameters) {
            std::list<handler_t> onceHandlers;
            std::list<handler_t> handlers;
            
            {
                std::lock_guard<std::mutex> guard(lock_);
                onceHandlers = onceHandlers_;
                onceHandlers_.clear();
                handlers = handlers_;
            }
            
            for (auto& onceHandler : onceHandlers) {
                onceHandler(parameters...);
            }
            
            for (auto& handler : handlers) {
                handler(parameters...);
            }
        }
        
        void addHandler(handler_t handler) {
            std::lock_guard<std::mutex> guard(lock_);
            handlers_.push_back(handler);
        }
        
        void addOnce(handler_t handler) {
            std::lock_guard<std::mutex> guard(lock_);
            onceHandlers_.push_back(handler);
        }
        
    private:
        std::mutex lock_;
        std::list<handler_t> onceHandlers_;
        std::list<handler_t> handlers_;
    };
    
}}
