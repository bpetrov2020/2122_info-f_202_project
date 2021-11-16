#include "point.h"

std::ostream &operator<<(std::ostream &o, const Point &p)
{
    o << "(" << p.x << ", " << p.y << ")";
    return o;
}
