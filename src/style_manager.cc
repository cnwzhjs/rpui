//
//  style_manager.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/4/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#include <rp/ui/style_manager.h>
#include <mutex>

using namespace std;

namespace rp { namespace ui {

    StyleManager::StyleManager() {}
    
    StyleManager::~StyleManager() {}
    
    int StyleManager::getInteger(const std::string& key, int defaultValue) {
        auto i = integers_.find(key);
        return i == integers_.end() ? defaultValue : i->second;
    }
    
    void StyleManager::setInteger(const std::string& key, int value) {
        integers_[key] = value;
    }
    
    float StyleManager::getFloat(const std::string& key, float defaultValue) {
        auto i = floats_.find(key);
        return i == floats_.end() ? defaultValue : i->second;
    }
    
    void StyleManager::setFloat(const std::string& key, float value) {
        floats_[key] = value;
    }
    
    Color StyleManager::getColor(const std::string& key, Color defaultValue) {
        auto i = colors_.find(key);
        return i == colors_.end() ? defaultValue : i->second;
    }
    
    void StyleManager::setColor(const std::string& key, Color value) {
        colors_[key] = value;
    }
    
    std::string StyleManager::getString(const std::string& key, std::string defaultValue) {
        auto i = strings_.find(key);
        return i == strings_.end() ? defaultValue : i->second;
    }
    
    void StyleManager::setString(const std::string& key, std::string value) {
        strings_[key] = value;
    }
    
    static once_flag once_;
    static shared_ptr<StyleManager> defaultManager_;
    
    void initStyleManager_() {
        defaultManager_ = shared_ptr<StyleManager>(new StyleManager());
    }
    
    shared_ptr<StyleManager> StyleManager::defaultManager() {
        call_once(once_, &initStyleManager_);
        return defaultManager_;
    }
    
}}
