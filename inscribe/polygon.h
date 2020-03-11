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
    static const array<Linear,3> trans;
    void renumbering(int n, DIR dir);
    bool computeVisEach(IpeletHelper *helper, DIR dir);
public:
    vector<PointInfo> points;
    vector<EPair> edgePairs;
    PPair verDiv;
    PPair horDiv;
public:
    Polygon(vector<PointInfo> _points, vector<EPair> _edgePairs);
    Polygon();
    ~Polygon();
    void setPoints(Curve const *curve);
    void transformPoints(Linear const &l);
    bool computeVis(IpeletHelper *helper);
    vector<Polygon> divide(DIR dir); //set Edge div in the function
    void cutting(bool horizontal);
    vector<Vector> compute();
    friend ostream& operator<<(ostream& os, const Polygon& polygon);
};



#endif