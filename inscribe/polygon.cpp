#include "polygon.h"

const array<Linear,3> Polygon::trans = {Linear(), Linear(0.0, 1.0, -1.0, 0.0), Linear(-1.0, 1.0, -1.0, -1.0)};

Polygon::Polygon(vector<PointInfo> _points, vector<EPair> _edgePairs)
{
    points = _points;
    edgePairs = _edgePairs;
}

Polygon::Polygon()
{
    points = vector<PointInfo>();
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
    PointInfo p(0,curve->segment(0).cp(0));
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
		points[i].v = l * points[i].v;
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

/**
 * TODO: erase edgePair if ipelet is complete
 */
bool Polygon::computeVis(IpeletHelper *helper, DIR dir)
{
    transformPoints(trans[dir]);
    edgePairs.clear();
    vector<Edge> polygonEdges;
    size_t n = points.size();
    for (size_t i = 1; i < n; i++)
        polygonEdges.push_back(Edge(i-1, points[i-1].v, points[i].v));
    polygonEdges.push_back(Edge(n-1, points[n-1].v, points[0].v));
    sort(points.begin(), points.end());
    vector<EPair> tempPairs, surroundPairsP, surroundPairsQ;
    set<EPair, Compare> intersectPairs;
    queue<PointInfo*> pointsOnLine;
    vector<PointInfo*> pointsOnSegment;
    for (size_t i = 0; i < n; i++)
    {
        pointsOnLine.push(&points[i]);
        if (i < n - 1 && ipe::abs(points[i+1].v.x - points[i].v.x) < EPS ) continue;
        Edge *prevEdge = nullptr;
        PointInfo *p, *q, *top, *bottom;
        shared_ptr<PointInfo> newTop, newBottom;
        top = nullptr; 
        bottom = nullptr;
        do
        {
            //compute vertical segment pg(p:bottom q: top)
            p = pointsOnLine.front();
            Edge pl(0, p->v, p->v);
            int temp = 0, dist = 0;
            do
            {
                q = pointsOnLine.front();
                pointsOnSegment.push_back(q);
                pointsOnLine.pop();
                if(pointsOnLine.empty()) break;
                temp = (pointsOnLine.front()->index - q->index + n) % n;
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
                    if (bottom == nullptr) bottom = p;
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
                    prevEdge = make_shared<Edge>(surroundPairsP.front().first.index, r, surroundPairsP.front().first.seg.iQ).get();
                    intersectPairs.erase(surroundPairsP.front());
                    Edge newpe(surroundPairsP.front().first.index, surroundPairsP.front().first.seg.iP, r);
                    intersectPairs.insert(pair<Edge, Edge>(newpe, surroundPairsP.front().second));
                    surroundPairsP = surroundPairs(*p, intersectPairs);
					surroundPairsQ = surroundPairs(*q, intersectPairs);
                    if (bottom == nullptr)
                    {
                        newBottom = make_shared<PointInfo>(PointInfo(surroundPairsP.front().first.index, r));
                        bottom = newBottom.get();
                    } 
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
                            prevEdge = &polygonEdges[(p->index + n - 1)%n];
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
                        prevEdge = &polygonEdges[q->index];
					}
					else
					{
						helper->message("Error3");//BUG3
                        return false;
					}
                }
                if (surroundPairsP.front().first.seg.iQ == p->v)
                {
                    if (bottom == nullptr) bottom = p;
                }
                
			}
            //make pair at the end
            Edge fl(0, q->v, q->v);
            if (!pointsOnLine.empty())
            {
                Segment fs(pointsOnLine.front()->v, pointsOnLine.front()->v);
                fl.seg = fs;
            } 
            if (pointsOnLine.empty() || surroundPairsQ.empty() || surroundPairsQ.back().second < fl)
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
                    if (top == nullptr)
                    {
                        newTop = make_shared<PointInfo>(surroundPairsQ.back().second.index, r);
                        top = newTop.get();
                    }  
                }
                else
                {
                    if (prevEdge != nullptr)
                    {
                        if (p == q)
                        {
                            if (prevEdge->seg.iQ == polygonEdges[prevEdge->index].seg.iP || prevEdge->seg.iP.x > prevEdge->seg.iQ.x)
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
                        //top = make_unique<Point>(p);
                    }
                    if (!surroundPairsP.empty())
                    {
                        edgePairs.push_back(surroundPairsP.front());
                        intersectPairs.erase(surroundPairsP.front());
                    }
                    if (top == nullptr) top = q;
                }
                prevEdge = nullptr;                 
                if (top && bottom)
                {
                    for (auto &&point : pointsOnSegment)
                    {
                        //point->visible[dir] = PPair(*bottom, *top);
                        point->visible[dir] = trans[dir].inverse() * PPair(*bottom, *top);
                    }
                    //sliceLines[dir].push_back(PPair(*bottom, *top));
                    pointsOnSegment.clear();
                    top = nullptr;
                    bottom = nullptr;
                }
            }        
        } while (!pointsOnLine.empty());
        for (auto &&pair : tempPairs)
        {
			if (pair.first.seg.iP.x > pair.first.seg.iQ.x && pair.second.seg.iP.x > pair.second.seg.iQ.x)
			{
				intersectPairs.emplace(Edge(pair.first.index, pair.first.seg.iQ ,pair.first.seg.iP), 
                                    Edge(pair.second.index,pair.second.seg.iQ ,pair.second.seg.iP));
			}
			else if (pair.first.seg.iP.x > pair.first.seg.iQ.x)
			{
				intersectPairs.emplace(Edge(pair.first.index, pair.first.seg.iQ ,pair.first.seg.iP), pair.second);
			}
			else if (pair.second.seg.iP.x > pair.second.seg.iQ.x)
			{
				intersectPairs.emplace(pair.first, Edge(pair.second.index,pair.second.seg.iQ ,pair.second.seg.iP));
			}
			else
			{
				intersectPairs.emplace(pair);
			}
        }
        tempPairs.clear(); 
    }
    sort(points.begin(), points.end(), compareIndex);
    transformPoints(trans[dir].inverse());
    return true;
}

