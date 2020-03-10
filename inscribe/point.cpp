#include "point.h"

/*Point::Point(int _index, Vector _v)
{
    index = _index;
    v = _v;
}*/

/*Point::Point()
{
    index = -1;
    v = Vector(0.0, 0.0);
}*/

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

bool Point::operator!=(const Point rhs) const
{
    return !(*this == rhs);
}

PPair operator*(const Linear &l, const PPair &ppair)
{
    return PPair(Point(ppair.first.index, l * ppair.first.v), Point(ppair.second.index, l * ppair.second.v));
}

bool compareIndex(Point const a, Point const b)
{
    return a.index < b.index;
}

Segment ppairToSeg(PPair &ppair)
{
    return Segment(ppair.first.v, ppair.second.v);
}
