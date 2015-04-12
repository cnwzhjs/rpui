//
//  scroll_view.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/4/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#include <rp/ui/scroll_view.h>
#include <math.h>
#include <stdio.h>

using namespace std;

namespace rp { namespace ui {
    
    ScrollView::ScrollView()
        : content_(nullptr)
        , horizontalScrollIndicator_(new Pane())
        , verticalScrollIndicator_(new Pane())
        , scrollX_(0)
        , scrollY_(0)
        , minScrollX_(0)
        , maxScrollX_(0)
        , minScrollY_(0)
        , maxScrollY_(0)
        , draging_(false)
        , scrollDirection_(ScrollDirectionBoth)
    {
        setTouchChildren(false);
        setTouchEnabled(true);
    }
    
    ScrollView::~ScrollView()
    {}
    
    ScrollDirection ScrollView::getScrollDirection() const {
        return scrollDirection_;
    }
    
    void ScrollView::setScrollDirection(rp::ui::ScrollDirection direction) {
        scrollDirection_ = direction;
    }
    
    void ScrollView::invokeContentSizeChanged() {
        calcScrollParameters(getWidth(), content_ ? content_->getWidth() : 0,
                             scrollDirection_ & ScrollDirectionHorizontal,
                             &scrollX_, &minScrollX_, &maxScrollX_);
        calcScrollParameters(getHeight(), content_ ? content_->getHeight() : 0,
                             scrollDirection_ & ScrollDirectionVertical,
                             &scrollY_, &minScrollY_, &maxScrollY_);
        
        fprintf(stderr, "scrollX: %d in (%d, %d), scrollY: %d in (%d, %d)\n",
                scrollX_, minScrollX_, maxScrollX_,
                scrollY_, minScrollY_, maxScrollY_
                );
        
        if (content_) {
            content_->setPosition(Point(scrollX_, scrollY_));
        }
    }
    
    shared_ptr<DisplayObject> ScrollView::getContent() {
        return content_;
    }
    
    void ScrollView::setContent(std::shared_ptr<DisplayObject> content) {
        if (content_ == content)
            return;
        
        if (content_)
            removeChild(content_);
        
        content_ = content;
        appendChild(content);
    }
    
    static int my_abs_(int v) {
        return v >= 0 ? v : -v;
    }

    void ScrollView::invokeOnTouch(const rp::ui::TouchEvent &ev) {
        DisplayObjectContainer::invokeOnTouch(ev);

        auto position = ev.position + getPosition();
        
        if (ev.type == TouchEventTypeBegan) {
            totalMoveDistance_ = 0;
            auto touchChildren = getTouchChildren();
            auto touchEnabled = getTouchEnabled();
            
            setTouchChildren(true);
            setTouchEnabled(false);
            touchManager_.dispatch(shared_from_this(), TouchEventTypeBegan, position);
            setTouchChildren(touchChildren);
            setTouchEnabled(touchEnabled);
        } else if (ev.type == TouchEventTypeMove) {
            setDraging_(true);
            auto offset = ev.position - ev.lastPosition;
            
            if (scrollDirection_ & ScrollDirectionHorizontal) {
                totalMoveDistance_ += my_abs_(offset.x);
            }
            
            if (scrollDirection_ & ScrollDirectionVertical) {
                totalMoveDistance_ += my_abs_(offset.y);
            }
            
            offsetContent_(offset.x, offset.y);
            
            if (totalMoveDistance_ >= 8) {
                touchManager_.dispatch(shared_from_this(), TouchEventTypeCancel, position);
            }
        } else if (ev.type == TouchEventTypeEnd) {
            touchManager_.dispatch(shared_from_this(), TouchEventTypeEnd, position);
            setDraging_(false);
        }
    }
    
    void ScrollView::update() {
        DisplayObjectContainer::update();
        
        if (draging_)
            return;
        
        if (content_) {
            content_->setX(composeScrollPosition_(scrollX_, content_->getX()));
            content_->setY(composeScrollPosition_(scrollY_, content_->getY()));
        }
    }

    void ScrollView::normalizeScrollPosition_() {
        scrollX_ = normalizeScrollPosition_(scrollX_, minScrollX_, maxScrollX_);
        scrollY_ = normalizeScrollPosition_(scrollY_, minScrollY_, maxScrollY_);
    }
    
    int ScrollView::normalizeScrollPosition_(int v, int minValue, int maxValue) {
        if (v < minValue)
            return minValue;
        
        if (v > maxValue)
            return maxValue;
        
        return v;
    }
    
    int ScrollView::composeScrollPosition_(int targetValue, int currentValue) {
        if (my_abs_(targetValue - currentValue) < 2)
            return targetValue;
        
        return (targetValue + currentValue)/2;
    }
    
    void ScrollView::offsetContent_(int offsetX, int offsetY) {
        if (scrollDirection_ & ScrollDirectionHorizontal) {
            scrollX_ += offsetX;
            if (content_) {
                content_->setX(content_->getX() + offsetX);
            }
        }
        
        if (scrollDirection_ & ScrollDirectionVertical) {
            scrollY_ += offsetY;
            if (content_) {
                content_->setY(content_->getY() + offsetY);
            }
        }
        
        normalizeScrollPosition_();
    }
    
    void ScrollView::calcScrollParameters(int containerSize, int contentSize, bool scrollable, int *scroll, int *minScroll, int *maxScroll) {
        if (contentSize <= containerSize || !scrollable) {
            *scroll = *minScroll = *maxScroll = 0;
        } else {
            *scroll = 0;
            *maxScroll = 0;
            *minScroll = containerSize - contentSize;
        }
    }
    
    void ScrollView::setDraging_(bool v) {
        if (draging_ == v)
            return;
        
        draging_ = v;
        
        if (v) {
            if (scrollDirection_ & ScrollDirectionHorizontal) {
                removeChild(horizontalScrollIndicator_);
                appendChild(horizontalScrollIndicator_);
                horizontalScrollIndicator_->setVisible(true);
            }
            
            if (scrollDirection_ & ScrollDirectionVertical) {
                removeChild(verticalScrollIndicator_);
                appendChild(verticalScrollIndicator_);
                verticalScrollIndicator_->setVisible(true);
            }
        } else {
            horizontalScrollIndicator_->setVisible(false);
            verticalScrollIndicator_->setVisible(false);
        }
    }
    
}}
