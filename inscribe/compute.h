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

class Polygon
{
public:
    vector<Point> points;
    vector<EPair> edgePairs;
    Edge div;
public:
    Polygon(vector<Point> _points, vector<EPair> _edgePairs);
    Polygon();
    ~Polygon();
    void setPoints(Curve const *curve);
    void transformPoints(Linear const &l);
    bool slicing(IpeletHelper *helper);
    vector<Polygon> divide(); //set Edge div in the function
    Polygon cutting();
    vector<Vector> compute();
};


#endif