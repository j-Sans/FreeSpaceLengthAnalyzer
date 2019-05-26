//
//  Point.hpp
//  SquareHeuristic
//
//  Created by Jake Sanders on 6/29/17.
//  Copyright © 2017 Jake Sanders. All rights reserved.
//

#ifndef Point_hpp
#define Point_hpp

#include <cmath>

class Point {
public:
    double x, y, z, depth = 0;
    
    Point();
    Point(double x, double y, double z = 0);
    
    static double getDistance(Point a, Point b);
    
    Point operator+(const Point& p);
    Point operator-(const Point& p);
    
    Point operator+(double n);
    Point operator-(double n);
    Point operator*(double n);
    Point operator/(double n);
};

#endif /* Point_hpp */
