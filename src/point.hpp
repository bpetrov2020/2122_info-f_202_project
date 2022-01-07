#ifndef POINT_HPP
#define POINT_HPP

#include <iostream>

/**
  Coordinate in the plan
  */
struct Point
{
    int x;
    int y;

    Point operator+(const Point& other) const;
    Point operator-(const Point& other) const;
    Point operator*(const int& multiplier) const;
    Point operator/(const int& divisor) const;
    bool operator==(const Point& other) const;
};

std::ostream &operator<<(std::ostream &ost, const Point &p);
std::istream &operator>>(std::istream &ist, Point &p);

#endif
