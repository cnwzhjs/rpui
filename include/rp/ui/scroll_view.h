//
//  scroll_view.h
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/4/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/ui/display_object_container.h>
#include <rp/ui/pane.h>
#include <rp/ui/touch_manager.h>

namespace rp { namespace ui {

    enum ScrollDirection {
        ScrollDirectionNone = 0,
        ScrollDirectionHorizontal = 1,
        ScrollDirectionVertical = 2,
        ScrollDirectionBoth = 3
    };
    
    class ScrollView : public DisplayObjectContainer {
    public:
        ScrollView();
        ~ScrollView();
        
        ScrollDirection getScrollDirection() const;
        void setScrollDirection(ScrollDirection direction);
        
        void invokeContentSizeChanged();
        
        std::shared_ptr<DisplayObject> getContent();
        void setContent(std::shared_ptr<DisplayObject> content);
        
        void invokeOnTouch(const TouchEvent& ev);
        
        void update();
        
    private:
        void normalizeScrollPosition_();
        int normalizeScrollPosition_(int v, int minValue, int maxValue);
        int composeScrollPosition_(int targetValue, int currentValue);
        void offsetContent_(int offsetX, int offsetY);
        
        void calcScrollParameters(int containerSize, int contentSize, bool scrollable, int* scroll, int* minScroll, int* maxScroll);
        
        void setDraging_(bool v);
        
        std::shared_ptr<DisplayObject> content_;
        std::shared_ptr<Pane> horizontalScrollIndicator_, verticalScrollIndicator_;
        
        int scrollX_;
        int scrollY_;
        
        int minScrollX_, maxScrollX_;
        int minScrollY_, maxScrollY_;
        
        int totalMoveDistance_;
        
        bool draging_;
        
        ScrollDirection scrollDirection_;
        
        TouchManager touchManager_;
    };
    
}}
