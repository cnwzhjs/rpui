//
//  style_manager.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/4/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/ui/color.h>
#include <rp/util/noncopyable.h>
#include <memory>
#include <map>
#include <string>

namespace rp { namespace ui {

    class StyleManager : private rp::util::noncopyable {
    public:
        ~StyleManager();
        
    public:
        int getInteger(const std::string& key, int defaultValue=0);
        void setInteger(const std::string& key, int value);

        float getFloat(const std::string& key, float defaultValue=0);
        void setFloat(const std::string& key, float value);

        Color getColor(const std::string& key, Color defaultValue=colors::black);
        void setColor(const std::string& key, Color value);
        
        std::string getString(const std::string& key, std::string defaultValue="");
        void setString(const std::string& key, std::string value);
        
        static std::shared_ptr<StyleManager> defaultManager();
        
    private:
        friend void initStyleManager_();
        
        StyleManager();
        
        std::map<std::string, int> integers_;
        std::map<std::string, float> floats_;
        std::map<std::string, Color> colors_;
        std::map<std::string, std::string> strings_;
    };
    
}}
