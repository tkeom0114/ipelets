#include "cut.h"
#include <set>
#include <queue>
#include <iostream>

Edge::Edge(int _index, Vector _p, Vector _q)
{
    index = _index;    
    seg = Segment(_p, _q);
}

bool Edge::operator<(const Edge &rhs) const
{	//debugging
	//cout << "lhs.iP:" << this->seg.iP.x << " " << this->seg.iP.y << endl; //debugging
	//cout << "lhs.iQ:" << this->seg.iQ.x << " " << this->seg.iQ.y << endl; //debugging
	//cout << "rhs.iP:" << rhs.seg.iP.x << " " << rhs.seg.iP.y << endl; //debugging
	//cout << "rhs.iQ:" << rhs.seg.iQ.x << " " << rhs.seg.iQ.y << endl; //debugging
	//debugging
    //not overlapped with respect projection of x-axis
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
        if (!seg.intersects(la, lhsa)) exit(-2);
        if (!seg.intersects(lb, lhsb)) exit(-2);
        if (!rhs.seg.intersects(la, rhsa)) exit(-2);
        if (!rhs.seg.intersects(lb, rhsb)) exit(-2);
		//debugging
		//cout << "lhsa:" << lhsa.x << " " << lhsa.y << endl; //debugging
		//cout << "lhsb:" << lhsb.x << " " << lhsb.y << endl; //debugging
		//cout << "rhsa:" << rhsa.x << " " << rhsa.y << endl; //debugging
		//cout << "rhsb:" << rhsb.x << " " << rhsb.y << endl; //debugging
		//debugging
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
    pair<set<EPair>::iterator, set<EPair>::iterator> iterPair = intersectPairs.equal_range(pair<Edge, Edge>(e,e));
    for (set<EPair>::iterator it = iterPair.first; it != iterPair.second; it++)
    {
        surroundPairs.push_back(*it);
    }
    return surroundPairs;
}

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

void printEdge(Edge e)
{
	cout << "Index:"  << e.index << endl;
	cout << "iP:"  << e.seg.iP.x << " " << e.seg.iP.y << endl;
	cout << "iQ:"  << e.seg.iQ.x << " " << e.seg.iQ.y << endl;
}

