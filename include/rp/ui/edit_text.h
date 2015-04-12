//
//  edit_text.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/4/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//


#pragma once

#include <rp/ui/display_object_container.h>
#include <rp/ui/stateful.h>
#include <rp/ui/pane.h>
#include <rp/ui/label.h>
#include <rp/ui/color.h>
#include <string>
#include <memory>

namespace rp { namespace ui {
    
    enum EditTextStatus {
        EditTextStatusNormal,
        EditTextStatusActive,
        EditTextStatusDisabled
    };
    
    class EditText : DisplayObjectContainer, public Stateful<EditText, EditTextStatus> {
    public:
        EditText();
        ~EditText();

#if 0 
        std::string getText() const;
        void setText(const std::string& text) const;
        
        int getSelectBegin() const;
        int getSelectLength() const;
        
        void select(int begin, int length);
        
        void update();
        
        void invokeOnStateChanging(const EditTextStatus& oldStatus, const EditTextStatus& newStatus);
#endif
    private:
        std::shared_ptr<Pane> background_;
        std::shared_ptr<Label> label_;
        std::shared_ptr<Pane> cursor_;
        
        std::string text_;
        int selectBegin_;
        int selectLength_;
        
        Color normalBgColor_;
        Color activeBgColor_;
        Color disabledBgColor_;
        
        Color normalTextColor_;
        Color activeTextColor_;
        Color disabledTextColor_;
        
        std::string fontFace_;
        float fontSize_;
    };
    
}}
