#include "polygon.h"

Polygon::Polygon(vector<Point> _points)
{
    points = _points;
}

Polygon::Polygon()
{
    points = vector<Point>();
    segments = vector<Segment>();
    edgePairs = vector<EPair>();
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
    cout << "Input" << endl;//debugging
    cout << "Index 0 " << p.v.x << " " << p.v.y << endl;//debugging
    for (int i = 0; i < curve->countSegments(); i++)
    {
        p.index = i + 1; p.v = curve->segment(i).last();
        points.push_back(p);
        cout << "Index " << p.index << " " << p.v.x << " " << p.v.y << endl;//debugging
    }
}

void Polygon::transform(Linear const &l)
{
    for (size_t i = 0; i < points.size(); i++)
        points[i].v = l * points[i].v;
    for (size_t i = 0; i < segments.size(); i++)
    {
        segments[i].iP = l * segments[i].iP;
        segments[i].iQ = l * segments[i].iQ;
    }
    
}

bool Polygon::intersect(const vector<Edge> &edges, const Segment &s, Vector &pt) const
{
    bool isIntersect = false;
    Vector temp;
    for (auto &&edge : edges)
    {
        if ((s.iP-edge.seg.iP).sqLen() < EPS || (s.iP-edge.seg.iQ).sqLen() < EPS)
            continue;
        if (s.intersects(edge.seg, temp) && std::abs(temp.x - s.iP.x) > EPS && (!isIntersect | (pt.x < temp.x)))
        {
            pt = temp;
            isIntersect = true;
        }
        if (s.distance(edge.seg.iP) < EPS && std::abs(edge.seg.iP.x - s.iP.x) > EPS && (!isIntersect | (pt.x < edge.seg.iP.x)))
        {
            pt = edge.seg.iP;
            isIntersect = true;
        }
        if (s.distance(edge.seg.iQ) < EPS && std::abs(edge.seg.iQ.x - s.iP.x) > EPS && (!isIntersect | (pt.x < edge.seg.iQ.x)))
        {
            pt = edge.seg.iQ;
            isIntersect = true;
        }
    }
    for (auto &&segment : segments)
    {
        if (s.intersects(segment, temp) && std::abs(temp.x - s.iP.x) > EPS && (!isIntersect | (pt.x < temp.x)))
        {
            pt = temp;
            isIntersect = true;
        }
        if (s.distance(segment.iP) < EPS && std::abs(segment.iP.x - s.iP.x) > EPS && (!isIntersect | (pt.x < segment.iP.x)))
        {
            pt = segment.iP;
            isIntersect = true;
        }
        if (s.distance(segment.iQ) < EPS && std::abs(segment.iQ.x - s.iP.x) > EPS && (!isIntersect | (pt.x < segment.iQ.x)))
        {
            pt = segment.iQ;
            isIntersect = true;
        }
    }
    return isIntersect;
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
bool Polygon::compute(IpeletHelper *helper)
{
    edgePairs.clear();
    segments.clear();
    vector<Edge> polygonEdges;
    size_t n = points.size();
    for (size_t i = 1; i < n; i++)
        polygonEdges.push_back(Edge(i-1, points[i-1].v, points[i].v));
    polygonEdges.push_back(Edge(n-1, points[n-1].v, points[0].v));
    sort(points.begin(), points.end());
    vector<Segment> tempSegs;
    vector<EPair> tempPairs, surroundPairsP, surroundPairsQ;
    set<EPair, Compare> intersectPairs;
    queue<Point*> pointsOnLine;
    vector<Point*> pointsOnSegment;
    for (size_t i = 0; i < n; i++)
    {
        pointsOnLine.push(&points[i]);
        if (i < n - 1 && ipe::abs(points[i+1].v.x - points[i].v.x) < EPS ) continue;
        Edge *prevEdge = nullptr;
        Point *p, *q;
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
                    //TODO:ray from bottom segment pq
                    if (!surroundPairsQ.empty())
                    {
                        Vector top;
                        if (std::abs(surroundPairsQ.front().second.seg.iQ.x - surroundPairsQ.front().first.seg.iQ.x) < EPS)
                        {
                            top = surroundPairsQ.front().second.seg.iQ;
                        }
                        else
                        {
                            Vector dir(0.0, 1.0);
                            Line l(q->v, dir);
                            surroundPairsQ.front().second.seg.intersects(l, top);
                        }
                        Segment tempSeg = surroundPairsQ.front().first.seg;
                        Vector tempVec1, tempVec2, tempVec3;
                        if (tempSeg.iQ.x - tempSeg.iP.x > tempSeg.iP.y - tempSeg.iQ.y)
                        {
                            tempVec1.x = q->v.x + (q->v.y - top.y) / 2.0;
                            tempVec1.y = (q->v.y + top.y) / 2.0;
                            if (intersect(polygonEdges, Segment(q->v, tempVec1), tempVec2))
                            {
                                tempSegs.push_back(Segment(q->v, tempVec2));
                            }
                            else
                            {
                                tempVec2.x = surroundPairsQ.front().first.seg.iP.x + 
                                            (surroundPairsQ.front().first.seg.iP.y - surroundPairsQ.front().second.seg.iP.y) / 2.0;
                                tempVec2.y = (surroundPairsQ.front().first.seg.iP.y + surroundPairsQ.front().second.seg.iP.y) / 2.0;
                                if (intersect(polygonEdges, Segment(tempVec1, tempVec2), tempVec3))
                                    tempSegs.push_back(Segment(tempVec1, tempVec3));
                                else
                                    tempSegs.push_back(Segment(tempVec1, tempVec2));
                                tempSegs.push_back(Segment(q->v, tempVec1));
                            }
                        }
                    }
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
                }
            }
            //TODO:ray from middle segment pq
            if (prevEdge != nullptr && !surroundPairsP.empty() && !surroundPairsQ.empty() && 
                surroundPairsP.front().second.seg.iQ == p->v && surroundPairsQ.back().first.seg.iQ == q->v)
            {
                //for p
                edgePairs.push_back(surroundPairsP.front());
                intersectPairs.erase(surroundPairsP.front());
                Segment tempSeg = surroundPairsP.front().second.seg;
                Vector tempVec1, tempVec2, tempVec3;
                if (tempSeg.iQ.x - tempSeg.iP.x > tempSeg.iQ.y - tempSeg.iP.y)
                {
                    tempVec1.x = surroundPairsP.front().first.seg.iQ.x + 
                                (surroundPairsP.front().first.seg.iQ.y - surroundPairsP.front().second.seg.iQ.y) / 2.0;
                    tempVec1.y = (surroundPairsP.front().first.seg.iQ.y + surroundPairsP.front().second.seg.iQ.y) / 2.0;
                    if (intersect(polygonEdges, Segment(p->v, tempVec1), tempVec2))
                    {
                        tempSegs.push_back(Segment(p->v, tempVec2));
                    }
                    else
                    {
                        tempVec2.x = surroundPairsP.front().first.seg.iP.x + 
                                    (surroundPairsP.front().first.seg.iP.y - surroundPairsP.front().second.seg.iP.y) / 2.0;
                        tempVec2.y = (surroundPairsP.front().first.seg.iP.y + surroundPairsP.front().second.seg.iP.y) / 2.0;
                        if (intersect(polygonEdges, Segment(tempVec1, tempVec2), tempVec3))
                            tempSegs.push_back(Segment(tempVec1, tempVec3));
                        else
                            tempSegs.push_back(Segment(tempVec1, tempVec2));
                        tempSegs.push_back(Segment(p->v, tempVec1));
                    }
                }
                //for q
                Vector top;
                if (std::abs(surroundPairsQ.back().second.seg.iQ.x - surroundPairsQ.back().first.seg.iQ.x) < EPS)
                {
                    top = surroundPairsQ.back().second.seg.iQ;
                }
                else
                {
                    Vector dir(0.0, 1.0);
                    Line l(q->v, dir);
                    surroundPairsQ.back().second.seg.intersects(l, top);
                }
                tempSeg = surroundPairsQ.back().first.seg;
                if (tempSeg.iQ.x - tempSeg.iP.x > tempSeg.iP.y - tempSeg.iQ.y)
                {
                    tempVec1.x = q->v.x + (q->v.y - top.y) / 2.0;
                    tempVec1.y = (q->v.y + top.y) / 2.0;
                    if (intersect(polygonEdges, Segment(q->v, tempVec1), tempVec2))
                    {
                        tempSegs.push_back(Segment(q->v, tempVec2));
                    }
                    else
                    {
                        tempVec2.x = surroundPairsQ.back().first.seg.iP.x + 
                                    (surroundPairsQ.back().first.seg.iP.y - surroundPairsQ.back().second.seg.iP.y) / 2.0;
                        tempVec2.y = (surroundPairsQ.back().first.seg.iP.y + surroundPairsQ.back().second.seg.iP.y) / 2.0;
                        if (intersect(polygonEdges, Segment(tempVec1, tempVec2), tempVec3))
                            tempSegs.push_back(Segment(tempVec1, tempVec3));
                        else
                            tempSegs.push_back(Segment(tempVec1, tempVec2));
                        tempSegs.push_back(Segment(q->v, tempVec1));
                    }
                }
            }
            //create vertical edgePair and erase prevEdge(if exist), create prevEdge from q
            if (!surroundPairsP.empty())
            {
                //TODO:ray from reflex segment pq
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
                    Vector top;
                    if (std::abs(surroundPairsQ.back().second.seg.iQ.x - surroundPairsQ.back().first.seg.iQ.x) < EPS)
                    {
                        top = surroundPairsQ.back().second.seg.iQ;
                    }
                    else
                    {
                        Vector dir(0.0, 1.0);
                        Line l(q->v, dir);
                        surroundPairsQ.back().second.seg.intersects(l, top);
                    }
                    //for p
                    Vector tempVec1, tempVec2, tempVec3;
                    tempVec1.x = p->v.x + (surroundPairsP.front().first.seg.iQ.y - p->v.y) / 2.0;
                    tempVec1.y = (surroundPairsP.front().first.seg.iQ.y + p->v.y) / 2.0;
                    if (intersect(polygonEdges, Segment(p->v, tempVec1), tempVec2))
                    {
                        tempSegs.push_back(Segment(p->v, tempVec2));
                    }
                    else
                    {
                        tempVec2.x = surroundPairsP.front().first.seg.iQ.x + 
                                    (surroundPairsP.front().first.seg.iQ.y - top.y) / 2.0;
                        tempVec2.y = (surroundPairsP.front().first.seg.iQ.y + top.y) / 2.0;
                        if (intersect(polygonEdges, Segment(tempVec1, tempVec2), tempVec3))
                            tempSegs.push_back(Segment(tempVec1, tempVec3));
                        else
                            tempSegs.push_back(Segment(tempVec1, tempVec2));
                        tempSegs.push_back(Segment(p->v, tempVec1));
                    }
                    //for q
                    tempVec1.x = q->v.x + (q->v.y - top.y) / 2.0;
                    tempVec1.y = (top.y + q->v.y) / 2.0;
                    if (intersect(polygonEdges, Segment(q->v, tempVec1), tempVec2))
                    {
                        tempSegs.push_back(Segment(q->v, tempVec2));
                    }
                    else
                    {
                        tempVec2.x = surroundPairsP.front().first.seg.iQ.x + 
                                    (surroundPairsP.front().first.seg.iQ.y - top.y) / 2.0;
                        tempVec2.y = (surroundPairsP.front().first.seg.iQ.y + top.y) / 2.0;
                        if (intersect(polygonEdges, Segment(tempVec1, tempVec2), tempVec3))
                            tempSegs.push_back(Segment(tempVec1, tempVec3));
                        else
                        {
                            tempVec3.x = surroundPairsP.front().first.seg.iP.x + 
                                    (surroundPairsP.front().first.seg.iP.y - surroundPairsP.front().second.seg.iP.y) / 2.0;
                            tempVec3.y = (surroundPairsP.front().first.seg.iP.y + surroundPairsP.front().second.seg.iP.y) / 2.0;
                            Vector tempVec4;
                            if (intersect(polygonEdges, Segment(tempVec2, tempVec3), tempVec4))
                                tempSegs.push_back(Segment(tempVec2, tempVec4));
                            else
                                tempSegs.push_back(Segment(tempVec2, tempVec3));
                            tempSegs.push_back(Segment(tempVec1, tempVec2));
                        }
                            
                        tempSegs.push_back(Segment(q->v, tempVec1));
                    }
                }
                //ray from bottom segment pq
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
                    Vector top;
                    if (std::abs(surroundPairsQ.front().second.seg.iQ.x - surroundPairsQ.front().first.seg.iQ.x) < EPS)
                    {
                        top = surroundPairsQ.front().second.seg.iQ;
                    }
                    else
                    {
                        Vector dir(0.0, 1.0);
                        Line l(p->v, dir);
                        surroundPairsP.front().second.seg.intersects(l, top);
                    }
                    Segment tempSeg = surroundPairsP.front().first.seg;
                    Vector tempVec1, tempVec2, tempVec3;
                    if (tempSeg.iQ.x - tempSeg.iP.x > tempSeg.iP.y - tempSeg.iQ.y)
                    {
                        tempVec1.x = p->v.x + (p->v.y - top.y) / 2.0;
                        tempVec1.y = (p->v.y + top.y) / 2.0;
                        if (intersect(polygonEdges, Segment(p->v, tempVec1), tempVec2))
                        {
                            tempSegs.push_back(Segment(p->v, tempVec2));
                        }
                        else
                        {
                            tempVec2.x = surroundPairsP.front().first.seg.iP.x + 
                                        (surroundPairsP.front().first.seg.iP.y - surroundPairsP.front().second.seg.iP.y) / 2.0;
                            tempVec2.y = (surroundPairsP.front().first.seg.iP.y + surroundPairsP.front().second.seg.iP.y) / 2.0;
                            if (intersect(polygonEdges, Segment(tempVec1, tempVec2), tempVec3))
                                tempSegs.push_back(Segment(tempVec1, tempVec3));
                            else
                                tempSegs.push_back(Segment(tempVec1, tempVec2));
                            tempSegs.push_back(Segment(p->v, tempVec1));
                        }
                    }
                    if (p != q)
                    {
                        tempVec1.x = q->v.x + (q->v.y - top.y) / 2.0;
                        tempVec1.y = (q->v.y + top.y) / 2.0;
                        if (intersect(polygonEdges, Segment(q->v, tempVec1), tempVec2))
                        {
                            tempSegs.push_back(Segment(q->v, tempVec2));
                        }
                        else
                        {
                            tempVec2.x = p->v.x + (p->v.y - top.y) / 2.0;
                            tempVec2.y = (p->v.y + top.y) / 2.0;
                            if (intersect(polygonEdges, Segment(tempVec1, tempVec2), tempVec3))
                                tempSegs.push_back(Segment(tempVec1, tempVec3));
                            else
                                tempSegs.push_back(Segment(tempVec1, tempVec2));
                            tempSegs.push_back(Segment(q->v, tempVec1));
                        }
                    }
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
                    }
                    if (!surroundPairsP.empty())
                    {
                        edgePairs.push_back(surroundPairsP.front());
                        intersectPairs.erase(surroundPairsP.front());
                        //ray from top segment pq
                        if (p == q)
                        {
                            Segment tempSeg = surroundPairsP.front().first.seg;
                            bool able = (surroundPairsP.front().second.seg.iQ.y - surroundPairsP.front().first.seg.iQ.y > EPS)
                                        || (tempSeg.iQ.x - tempSeg.iP.x > tempSeg.iP.y - tempSeg.iQ.y);
                            if (able)
                            {
                                tempSeg = surroundPairsP.front().second.seg;
                                Vector tempVec1, tempVec2, tempVec3;
                                if (tempSeg.iQ.x - tempSeg.iP.x > tempSeg.iQ.y - tempSeg.iP.y)
                                {
                                    tempVec1.x = surroundPairsP.front().first.seg.iQ.x + 
                                                (surroundPairsP.front().first.seg.iQ.y - surroundPairsP.front().second.seg.iQ.y) / 2.0;
                                    tempVec1.y = (surroundPairsP.front().first.seg.iQ.y + surroundPairsP.front().second.seg.iQ.y) / 2.0;
                                    if (intersect(polygonEdges, Segment(p->v, tempVec1), tempVec2))
                                    {
                                        tempSegs.push_back(Segment(p->v, tempVec2));
                                    }
                                    else
                                    {
                                        tempVec2.x = surroundPairsP.front().first.seg.iP.x + 
                                                    (surroundPairsP.front().first.seg.iP.y - surroundPairsP.front().second.seg.iP.y) / 2.0;
                                        tempVec2.y = (surroundPairsP.front().first.seg.iP.y + surroundPairsP.front().second.seg.iP.y) / 2.0;
                                        if (intersect(polygonEdges, Segment(tempVec1, tempVec2), tempVec3))
                                            tempSegs.push_back(Segment(tempVec1, tempVec3));
                                        else
                                            tempSegs.push_back(Segment(tempVec1, tempVec2));
                                        tempSegs.push_back(Segment(p->v, tempVec1));
                                    }
                                }
                            }
                        }
                        else if (surroundPairsP.empty() || surroundPairsQ.empty())
                        {
                            Segment tempSeg = surroundPairsP.front().second.seg;
                            Vector tempVec1, tempVec2, tempVec3;
                            if (tempSeg.iQ.x - tempSeg.iP.x > tempSeg.iQ.y - tempSeg.iP.y)
                            {
                                tempVec1.x = surroundPairsP.front().first.seg.iQ.x + 
                                            (surroundPairsP.front().first.seg.iQ.y - surroundPairsP.front().second.seg.iQ.y) / 2.0;
                                tempVec1.y = (surroundPairsP.front().first.seg.iQ.y + surroundPairsP.front().second.seg.iQ.y) / 2.0;
                                if (intersect(polygonEdges, Segment(p->v, tempVec1), tempVec2))
                                {
                                    tempSegs.push_back(Segment(p->v, tempVec2));
                                }
                                else
                                {
                                    tempVec2.x = surroundPairsP.front().first.seg.iP.x + 
                                                (surroundPairsP.front().first.seg.iP.y - surroundPairsP.front().second.seg.iP.y) / 2.0;
                                    tempVec2.y = (surroundPairsP.front().first.seg.iP.y + surroundPairsP.front().second.seg.iP.y) / 2.0;
                                    if (intersect(polygonEdges, Segment(tempVec1, tempVec2), tempVec3))
                                        tempSegs.push_back(Segment(tempVec1, tempVec3));
                                    else
                                        tempSegs.push_back(Segment(tempVec1, tempVec2));
                                    tempSegs.push_back(Segment(p->v, tempVec1));
                                }
                            }
                        }
                        else
                        {
                            Segment tempSeg = surroundPairsP.front().second.seg;
                            Vector tempVec1, tempVec2, tempVec3;
                            if (tempSeg.iQ.x - tempSeg.iP.x > tempSeg.iQ.y - tempSeg.iP.y)
                            {
                                tempVec1.x = surroundPairsP.front().first.seg.iQ.x + 
                                            (surroundPairsP.front().first.seg.iQ.y - surroundPairsP.front().second.seg.iQ.y) / 2.0;
                                tempVec1.y = (surroundPairsP.front().first.seg.iQ.y + surroundPairsP.front().second.seg.iQ.y) / 2.0;
                                if (intersect(polygonEdges, Segment(q->v, tempVec1), tempVec2))
                                {
                                    tempSegs.push_back(Segment(q->v, tempVec2));
                                }
                                else
                                {
                                    tempVec2.x = surroundPairsP.front().first.seg.iP.x + 
                                                (surroundPairsP.front().first.seg.iP.y - surroundPairsP.front().second.seg.iP.y) / 2.0;
                                    tempVec2.y = (surroundPairsP.front().first.seg.iP.y + surroundPairsP.front().second.seg.iP.y) / 2.0;
                                    if (intersect(polygonEdges, Segment(tempVec1, tempVec2), tempVec3))
                                        tempSegs.push_back(Segment(tempVec1, tempVec3));
                                    else
                                        tempSegs.push_back(Segment(tempVec1, tempVec2));
                                    tempSegs.push_back(Segment(q->v, tempVec1));
                                }
                            }
                            tempSeg = surroundPairsP.front().first.seg;
                            bool able = (surroundPairsP.front().first.seg.iQ.y - p->v.y > EPS)
                                        || (tempSeg.iQ.x - tempSeg.iP.x > tempSeg.iP.y - tempSeg.iQ.y);
                            if (able)
                            {
                                tempVec1.x = p->v.x + 
                                        (surroundPairsP.front().first.seg.iQ.y - p->v.y) / 2.0;
                                tempVec1.y = (surroundPairsP.front().first.seg.iQ.y + p->v.y) / 2.0;
                                if (intersect(polygonEdges, Segment(p->v, tempVec1), tempVec2))
                                {
                                    tempSegs.push_back(Segment(p->v, tempVec2));
                                }
                                else
                                {
                                    tempVec2.x = surroundPairsP.front().first.seg.iQ.x + 
                                                (surroundPairsP.front().first.seg.iQ.y - surroundPairsP.front().second.seg.iQ.y) / 2.0;
                                    tempVec2.y = (surroundPairsP.front().first.seg.iQ.y + surroundPairsP.front().second.seg.iQ.y) / 2.0;
                                    if (intersect(polygonEdges, Segment(tempVec1, tempVec2), tempVec3))
                                        tempSegs.push_back(Segment(tempVec1, tempVec3));
                                    else
                                        tempSegs.push_back(Segment(tempVec1, tempVec2));
                                    tempSegs.push_back(Segment(p->v, tempVec1));
                                }
                            }
                        }
                    }
                    
                }
                prevEdge = nullptr;
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
        for (auto &&segment : tempSegs)
        {
            segments.emplace_back(segment);
        }
        tempSegs.clear();
        tempPairs.clear(); 
    }
    sort(points.begin(), points.end(), compareIndex);
    return true;
}

ostream& operator<<(ostream& os, const Point& point)
{
    cout << "<<" << "Index:" << point.index << " x:" << point.v.x << " y:" << point.v.y << ">>" << endl;
    return os;
}

ostream& operator<<(ostream& os, const Polygon& polygon)
{
    for (auto &&point : polygon.points)
        cout << point;
    return os;
}