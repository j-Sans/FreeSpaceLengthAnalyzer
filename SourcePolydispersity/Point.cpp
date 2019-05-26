//
//  Point.cpp
//  SquareHeuristic
//
//  Created by Jake Sanders on 6/29/17.
//  Copyright © 2017 Jake Sanders. All rights reserved.
//

#include "Point.hpp"

Point::Point() {
	Point(0, 0);
}

Point::Point(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

// Static

double Point::getDistance(Point a, Point b) {
	return hypot(b.x - a.x, b.y - a.y);
}

// Public

Point Point::operator+(const Point& p) {
	this->x += p.x;
	this->y += p.y;
	return *this;
}

Point Point::operator-(const Point& p) {
	this->x -= p.x;
	this->y -= p.y;
	return *this;
}

Point Point::operator+(double n) {
	this->x += n;
	this->y += n;
	return *this;
}

Point Point::operator-(double n) {
	this->x -= n;
	this->y -= n;
	return *this;
}

Point Point::operator*(double n) {
	this->x *= n;
	this->y *= n;
	return *this;
}

Point Point::operator/(double n) {
	this->x /= n;
	this->y /= n;
	return *this;
}
