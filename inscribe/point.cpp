#include "point.h"

Point::Point(int _index, Vector _v)
{
    index = _index;
    v = _v;
}

bool Point::operator<(const Point rhs) const
{
    if (ipe::abs(this->v.x-rhs.v.x) < EPS)
    {
        return this->v.y < rhs.v.y;
    }
    else
    {
        return this->v.x < rhs.v.x;
    }
}

bool Point::operator==(const Point rhs) const
{
    return this->v == rhs.v;
}