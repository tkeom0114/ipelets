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
    Edge(int _index, Vector p, Vector q);
    inline bool operator<(const Edge &rhs) const;
};

Edge::Edge(int _index, Vector p, Vector q)
{
    index = index;
    Segment _seg(p,q);
    seg = _seg;
}

typedef pair<Edge, Edge> EPair;

vector<EPair> slicing(vector<Point> polygon);

struct Compare
{
    bool operator()(const EPair& a, const EPair& b) const;
};



#endif