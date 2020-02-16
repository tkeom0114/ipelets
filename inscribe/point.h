#ifndef POINT_H
#define POINT_H

#include "ipelet.h"
#include "ipepath.h"
#include "ipepage.h"
#include "ipereference.h"
#include <vector>
#include <algorithm>

using namespace ipe;
using namespace std;

namespace 
{
    double EPS = 1e-8;
}


class Point
{
public:
    int index;
    Vector v;
public:
    Point(int _index, Vector _v);
    Point();
    inline bool operator<(const Point rhs) const;
    inline bool operator==(const Point rhs) const;
};

Point::Point(int _index, Vector _v)
{
    index = _index;
    v = _v;
}

inline bool Point::operator<(const Point rhs) const
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

inline bool Point::operator==(const Point rhs) const
{
    return this->v == rhs.v;
}

#endif