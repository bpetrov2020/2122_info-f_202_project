#ifndef POINT_HPP
#define POINT_HPP

#include <ostream>

// 2D coordinates
struct Point {
    int x;
    int y;
    Point operator+(const Point &other) const 
    {
        return Point{x+other.x, y+other.y};
    }
    Point operator-(const Point &other) const 
    {
        return Point{x-other.x, y-other.y};
    }
    bool operator==(const Point &other) const = default;
};

std::ostream &operator<<(std::ostream &o, const Point &p);

#endif
