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


class Point
{
public:
    int index;
    Vector v;
public:
    Point(int _index, Vector _v);
    Point();
    bool operator<(const Point rhs) const;
    bool operator==(const Point rhs) const;
};

typedef pair<Point, Point> PPair;
void printPair(vector<PPair> PointPairs);

#endif