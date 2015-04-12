//
//  edit_text.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/4/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#include <rp/ui/edit_text.h>
#include <rp/ui/style_manager.h>

namespace rp { namespace ui {
    
    EditText::EditText()
    : DisplayObjectContainer()
    , Stateful<rp::ui::EditText, rp::ui::EditTextStatus>(EditTextStatusNormal)
    , background_(new Pane())
    , label_(new Label())
    , cursor_(new Pane())
    , text_()
    , selectBegin_(0)
    , selectLength_(0)
    {
        auto style = StyleManager::defaultManager();
        
        std::string defaultFont = style->getString("Font.DefaultFont", "Verdana");
        
        normalBgColor_ = style->getColor("EditText.NormalBackgroundColor", colors::gray);
        activeBgColor_ = style->getColor("EditText.ActiveBackgroundColor", colors::silver);
        disabledBgColor_ = style->getColor("EditText.DisabledBackgroundColor", colors::white);
        
        normalTextColor_ = style->getColor("EditText.NormalBackgroundColor", colors::gray);
        activeTextColor_ = style->getColor("EditText.ActiveBackgroundColor", colors::silver);
        disabledTextColor_ = style->getColor("EditText.DisabledBackgroundColor", colors::white);
        
        fontFace_ = style->getString("EditText.FontFace", defaultFont);
        fontSize_ = style->getFloat("EditText.FontSize", 9);
        
        setTouchChildren(false);
        setTouchEnabled(true);
        
        
    }

    EditText::~EditText() {}
    
}}
