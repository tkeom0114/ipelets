#ifndef CUT_H
#define CUT_H

#include "ipelet.h"
#include "ipepath.h"
#include "ipepage.h"
#include "ipereference.h"
#include "point.h"
#include <vector>
#include <algorithm>


using namespace ipe;
using namespace std;

class Edge
{
public:
    int index;
    Segment seg;
public:
    Edge(int _index, Vector _p, Vector _q);
    bool operator<(const Edge &rhs) const;
};

typedef pair<Edge, Edge> EPair;




struct Compare
{
    bool operator()(const EPair& a, const EPair& b) const;
};

void printPair(vector<EPair> EdgePairs);
vector<EPair> slicing(vector<Point> polygon, IpeletHelper *helper);



#endif