#include "polygon.h"

Polygon::Polygon(vector<Point> _points, vector<EPair> _edgePairs)
{
    points = _points;
    edgePairs = _edgePairs;
}

Polygon::Polygon()
{
    points = vector<Point>();
    edgePairs = vector<EPair>();
    verDiv = PPair();
    horDiv = PPair();
}

Polygon::~Polygon()
{
    points.clear();
    edgePairs.clear();
}

void Polygon::setPoints(Curve const *curve)
{
    points.clear();
    Point p(0,curve->segment(0).cp(0));
	points.push_back(p);
	for (int i = 0; i < curve->countSegments(); i++)
	{
        p.index = i + 1; p.v = curve->segment(i).last();
		points.push_back(p);
	}
}

void Polygon::transformPoints(Linear const &l)
{
    for (size_t i = 0; i < points.size(); i++)
		points[i].v = l.inverse() * points[i].v;
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

bool Polygon::slicing(IpeletHelper *helper)
{
    edgePairs.clear();
    sliceLines.clear();
    vector<Edge> polygonEdges;
    size_t n = points.size();
    for (size_t i = 1; i < n; i++)
    {
        polygonEdges.push_back(Edge(i-1, points[i-1].v, points[i].v));
    }
    polygonEdges.push_back(Edge(n-1, points[n-1].v, points[0].v));
    sort(points.begin(), points.end());
    vector<EPair> tempPairs, surroundPairsP, surroundPairsQ;
    set<EPair, Compare> intersectPairs;
    queue<Point> tempPoints;
    for (size_t i = 0; i < n; i++)
    {
        tempPoints.push(points[i]);
        if (i < n - 1 && ipe::abs(points[i+1].v.x - points[i].v.x) < EPS ) continue;
        Edge *prevEdge = nullptr;
        Edge *edgePointer = nullptr;
        Point *p, *q;
        Point *top = nullptr;
        Point *bottom = nullptr;
        do
        {
            //compute vertical segment pg(p:bottom q: top)
            p = &tempPoints.front();
            Edge pl(0, p->v, p->v);
            q = nullptr;
            int temp = 0, dist = 0;
            do
            {
                q = &tempPoints.front();
                tempPoints.pop();
                if(tempPoints.empty()) break;
                temp = (tempPoints.front().index - q->index + n) % n;
                if (dist == 0)
                {
                    dist = temp;
                }
            } while (temp == 1 || temp == static_cast<int>(n - 1));
			Edge ql(0, q->v, q->v);
            surroundPairsP = surroundPairs(*p, intersectPairs);
            surroundPairsQ = surroundPairs(*q, intersectPairs);
            //create prevEdge if not exist
            if (prevEdge == nullptr)
            {
                if (surroundPairsP.empty())
                {
                    if (p == q)
                    {
						prevEdge = (polygonEdges[p->index] < polygonEdges[(p->index + n - 1) % n])? 
                                    &polygonEdges[p->index]:&polygonEdges[(p->index + n - 1) % n];
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
                        helper->message("Error1");//BUG1
                        return false;
                    }
                    bottom = new Point(*q);
                }
                else if (surroundPairsP.front().first < pl)
                {
                    Vector r;
                    Vector dir(0.0, 1.0);
                    Line l(p->v, dir);
                    if (!surroundPairsP.front().first.seg.intersects(l, r))
                    {
                        r = surroundPairsP.front().first.seg.iQ;
                    }
                    edgePointer = new Edge(surroundPairsP.front().first.index, r, surroundPairsP.front().first.seg.iQ);
                    prevEdge = edgePointer;
                    intersectPairs.erase(surroundPairsP.front());
                    Edge newpe(surroundPairsP.front().first.index, surroundPairsP.front().first.seg.iP, r);
                    intersectPairs.insert(pair<Edge, Edge>(newpe, surroundPairsP.front().second));
                    surroundPairsP = surroundPairs(*p, intersectPairs);
					surroundPairsQ = surroundPairs(*q, intersectPairs);
                    bottom = new Point(surroundPairsP.front().first.index, r);
                }
            }
            if (prevEdge != nullptr && !surroundPairsP.empty() && !surroundPairsQ.empty() && 
                surroundPairsP.front().second.seg.iQ == p->v && surroundPairsQ.back().first.seg.iQ == q->v)
            {
                edgePairs.push_back(surroundPairsP.front());
                intersectPairs.erase(surroundPairsP.front());
            }
            //create vertical edgePair and erase prevEdge(if exist), create prevEdge from q
			if (!surroundPairsP.empty())
			{
				if (surroundPairsP.front().first < pl && ql < surroundPairsP.front().second)
				{
					if (p == q)
					{
						tempPairs.push_back(pair<Edge, Edge>(*prevEdge, std::min(polygonEdges[p->index], polygonEdges[(p->index + n - 1) % n])));
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
						helper->message("Error2");//BUG2
                        return false;
					}
				}
                else if (surroundPairsP.front().first.seg.iQ == p->v && ql < surroundPairsP.front().second)
                {
                    if (p == q)
					{
						if (surroundPairsP.front().first.index == p->index)
                        {
                            prevEdge = &polygonEdges[(q->index + n -1) % n];
                        }
                        else
                        {
                            prevEdge = &polygonEdges[q->index];
                        }
					}
					else if (dist == 1)
					{
						prevEdge = &polygonEdges[q->index];
					}
					else if (dist == static_cast<int>(n - 1))
					{
						prevEdge = &polygonEdges[(q->index + n -1) % n];
					}
					else
					{
						helper->message("Error3");//BUG3
                        return false;
					}
                    bottom = new Point(*q);
                }
			}
            //make pair at the end
            Edge fl(0, q->v, q->v);
            if (!tempPoints.empty())
            {
                Segment fs(tempPoints.front().v, tempPoints.front().v);
                fl.seg = fs;
            } 
            if (tempPoints.empty() || surroundPairsQ.empty() || surroundPairsQ.back().second < fl)
            {
                if (!surroundPairsQ.empty() && ql < surroundPairsQ.back().second)
                {
                    Vector r;
                    Vector dir(0.0, 1.0);
                    Line l(q->v, dir);
                    if (!surroundPairsQ.back().second.seg.intersects(l, r))
                    {
                        //r = surroundPairsQ.back().second.seg.iQ;
                        helper->message("Error0");//BUG0
                        return false;
                    }
                    tempPairs.push_back(pair<Edge, Edge>(*prevEdge, Edge(surroundPairsQ.back().second.index, r, surroundPairsQ.back().second.seg.iQ)));
                    edgePairs.push_back(pair<Edge, Edge>(surroundPairsQ.back().first, Edge(surroundPairsQ.back().second.index, surroundPairsQ.back().second.seg.iP, r)));
                    intersectPairs.erase(surroundPairsQ.back());
                    top = new Point(surroundPairsQ.back().second.index, r);
                }
                else
                {
                    if (prevEdge != nullptr)
                    {
                        if (p == q)
                        {
                            if (prevEdge->seg.iQ == polygonEdges[prevEdge->index].seg.iP)
                            {
                                tempPairs.push_back(pair<Edge, Edge>(*prevEdge, polygonEdges[q->index]));
                            }
                            else if (prevEdge->seg.iQ == polygonEdges[prevEdge->index].seg.iQ)
                            {
                                tempPairs.push_back(pair<Edge, Edge>(*prevEdge, polygonEdges[(q->index + n - 1) % n]));
                            }
                            else
                            {
                                helper->message("Error4");//BUG4
                                return false;
                            }
                        }
                        else if (surroundPairsP.empty() || surroundPairsQ.empty())
                        {
                            if (dist == 1)
                            {
                                tempPairs.push_back(pair<Edge, Edge>(*prevEdge, polygonEdges[q->index]));
                            }
                            else if (dist == static_cast<int>(n - 1))
                            {
                                tempPairs.push_back(pair<Edge, Edge>(*prevEdge, polygonEdges[(q->index + n - 1) % n]));
                            }
                            else
                            {
                                helper->message("Error5");//BUG5
                                return false;
                            }
                        }
                        else
                        {
                            if (dist == 1)
                            {
                                tempPairs.push_back(pair<Edge, Edge>(*prevEdge, polygonEdges[(p->index + n - 1) % n]));
                            }
                            else if (dist == static_cast<int>(n - 1))
                            {
                                tempPairs.push_back(pair<Edge, Edge>(*prevEdge, polygonEdges[p->index]));
                            }
                            else
                            {
                                helper->message("Error6");//BUG6
                                return false;
                            }
                        }
                        top = new Point(*p);    
                    }
                    if (!surroundPairsP.empty())
                    {
                        edgePairs.push_back(surroundPairsP.front());
                        intersectPairs.erase(surroundPairsP.front());
                    }
                }      
                prevEdge = nullptr;
                if (edgePointer != nullptr) delete edgePointer;
                if (top && bottom && top->v.y > bottom->v.y)
                    sliceLines.push_back(PPair(*bottom, *top));  
                if (top != nullptr)
                {
                    delete top;
                    top = nullptr;
                } 
                if (bottom != nullptr)
                {
                    delete bottom;
                    bottom = nullptr;
                } 
            }        
        } while (!tempPoints.empty());
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
        tempPairs.clear(); 
    }
    sort(points.begin(), points.end(), compareIndex);
    //printPair(sliceLines);
    return true;
}

void Polygon::renumbering(int n, bool horizontal)
{
    //erase colinear points
    size_t m = points.size();
    while (m > 1 && std::abs(points[m - 1].v.x - points[m - 2].v.x) < EPS)
    {
        points.pop_back();
        m--;
    }
    int slice = points[0].index;
    for (auto &&point : points)
        point.index = (point.index + n - slice) % n;
    for (auto &&line : sliceLines)
    {
        line.first.index = (line.first.index + n - slice) % n;
        line.second.index = (line.second.index + n - slice) % n;
    }
    if (verDiv.first.index != -1)
    {
        verDiv.first.index = (verDiv.first.index + n - slice) % n;
        verDiv.second.index = (verDiv.second.index + n - slice) % n;
    }
}

vector<Polygon> Polygon::divide(bool horizontal)
{
    cout << "divide!!!" << endl; //debugging
    int n = static_cast<int>(points.size());
    int temp = 0;
    for (auto &&line : sliceLines)
    {
        int min = std::min(line.first.index, line.second.index);
        int max = std::max(line.first.index, line.second.index);
        int dist = std::min(max - min, min + n - max);
        if (dist > temp)
        {
            (horizontal? horDiv:verDiv) = line;
        }
    }
    vector<Polygon> polygons;
    vector<int> segToPol, slidePol;
    segToPol.assign(n, -1);
    int start = horizontal? horDiv.first.index:verDiv.first.index;
    start = (start + 1) % n;
    temp = start;
    bool touchedPrev = true;
    Point first(horizontal? horDiv.first:verDiv.first);
    PPair div = (horizontal)? horDiv:verDiv;
    do
    {
        bool touched = ppairToSeg(div).distance(points[temp].v) < EPS;
        if (touchedPrev && !touched)
        {
            polygons.push_back(Polygon());
            slidePol.push_back(first.index);
            polygons.back().points.push_back(first);  
        } 
        if (!touchedPrev || !touched) 
        {
            segToPol[temp] = polygons.size() - 1;
            polygons.back().points.push_back(points[temp]);
            if (!touched && temp == div.second.index)
            {
                first = div.second;
                first.index = (temp + 1) % n;
                polygons.back().points.push_back(first);
                first.index = (temp + n - 1) % n;
            }
            else if (!touched && temp == div.first.index)
            {
                first = div.first;
                first.index = (temp + 1) % n;
                polygons.back().points.push_back(first);
                first.index = (temp + n - 1) % n;
            }
        }
        if (touched)
        {
            first = points[temp];
        }        
        touchedPrev = touched || (temp == div.second.index);
        temp = (temp + 1) % n;
    } while (temp != start);
    for (auto &&line : sliceLines)
    {
        if (line == div)
            continue;
        if (line.first.index != div.first.index)
        {
            polygons[segToPol[(line.first.index + 1) % n]].sliceLines.push_back(line);
        }
        else if (line.second.index != div.second.index)
        {
            polygons[segToPol[(line.second.index + 1) % n]].sliceLines.push_back(line);
        }
        else if ((horizontal && (line.first.v.y - div.first.v.y)*(line.first.v.y - points[div.first.index].v.y) > 0) ||
                (!horizontal && (line.first.v.x - div.first.v.x)*(line.first.v.x - points[div.first.index].v.x) > 0))
        {
            polygons.front().sliceLines.push_back(line);
        }
        else
        {
            polygons.back().sliceLines.push_back(line);
        }
    }
    //divide verDiv
    if (horizontal)
    {
        vector<Polygon> newPolygons;
        vector<int> newSlidePol;
        Vector r;
        if (ppairToSeg(horDiv).intersects(ppairToSeg(verDiv), r) && Vector(r - verDiv.second.v).len() > EPS)
        {
            //get lower part
            if (verDiv.first.index != horDiv.first.index)
            {
                newPolygons.push_back(polygons[segToPol[(verDiv.first.index + 1) % n]]);
            }
            else if ((verDiv.first.v.x - horDiv.first.v.x)*(verDiv.first.v.x - points[horDiv.first.index].v.x) > 0)
            {
                newPolygons.push_back(polygons.front());
            }
            else
            {
                newPolygons.push_back(polygons.back());
            }
            //get upper part
            if (verDiv.second.index != horDiv.second.index)
            {
                newPolygons.push_back(polygons[segToPol[(verDiv.second.index + 1) % n]]);
            }
            else if ((verDiv.second.v.x - horDiv.second.v.x)*(verDiv.second.v.x - points[horDiv.second.index].v.x) > 0)
            {
                newPolygons.push_back(polygons.front());
            }
            else
            {
                newPolygons.push_back(polygons.back());
            }
            newPolygons.front().verDiv = PPair(verDiv.first, Point(newPolygons.front().points.back().index, r));
            newPolygons.back().verDiv = PPair(Point(newPolygons.back().points.back().index, r), verDiv.second);
        }
        else
        {
            if (verDiv.first.index != horDiv.first.index)
            {
                newPolygons.push_back(polygons[segToPol[(verDiv.first.index + 1) % n]]);
            }
            else if (verDiv.second.index != horDiv.second.index)
            {
                newPolygons.push_back(polygons[segToPol[(verDiv.second.index + 1) % n]]);
            }
            else if ((verDiv.first.v.x - horDiv.first.v.x)*(verDiv.first.v.x - points[horDiv.first.index].v.x) > 0)
            {
                newPolygons.push_back(polygons.front());
            }
            else
            {
                newPolygons.push_back(polygons.back());
            }
            newPolygons.front().verDiv = verDiv;
        }
        polygons = newPolygons;
        slidePol = newSlidePol;
    }
    for (auto &&polygon : polygons)
    {
        polygon.renumbering(n, horizontal);
    }
    return polygons;
}

Polygon Polygon::cutting(bool horizontal)
{
    vector<Vector> points;
    return Polygon();
}


vector<Vector> Polygon::compute()
{
    //base case
    if (edgePairs.size() == 1)
    {

    }
    //divide
    vector<Vector> triangle;
    double ar = 0.0;
    for (auto &&polygon : divide(false))
    {
        vector<Vector> temp = polygon.compute();
        if (ar < area(temp))
        {
            triangle.clear();
            triangle = temp;
            ar = area(temp);
        }
    }
    //conqure
    
    return triangle;
}

ostream& operator<<(ostream& os, const Polygon& polygon)
{
    cout << "Points" << endl;
    for (auto &&point : polygon.points)
    {
        cout << "Index:" << point.index << " x:" << point.v.x << " y:" << point.v.y << endl;
    }
    cout << "SliceLines" << endl;
    int count = 0;
    for (auto &&line : polygon.sliceLines)
    {
        cout << "pair" << count << endl;
		cout << "First index:"  << line.first.index << endl;
		cout << "First vector:"  << line.first.v.x << " " << line.first.v.y << endl;
		cout << "Second index:"  << line.second.index << endl;
		cout << "Second vector:"  << line.second.v.x << " " << line.second.v.y << endl;
        count++;
    }
    return os;
}