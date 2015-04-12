//
//  geometry.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/16/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <rp/ui/geometry.h>

namespace rp { namespace ui {
    
    Point::Point() : x(0), y(0) {}
    Point::Point(int x, int y) : x(x), y(y) {}
    Point::Point(const Point& that) : x(that.x), y(that.y) {}
    Point::Point(const Size& that) : x(that.width), y(that.height) {}
    Point::~Point() {}
    
    Point& Point::operator=(const Point& that) {
        this->x = that.x;
        this->y = that.y;
        return *this;
    }
    
    Size::Size() : width(0), height(0) {}
    Size::Size(int width, int height) : width(width), height(height) {}
    Size::Size(const Size& that) : width(that.width), height(that.height) {}
    Size::Size(const Point& that) : width(that.x), height(that.y) {}
    Size::~Size() {}
    
    Size& Size::operator=(const Size& that) {
        width = that.width;
        height = that.height;
        return *this;
    }
    
    Rectangle::Rectangle() {}
    Rectangle::Rectangle(int x, int y, int width, int height) : position(x, y), size(width, height) {}
    Rectangle::Rectangle(const Point& position, const Size& size) : position(position), size(size) {}
    Rectangle::Rectangle(const Rectangle& rect) : position(rect.position), size(rect.size) {}
    Rectangle::~Rectangle() {}
    
    Rectangle& Rectangle::operator=(const rp::ui::Rectangle &that) {
        position = that.position;
        size = that.size;
        return *this;
    }
    
    int Rectangle::getRight() const {
        return position.x + size.width - 1;
    }
    
    int Rectangle::getBottom() const {
        return position.y + size.height - 1;
    }
    
    bool Rectangle::isEmpty() const {
        return size.width <= 0 || size.height <= 0;
    }
    
    Rectangle Rectangle::offset(Size size) {
        return Rectangle(this->position+Point(size), this->size);
    }
    
    bool operator==(const Point& v1, const Point& v2) {
        return (v1.x == v2.x) && (v1.y == v2.y);
    }
    
    bool operator!=(const Point& v1, const Point& v2) {
        return (v1.x != v2.x) || (v1.y != v2.y);
    }
    
    Point operator+(const Point& v1, const Point& v2) {
        return Point(v1.x + v2.x, v1.y + v2.y);
    }
    
    Point operator-(const Point& v1, const Point& v2) {
        return Point(v1.x - v2.x, v1.y - v2.y);
    }
    
    bool operator==(const Size& v1, const Size& v2) {
        return (v1.width == v2.width) && (v1.height == v2.height);
    }
    
    bool operator!=(const Size& v1, const Size& v2) {
        return (v1.width != v2.width) || (v1.height != v2.height);
    }
    
    Size operator+(const Size& v1, const Size& v2) {
        return Point(v1.width + v2.width, v1.height + v2.height);
    }
    
    Size operator-(const Size& v1, const Size& v2) {
        return Point(v1.width - v2.width, v1.height - v2.height);
    }
    
    Point operator-(const Point& v) {
        return Point(-v.x, -v.y);
    }
    
    Size operator-(const Size& v) {
        return Size(-v.width, -v.height);
    }
    
    bool operator==(const Rectangle& v1, const Rectangle& v2) {
        return (v1.position == v2.position) && (v1.size == v2.size);
    }
    
    bool operator!=(const Rectangle& v1, const Rectangle& v2) {
        return (v1.position != v2.position) || (v1.size != v2.size);
    }
    
    Rectangle intersects(const Rectangle& a, const Rectangle& b) {
        if (a.isEmpty()) return a;
        if (b.isEmpty()) return b;
        
        int l = a.position.x;
        int t = a.position.y;
        int r = a.getRight();
        int bottom = a.getBottom();
        
        if (b.position.x > l)
            l = b.position.x;
        
        if (b.position.y > t)
            t = b.position.y;
        
        if (b.getRight() < r)
            r = b.getRight();
        
        if (b.getBottom() < bottom)
            bottom = b.getBottom();
        
        int w = r-l+1;
        int h = bottom-t+1;
        
        if (w < 0) w = 0;
        if (h < 0) h = 0;
        
        return Rectangle(l, t, w, h);
    }
    
    Rectangle unions(const Rectangle& a, const Rectangle& b) {
        if (a.isEmpty()) return b;
        if (b.isEmpty()) return a;
        
        int l = a.position.x;
        int t = a.position.y;
        int r = a.getRight();
        int bottom = a.getBottom();
        
        if (b.position.x < l)
            l = b.position.x;
        
        if (b.position.y < t)
            t = b.position.y;
        
        if (b.getRight() > r)
            r = b.getRight();
        
        if (b.getBottom() > bottom)
            bottom = b.getBottom();
        
        int w = r-l+1;
        int h = bottom-t+1;
        
        return Rectangle(l, t, w, h);
    }
    
    bool isIntersect(const Point& p, const Rectangle& rect) {
        if (p.x < rect.position.x) return false;
        if (p.x > rect.getRight()) return false;
        if (p.y < rect.position.y) return false;
        if (p.y > rect.getBottom()) return false;
        return true;
    }
    
}}