void Polygon::renumbering(int n, DIR dir)
{
    //erase colinear points
    size_t m = points.size();
    while (m > 1 && std::abs(points[m - 1].v.x - points[m - 2].v.x) < EPS)
    {
        points.pop_back();
        m--;
    }
    for (size_t i = 0; i < m; i++)
    {
        for (size_t d = 0; d < 3; d++)
        {
            if ((points[i] == points.front() || points[i] == points.back()) && d == dir)
            {
                points[i].visible[d] = PPair(std::min(points.front(), points.back()), std::max(points.front(), points.back())); 
                continue;
            }
            else if (points[i].visible[d].first.index == -1)
            {
                Vector a = points[i].v - points[(i + m - 1) % m].v;
                Vector b = points[(i + 1) % m].v - points[i].v;
                Vector c;
                (trans[d].inverse() * Vector(0.0, 1.0)).factorize(c);
                double horDir = std::abs(cross(b, c)) < EPS? cross(a, c):cross(b, c);
                double verDir = horDir * cross(a, b);
                if (std::abs(cross(a, c)) > EPS && std::abs(cross(b, c)) > EPS && cross(a, c) * cross(b, c) < 0)
                {
                    points[i].visible[d] = PPair(points[i], points[i]);
                }
                else
                {
                    bool open = true;
                    bool preOpen = false;
                    Point intersect = points[i];
                    Vector temp;
                    for (size_t j = (i + 1) % m; j != i; j = (j + 1) % m)
                    {
                        temp = points[j].v - points[i].v;
                        if (open && dot(intersect.v - points[i].v, c) * verDir < dot(temp, c) * verDir &&
                            std::abs(cross(temp, c)) < EPS)
                        {
                            intersect = points[j];
                        }
                        else if (open && cross(temp, c) * horDir < -EPS)
                        {
                            if (intersect != points[(j + m - 1) % m])
                            {
                                Segment(points[(j + m - 1) % m].v, points[j].v).intersects(Line(points[i].v, c), intersect.v);
                                intersect.index = points[(j + m - 1) % m].index;
                            }
                            open = false;
                            preOpen = false;
                        }
                        else if (preOpen && std::abs(cross(temp, c)) < EPS && 
                                dot(intersect.v - points[i].v, c) * verDir > dot(temp, c) * verDir)
                        {
                            intersect = points[j];
                            open = true;
                        }
                        else if (!open && std::abs(cross(temp, c) > EPS))
                        {
                            Vector r = points[(j + m - 1) % m].v;
                            bool isIntersect = 
                            (std::abs(cross(r - points[i].v, c)) > EPS && Segment(points[(j + m - 1) % m].v, points[j].v).intersects(Line(points[i].v, c), r))
                            || (std::abs(cross(points[(j + m - 1) % m].v - points[i].v, c)) < EPS && cross(temp, c) * horDir > EPS);
                            if (isIntersect && dot(intersect.v - points[i].v, c) * verDir > dot(r, c) * verDir)
                            {
                                intersect.v = r;
                                intersect.index = points[(j + m - 1) % m].index;
                                preOpen = !preOpen;
                            }
                        }   
                    }
                    points[i].visible[d] = PPair(std::min(Point(points[i]), intersect), std::max(Point(points[i]), intersect));
                }
                continue;
            }     
            Point p = Point(points.back().index, Vector());
            bool intersect = ppairToSeg(points[i].visible[d]).intersects(Segment(points.front().v, points.back().v), p.v);
            bool frontOn = ppairToSeg(points[i].visible[d]).distance(points.front().v) < EPS;
            bool backOn = ppairToSeg(points[i].visible[d]).distance(points.back().v) < EPS;
            if (intersect || frontOn || backOn)
            {
                if (frontOn) p = points.front();
                if (backOn) p = points.back();
                double discriminant = (dir == DIR::VER)? points[1].v.x - points[0].v.x:points[1].v.y - points[0].v.y;
                double first = (dir == DIR::VER)? points[i].visible[d].first.v.x - points[0].v.x:
                                            points[i].visible[d].first.v.y - points[0].v.y;
                double second = (dir == DIR::VER)? points[i].visible[d].second.v.x - points[0].v.x:
                                            points[i].visible[d].second.v.y - points[0].v.y;
                if (std::abs(first) > EPS && first * discriminant < 0)
                    points[i].visible[d].first = p;
                if (std::abs(second) > EPS && second * discriminant < 0)
                    points[i].visible[d].second = p;
            } 
        }
    }
    int slice = points[0].index;
    for (auto &&point : points)
    {
        point.index = (point.index + n - slice) % n;
        for (int dir = 0; dir < 3; dir++)
        {
            point.visible[dir].first.index = (point.visible[dir].first.index + n - slice) % n;
            point.visible[dir].second.index = (point.visible[dir].second.index + n - slice) % n;
        }
    }
    if (verDiv.first.index != -1)
    {
        verDiv.first.index = (verDiv.first.index + n - slice) % n;
        verDiv.second.index = (verDiv.second.index + n - slice) % n;
    }
}

