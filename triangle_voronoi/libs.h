#ifndef LIBS_H
#define LIBS_H

#include "ipelet.h"
#include "ipepath.h"
#include "ipepage.h"
#include "ipereference.h"
#include "point.h"
#include <vector>
#include <algorithm>

using namespace ipe;
using namespace std;


void setVectors(vector<Point> &polygon, Curve const *curve);
double area(vector<Vector> const &polygon);
double cross(const Vector &a, const Vector &b);

#endif