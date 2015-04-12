//
//  geometry.h
//  Geometry
//
//  Created by Tony Huang on 12/14/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

namespace rp { namespace ui {
    
    class Size;
    
    class Point {
    public:
        Point();
        Point(int x, int y);
        Point(const Point& that);
        Point(const Size& that);
        ~Point();
        
        Point& operator=(const Point& that);
        
        int x;
        int y;
    };
    
    class Size {
    public:
        Size();
        Size(int width, int height);
        Size(const Size& that);
        Size(const Point& that);
        ~Size();
        
        Size& operator=(const Size& that);
        
        int width;
        int height;
    };
    
    class Rectangle {
    public:
        Rectangle();
        Rectangle(int x, int y, int width, int height);
        Rectangle(const Point& position, const Size& size);
        Rectangle(const Rectangle& that);
        ~Rectangle();
        
        Rectangle& operator=(const Rectangle& that);
        
        int getRight() const;
        int getBottom() const;
        
        bool isEmpty() const;
        
        Rectangle offset(Size size);
        
        Point position;
        Size size;
    };
    
    bool operator==(const Point& v1, const Point& v2);
    bool operator!=(const Point& v1, const Point& v2);
    
    Point operator+(const Point& v, const Point& o);
    Point operator-(const Point& v1, const Point& v2);

    bool operator==(const Size& v1, const Size& v2);
    bool operator!=(const Size& v1, const Size& v2);

    Size operator+(const Size& v, const Size& o);
    Size operator-(const Size& v1, const Size& v2);
    
    Point operator-(const Point& v);
    Size operator-(const Size& v);
    
    bool operator==(const Rectangle& v1, const Rectangle& v2);
    bool operator!=(const Rectangle& v1, const Rectangle& v2);

    Rectangle intersects(const Rectangle& a, const Rectangle& b);
    Rectangle unions(const Rectangle& a, const Rectangle& b);
    
    bool isIntersect(const Point& p, const Rectangle& rect);
    
}}