vector<Polygon> Polygon::divide(DIR dir)
{
    int n = static_cast<int>(points.size());
    int temp = 0;
    PPair &div = (dir == HOR)? horDiv:verDiv;
    for (auto &&point : points)
    {
        int min = std::min(point.visible[dir].first.index, point.visible[dir].second.index);
        int max = std::max(point.visible[dir].first.index, point.visible[dir].second.index);
        int dist = std::min(max - min, min + n - max);
        if (dist > temp)
        {
            div = point.visible[dir];
            temp = dist;
        } 
    }
    vector<Polygon> polygons;
    vector<int> segToPol;
    segToPol.assign(n, -1);
    PointInfo first = (div.first == points[div.first.index])? points[div.first.index]:PointInfo(div.first);
    int start = first.index;
    start = (start + 1) % n;
    temp = start;
    bool touchedPrev = true;
    do
    {
        bool touched = ppairToSeg(div).distance(points[temp].v) < EPS;
        if (touchedPrev && !touched)
        {
            polygons.push_back(Polygon());
            polygons.back().points.push_back(first);  
        } 
        if (!touchedPrev || !touched) 
        {
            segToPol[temp] = polygons.size() - 1;
            polygons.back().points.push_back(points[temp]);
            if (!touched && temp == div.second.index)
            {
                first = PointInfo(div.second);
                first.index = (temp + 1) % n;
                polygons.back().points.push_back(first);
                first.index = temp;
            }
            else if (!touched && temp == div.first.index)
            {
                first = PointInfo(div.first);
                first.index = (temp + 1) % n;
                polygons.back().points.push_back(first);
                first.index = temp;
            }
        }
        if (touched) first = points[temp];      
        touchedPrev = touched || (temp == div.second.index);
        temp = (temp + 1) % n;
    } while (temp != start);
    //divide verDiv
    if (dir == HOR)
    {
        vector<Polygon> newPolygons;
        Vector r;
        if (ppairToSeg(horDiv).intersects(ppairToSeg(verDiv), r) && Vector(r - verDiv.second.v).len() > EPS)
        {
            //get lower part
            if (verDiv.first.index != horDiv.first.index)
                newPolygons.push_back(polygons[segToPol[(verDiv.first.index + 1) % n]]);
            else if ((verDiv.first.v.x - horDiv.first.v.x)*(verDiv.first.v.x - points[horDiv.first.index].v.x) > 0)
                newPolygons.push_back(polygons.front());
            else
                newPolygons.push_back(polygons.back());
            //get upper part
            if (verDiv.second.index != horDiv.second.index)
                newPolygons.push_back(polygons[segToPol[(verDiv.second.index + 1) % n]]);
            else if ((verDiv.second.v.x - horDiv.second.v.x)*(verDiv.second.v.x - points[horDiv.second.index].v.x) > 0)
                newPolygons.push_back(polygons.front());
            else
                newPolygons.push_back(polygons.back());
            newPolygons.front().verDiv = PPair(verDiv.first, Point(newPolygons.front().points.back().index, r));
            newPolygons.back().verDiv = PPair(Point(newPolygons.back().points.back().index, r), verDiv.second);
        }
        else
        {
            if (verDiv.first.index != horDiv.first.index)
                newPolygons.push_back(polygons[segToPol[(verDiv.first.index + 1) % n]]);
            else if (verDiv.second.index != horDiv.second.index)
                newPolygons.push_back(polygons[segToPol[(verDiv.second.index + 1) % n]]);
            else if ((verDiv.first.v.x - horDiv.first.v.x)*(verDiv.first.v.x - points[horDiv.first.index].v.x) > 0)
                newPolygons.push_back(polygons.front());
            else
                newPolygons.push_back(polygons.back());
            newPolygons.front().verDiv = verDiv;
        }
        polygons = newPolygons;
    }
    for (auto &&polygon : polygons)
        polygon.renumbering(n, dir);
    return polygons;
}

void Polygon::cutting(bool horizontal)
{
    vector<Vector> points;
}


vector<Vector> Polygon::compute()
{
    double ar = 0.0;
    //base case
    if (edgePairs.size() == 1)
    {

    }
    //divide
    vector<Vector> triangle;
    for (auto &&polygon : divide(DIR::VER))
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
    for (auto &&point : polygon.points)
    {
        cout << "<<" << "Index:" << point.index << " x:" << point.v.x << " y:" << point.v.y << ">>" << endl;
        for (size_t dir = 0; dir < 3; dir++)
        {
            switch (dir)
            {
            case DIR::VER:
                cout << "Vertical" << endl;
                break;
            case DIR::HOR:
                cout << "Horizontal" << endl;
                break;
            case DIR::DIAG:
                cout << "Diagonal" << endl;
                break;
            default:
                break;
            }
            cout << "First index:"  << point.visible[dir].first.index << endl;
            cout << "First vector:"  << point.visible[dir].first.v.x << " " << point.visible[dir].first.v.y << endl;
            cout << "Second index:"  << point.visible[dir].second.index << endl;
            cout << "Second vector:"  << point.visible[dir].second.v.x << " " << point.visible[dir].second.v.y << endl;
        }
    }
    return os;
}