vector<EPair> slicing(vector<Point> polygon)
{
    vector<Edge> polygonEdges;
    size_t n = polygon.size();
    for (size_t i = 1; i < n; i++)
    {
        polygonEdges.push_back(Edge(i-1, polygon[i-1].v, polygon[i].v));
    }
    polygonEdges.push_back(Edge(n-1, polygon[n-1].v, polygon[0].v));
    sort(polygon.begin(), polygon.end());
    vector<EPair> edgePairs, tempPairs, surroundPairsP, surroundPairsQ;
    set<EPair, Compare> intersectPairs;
    queue<Point> tempPoints;
    for (size_t i = 0; i < n; i++)
    {
		cout << "iteration " << i << endl; //debugging
        tempPoints.push(polygon[i]);
        if (i < n - 1 && ipe::abs(polygon[i+1].v.x - polygon[i].v.x) < EPS ) continue;
        Edge *prevEdge = NULL;
        Point *p, *q;
        int dist = 0;
        do
        {
            //compute vertical segment pg(p:bottom q: top)
            p = &tempPoints.front();
            Edge pl(0, p->v, p->v);
            do
            {
                q = &tempPoints.front();
                tempPoints.pop();
                if(tempPoints.empty()) break;
                dist = (q->index + n - tempPoints.front().index) % n;
            } while (dist == 1 || dist == static_cast<int>(n - 1));
			Edge ql(0, q->v, q->v);
            surroundPairsP = surroundPairs(*p, intersectPairs);
            surroundPairsQ = surroundPairs(*q, intersectPairs);
			cout << "p:" << p->v.x << " " << p->v.y << endl; // debugging
			cout << "q:" << q->v.x << " " << q->v.y << endl; // debugging
			cout << "intersectPairs.size:" << intersectPairs.size() << endl;
			cout << "surroundPairsP.size:" << surroundPairsP.size() << endl; // debugging
			cout << "surroundPairsQ.size:" << surroundPairsQ.size() << endl; // debugging
            //create prevEdge if not exist
            if (prevEdge == NULL)
            {
                if (surroundPairsP.empty())
                {
                    if (p == q)
                    {
						prevEdge = (polygonEdges[p->index] < polygonEdges[(p->index + n - 1) % n])? &polygonEdges[p->index]:&polygonEdges[(p->index + n - 1) % n];
                    }
                    else if (dist == 1)
                    {
                        prevEdge = &polygonEdges[(p->index + n - 1)%n];
                    }
                    else if (dist == static_cast<int>(n - 1))
                    {
                        prevEdge = &polygonEdges[p->index];
                    }
                    else
                    {
                        exit(-3); //BUG3
                    }
                }
                else if (surroundPairsP.front().first < pl)
                {
					cout << "passed0" << endl; // debugging
                    Vector r;
                    Vector dir(0.0, 1.0);
                    Line l(p->v, dir);
                    //if (!surroundPairsP.front().first.seg.intersects(l, r)) exit(-2);
					surroundPairsP.front().first.seg.intersects(l, r);
                    prevEdge = new Edge(surroundPairsP.front().first.index, r, surroundPairsP.front().first.seg.iQ);
                    intersectPairs.erase(surroundPairsP.front());
                    Edge newpe(surroundPairsP.front().first.index, surroundPairsP.front().first.seg.iP, r);
                    intersectPairs.insert(pair<Edge, Edge>(newpe, surroundPairsP.front().second));
                    surroundPairsP = surroundPairs(*p, intersectPairs);
					surroundPairsQ = surroundPairs(*q, intersectPairs);
                }                
            }
            //create vertical edgePair and erase prevEdge, create prevEdge from q
			if (!surroundPairsP.empty())
			{
				if (surroundPairsP.front().first < pl && pl < surroundPairsP.front().second)
				{
					if (p == q)
					{
						cout << "passed1" << endl; // debugging
						tempPairs.push_back(pair<Edge, Edge>(*prevEdge, std::min(polygonEdges[p->index], polygonEdges[(p->index + n - 1) % n])));
						cout << "p->index:" << p->index << endl; // debugging
						cout << (polygonEdges[p->index] < polygonEdges[(p->index + n - 1) % n]) << endl; // debugging
						printEdge(polygonEdges[p->index]); //debugging
						printEdge( polygonEdges[(p->index + n - 1) % n]); //debugging
						if (polygonEdges[p->index] < polygonEdges[(p->index + n - 1) % n])
						{
							prevEdge = &polygonEdges[(p->index + n - 1) % n];
						}
						else
						{
							prevEdge = &polygonEdges[p->index];
						}
					}
					else if (dist == 1)
					{
						tempPairs.push_back(pair<Edge, Edge>(*prevEdge, polygonEdges[(p->index + n -1) % n]));
						prevEdge = &polygonEdges[q->index];
					}
					else if (dist == static_cast<int>(n - 1))
					{
						tempPairs.push_back(pair<Edge, Edge>(*prevEdge, polygonEdges[p->index]));
						prevEdge = &polygonEdges[(q->index + n -1) % n];
					}
					else
					{
						exit(-3); //BUG3
					}
				}
				//erase edgePair from intersectPairs and insert to edgePairs
				if (!surroundPairsQ.empty() && (surroundPairsP.front().second.seg.iQ == p->v || surroundPairsQ.back().second.seg.iQ == q->v))
				{
					cout << "passed2" << endl; // debugging
					edgePairs.push_back(surroundPairsP.front());
					intersectPairs.erase(surroundPairsP.front());
				}
			}
            
            //make pair at the end
            Edge fl(0, q->v, q->v);
            if (!tempPoints.empty())
            {
				cout << "passed3" << endl; // debugging
                Segment fs(tempPoints.front().v, tempPoints.front().v);
                fl.seg = fs;
            } 
            if (prevEdge != NULL && (tempPoints.empty() || surroundPairsQ.empty() || surroundPairsQ.back().second < fl))
            {
                if (!surroundPairsQ.empty() && ql < surroundPairsQ.back().second)
                {
					cout << "passed4" << endl; // debugging
                    Vector r;
                    Vector dir(0.0, 1.0);
                    Line l(q->v, dir);
                    if (!surroundPairsQ.back().second.seg.intersects(l, r)) exit(-2);
                    tempPairs.push_back(pair<Edge, Edge>(*prevEdge, Edge(surroundPairsQ.back().second.index, r, surroundPairsQ.back().second.seg.iQ)));
                    edgePairs.push_back(pair<Edge, Edge>(surroundPairsQ.back().first, Edge(surroundPairsQ.back().second.index, surroundPairsQ.back().second.seg.iP, r)));
                    intersectPairs.erase(surroundPairsQ.back());
                }
                else
                {
                    if (p == q)
                    {
						cout << prevEdge->index << endl; // debugging
						cout << prevEdge->seg.iQ.x << " " << prevEdge->seg.iQ.y << endl; // debugging
						cout << polygonEdges[prevEdge->index].seg.iP.x << " " << polygonEdges[prevEdge->index].seg.iP.y << endl; // debugging 
						cout << polygonEdges[prevEdge->index].seg.iQ.x << " " << polygonEdges[prevEdge->index].seg.iQ.y << endl; // debugging 
                        if (prevEdge->seg.iQ == polygonEdges[prevEdge->index].seg.iP)
                        {
							cout << "passed5" << endl; // debugging
                            tempPairs.push_back(pair<Edge, Edge>(*prevEdge, polygonEdges[q->index]));
                        }
                        else if (prevEdge->seg.iQ == polygonEdges[prevEdge->index].seg.iQ)
                        {
							cout << "passed6" << endl; // debugging
                            tempPairs.push_back(pair<Edge, Edge>(*prevEdge, polygonEdges[(q->index + n - 1) % n]));
                        }
                        else
                        {
							cout << "passed7" << endl; // debugging
                            exit(-3); //BUG3
                        }
                    }
                    else if (dist == 1)
                    {
                        tempPairs.push_back(pair<Edge, Edge>(*prevEdge, polygonEdges[q->index]));
                    }
                    else if (dist == static_cast<int>(n - 1))
                    {
                        tempPairs.push_back(pair<Edge, Edge>(*prevEdge, polygonEdges[(q->index + n - 1) % n]));
                    }
                    else
                    {
                        exit(-3); //BUG3
                    }
                    if (!surroundPairsP.empty()) intersectPairs.erase(surroundPairsP.front());
                }      
                prevEdge = NULL;
            }
        } while (!tempPoints.empty());
		printPair(tempPairs); // debugging
        for (auto &&pair : tempPairs)
        {
			if (pair.first.seg.iP.x > pair.first.seg.iQ.x && pair.second.seg.iP.x > pair.second.seg.iQ.x)
			{
				intersectPairs.emplace(EPair(Edge(pair.first.index, pair.first.seg.iQ ,pair.first.seg.iP), Edge(pair.second.index,pair.second.seg.iQ ,pair.second.seg.iP)));
			}
			else if (pair.first.seg.iP.x > pair.first.seg.iQ.x)
			{
				intersectPairs.emplace(EPair(Edge(pair.first.index, pair.first.seg.iQ ,pair.first.seg.iP), pair.second));
			}
			else if (pair.second.seg.iP.x > pair.second.seg.iQ.x)
			{
				intersectPairs.emplace(EPair(pair.first, Edge(pair.second.index,pair.second.seg.iQ ,pair.second.seg.iP)));
			}
			else
			{
				intersectPairs.emplace(pair);
			}
        }
		cout << "intersectPairs.size:" << intersectPairs.size() << endl;
        tempPairs.clear(); 
    }
    return edgePairs;
}