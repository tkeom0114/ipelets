#ifndef POINT_H
#define POINT_H

#include "ipelet.h"
#include "ipepath.h"
#include "ipepage.h"
#include "ipereference.h"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace ipe;
using namespace std;

namespace 
{
    double const EPS = 1e-8;
}

enum DIR{
    VER,
    HOR,
    DIAG
};

class Point
{
public:
    int index;
    Vector v;
public:
    Point(int _index, Vector _v):index(_index),v(_v){};
    Point():index(-1),v(Vector(0.0, 0.0)){};
    bool operator<(const Point rhs) const;
    bool operator==(const Point rhs) const;
    bool operator!=(const Point rhs) const;
    friend pair<Point, Point> operator*(const Linear &l, const pair<Point, Point> &ppair);
};

typedef pair<Point, Point> PPair;

bool compareIndex(Point const a, Point const b);
Segment ppairToSeg(PPair &ppair);


#endif