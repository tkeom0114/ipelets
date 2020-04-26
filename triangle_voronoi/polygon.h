#ifndef COMPUTE_H
#define COMPUTE_H

#include "ipelet.h"
#include "ipepath.h"
#include "ipepage.h"
#include "ipereference.h"
#include "point.h"
#include "cut.h"
#include "libs.h"
#include <vector>
#include <algorithm>
#include <set>
#include <iostream>
#include <queue>

using namespace std;
using namespace ipe;

/**
 * TODO: erase edgePair if ipelet is complete
 */
class Polygon
{
private:
    bool intersect(const vector<Edge> &edges, const Segment &s, Vector &pt) const;
public:
    vector<Point> points;
    vector<Segment> segments;
    vector<EPair> edgePairs;
public:
    Polygon(vector<Point> _points);
    Polygon();
    ~Polygon();
    void setPoints(Curve const *curve);
    void transform(Linear const &l);
    bool compute(IpeletHelper *helper);
    friend ostream& operator<<(ostream& os, const Polygon& polygon);
};



#endif