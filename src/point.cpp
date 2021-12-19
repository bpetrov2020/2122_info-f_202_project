#include "point.hpp"

/*----------------------------------------------------------
 * Point
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

Point Point::operator/(const int& div) const
{
    return {x/div, y/div};
}

bool Point::operator==(const Point& other) const
{
    return x==other.x && y==other.y;
}

std::ostream &operator<<(std::ostream &ost, const Point &p)
{
    ost << static_cast<char>(p.x+'a') << p.y+1;
    return ost;
}

std::istream &operator>>(std::istream &ist, Point &p)
{
    char x;
    int y;
    ist >> x >> y;
    if (!ist) return ist;

    p = Point{static_cast<int>(x-'a'), y-1};
    return ist;
}
