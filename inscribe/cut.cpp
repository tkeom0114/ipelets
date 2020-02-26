#include "cut.h"
#include <set>
#include <queue>
#include <iostream>

Edge::Edge(int _index, Vector _p, Vector _q)
{
    index = _index;    
    seg = Segment(_p, _q);
}

Edge::Edge()
{
	index = -1;    
    seg = Segment();
}

bool Edge::operator<(const Edge &rhs) const
{
	double alx, aly, arx, ary, blx, bly, brx, bry;
	if (seg.iP.x < seg.iQ.x)
	{
		alx = seg.iP.x; aly = seg.iP.y; arx = seg.iQ.x; ary = seg.iQ.y;
	}
	else
	{
		alx = seg.iQ.x; aly = seg.iQ.y; arx = seg.iP.x; ary = seg.iP.y;
	}
	if (rhs.seg.iP.x < rhs.seg.iQ.x)
	{
		blx = rhs.seg.iP.x; bly = rhs.seg.iP.y; brx = rhs.seg.iQ.x; bry = rhs.seg.iQ.y;
	}
	else
	{
		blx = rhs.seg.iQ.x; bly = rhs.seg.iQ.y; brx = rhs.seg.iP.x; bry = rhs.seg.iP.y;
	}
    //not overlapped with respect projection of x-axis
    if (arx < blx)
    {
        return ary + EPS < bly;
    }
    else if (brx < alx)
    {
        return bry + EPS < aly;
    }
    //overlapped with respect projection of x-axis
    else
    {
        double a = ipe::max(alx, blx);
        double b = ipe::min(arx, brx);
        Vector va(a, 0.0), vb(b, 0.0), dir(0.0, 1.0);
        Line la(va, dir), lb(vb, dir);
        Vector lhsa, lhsb, rhsa, rhsb;
        if (!seg.intersects(la, lhsa)) cout << "Error!" << endl;
        if (!seg.intersects(lb, lhsb)) cout << "Error!" << endl;
        if (!rhs.seg.intersects(la, rhsa)) cout << "Error!" << endl;
        if (!rhs.seg.intersects(lb, rhsb)) cout << "Error!" << endl;
        return (lhsa.y + EPS < rhsa.y && lhsb.y < rhsb.y + EPS) || (lhsa.y < rhsa.y +EPS && lhsb.y + EPS < rhsb.y);
    }
}


bool Compare::operator()(const EPair& a, const EPair& b) const
{
    return a.second < b.first;
}



//for debugging
void printPair(vector<EPair> EdgePairs)
{
	for (size_t i = 0; i < EdgePairs.size(); i++)	
	{
		cout << "pair" << i << endl;
		cout << "First index:"  << EdgePairs[i].first.index << endl;
		cout << "First iP:"  << EdgePairs[i].first.seg.iP.x << " " << EdgePairs[i].first.seg.iP.y << endl;
		cout << "First iQ:"  << EdgePairs[i].first.seg.iQ.x << " " << EdgePairs[i].first.seg.iQ.y << endl;
		cout << "Second index:"  << EdgePairs[i].second.index << endl;
		cout << "Second iP:"  << EdgePairs[i].second.seg.iP.x << " " << EdgePairs[i].second.seg.iP.y << endl;
		cout << "Second iQ:"  << EdgePairs[i].second.seg.iQ.x << " " << EdgePairs[i].second.seg.iQ.y << endl;
	}
}

//for debugging
void printEdge(Edge e)
{
	cout << "Index:"  << e.index << endl;
	cout << "iP:"  << e.seg.iP.x << " " << e.seg.iP.y << endl;
	cout << "iQ:"  << e.seg.iQ.x << " " << e.seg.iQ.y << endl;
}
