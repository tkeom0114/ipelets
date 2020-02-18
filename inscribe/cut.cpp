#include "cut.h"
#include <set>
#include <queue>

inline bool Edge::operator<(const Edge &rhs) const
{
    //not overlapped with respect projection of x-axis
    if (seg.iQ.x < rhs.seg.iP.x)
    {
        return seg.iQ.y + EPS < rhs.seg.iP.y;
    }
    else if (rhs.seg.iQ.x < seg.iP.x)
    {
        return rhs.seg.iQ.y + EPS < seg.iP.y;
    }
    //overlapped with respect projection of x-axis
    else
    {
        double a = ipe::max(seg.iP.x, rhs.seg.iP.x);
        double b = ipe::min(seg.iQ.x, rhs.seg.iQ.x);
        Vector va(a, 0.0), vb(b, 0.0), dir(0.0, 1.0);
        Line la(va, dir), lb(vb, dir);
        Vector lhsa, lhsb, rhsa, rhsb;
        if (!seg.intersects(la, lhsa)) exit(-2);
        if (!seg.intersects(lb, lhsb)) exit(-2);
        if (!rhs.seg.intersects(la, rhsa)) exit(-2);
        if (!rhs.seg.intersects(lb, rhsb)) exit(-2);
        return (lhsa.y + EPS < rhsa.y && lhsb.y < rhsb.y + EPS) || (lhsa.y < rhsa.y +EPS && lhsb.y + EPS < rhsb.y);
    }
}


bool Compare::operator()(const EPair& a, const EPair& b) const
{
    return a.second < b.first;
}

vector<EPair> surroundPairs(Point &q, set<EPair, Compare> &intersectPairs)
{
    vector<EPair> surroundPairs;
    Edge e(0, q.v, q.v);
    pair<set<EPair>::iterator, set<EPair>::iterator> iterPair = intersectPairs.equal_range(pair(e,e));
    for (set<EPair>::iterator it = iterPair.first; it != iterPair.second; it++)
    {
        surroundPairs.push_back(*it);
    }
    return surroundPairs;
}

vector<EPair> slicing(vector<Point> polygon)
{
    vector<Edge> polygonEdges;
    size_t n = polygon.size();
    for (size_t i = 1; i < n; i++)
    {
        Edge e(i-1, polygon[i-1].v, polygon[i].v);
        polygonEdges.push_back(e);
    }
    Edge e(n-1, polygon[n-1].v, polygon[0].v);
    sort(polygon.begin(), polygon.end());
    vector<EPair> edgePairs, tempPairs, surroundPairsP, surroundPairsQ;
    set<EPair, Compare> intersectPairs;
    queue<Point> tempPoints;
    for (size_t i = 0; i < n; i++)
    {
        tempPoints.push(polygon[i]);
        if (i < n - 1 && ipe::abs(polygon[i+1].v.x - polygon[i].v.x) < EPS ) continue;
        Edge *prevEdge = NULL;
        Point p, q;
        int dist = 0;
        do
        {
            //compute vertical segment pg(p:bottom q: top)
            p = tempPoints.front();
            Edge pl(0, p.v, p.v);
            Edge ql(0, q.v, q.v);
            do
            {
                q = tempPoints.front();
                tempPoints.pop();
                if(tempPoints.empty()) break;
                dist = (q.index + n - tempPoints.front().index) % n;
            } while (dist == 1 || dist == n-1);
            surroundPairsP = surroundPairs(p, intersectPairs);
            surroundPairsQ = surroundPairs(q, intersectPairs);
            //create prevEdge if not exist
            if (prevEdge == NULL)
            {
                if (surroundPairsP.empty())
                {
                    if (p == q)
                    {
                        prevEdge = (polygonEdges[p.index] < polygonEdges[(p.index + n -1) % n])? &polygonEdges[p.index]:&polygonEdges[(p.index + n -1) % n];
                    }
                    else if (dist == 1)
                    {
                        prevEdge = &polygonEdges[(p.index+n-1)%n];
                    }
                    else if (dist == n - 1)
                    {
                        prevEdge = &polygonEdges[p.index];
                    }
                    else
                    {
                        exit(-3); //BUG3
                    }
                }
                else if (surroundPairsP.front().first < pl)
                {
                    Vector r;
                    Vector dir(0.0, 1.0);
                    Line l(p.v, dir);
                    if (!surroundPairsP.front().first.seg.intersects(l, r)) exit(-2);
                    prevEdge = new Edge(surroundPairsP.front().first.index, r, surroundPairsP.front().first.seg.iQ);
                }                
            }
            //create vertical edgePair and erase prevEdge
            if (prevEdge != NULL)
            {
                
            }
            //create prevEdge from q

            //erase edgePair from intersectPairs and insert to edgePairs
            if (surroundPairsP.front().second.seg.iQ == p.v || surroundPairsQ.back().second.seg.iQ == q.v)
            {
                edgePairs.push_back(surroundPairsP.front());
                intersectPairs.erase(surroundPairsP.front());
            }
            
            //make pair at the end
            Edge fl(0, q.v, q.v);
            if (!tempPoints.empty())
            {
                Segment fs(tempPoints.front().v, tempPoints.front().v);
                fl.seg = fs;
            } 
            if (prevEdge != NULL && (tempPoints.empty() || surroundPairsQ.empty() || surroundPairsQ.back().second < fl))
            {
                if (!surroundPairsQ.empty() && ql < surroundPairsQ.back().second)
                {
                    /* code */
                }
                else
                {
                    /* code */
                }      
                prevEdge == NULL;
            }
        } while (!tempPoints.empty());
        for (auto &&pair : tempPairs)
        {
            intersectPairs.emplace(pair);
        }
        tempPairs.clear(); 
    }
    return edgePairs;
}