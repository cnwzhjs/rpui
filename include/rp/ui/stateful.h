//
//  stateful.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/3/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#pragma once

#include <string>

namespace rp { namespace ui {
    
    template<class T, class TState = std::string>
    class Stateful {
    public:
        typedef T base_t;
        typedef TState state_t;
        
    protected:
        Stateful(state_t defaultState)
            : state_(defaultState)
        {}
        
        ~Stateful()
        {}
        
        state_t getState() const {
            return state_;
        }
        
        void setState(const state_t& newState) {
            if (state_ == newState)
                return;
            
            onStateChanging(state_, newState);
            state_ = newState;
        }
        
        void onStateChanging(const state_t& lastState, const state_t& newState) {
            ((T*)this)->onStateChanging(lastState, newState);
        }
        
    private:
        state_t state_;
    };
    
}}
