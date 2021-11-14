#include "point.hpp"

/*----------------------------------------------------------
  Point
 *--------------------------------------------------------*/

Point Point::operator+(const Point& other) const
{
    return {x+other.x, y+other.y};
}

Point Point::operator-(const Point& other) const
{
    return {x-other.x, y-other.y};
}

Point Point::operator*(const int& mult) const
{
    return {mult*x, mult*y};
}

std::ostream &operator<<(std::ostream &o, const Point &p)
{
    o << "(" << p.x << ", " << p.y << ")";
    return o;
}

