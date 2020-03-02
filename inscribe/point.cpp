#include "point.h"

Point::Point(int _index, Vector _v)
{
    index = _index;
    v = _v;
}

Point::Point()
{
    index = -1;
    v = Vector(0.0, 0.0);
}

bool Point::operator<(const Point rhs) const
{
    if (ipe::abs(this->v.x-rhs.v.x) < EPS)
    {
        return this->v.y < rhs.v.y;
    }
    else
    {
        return this->v.x < rhs.v.x;
    }
}

bool Point::operator==(const Point rhs) const
{
    return this->v == rhs.v;
}

inline bool compareIndex(Point const a, Point const b)
{
    return a.index < b.index;
}

//for debugging
void printPair(vector<PPair> PointPairs)
{
	for (size_t i = 0; i < PointPairs.size(); i++)	
	{
		cout << "pair" << i << endl;
		cout << "First index:"  << PointPairs[i].first.index << endl;
		cout << "First vector:"  << PointPairs[i].first.v.x << " " << PointPairs[i].first.v.y << endl;
		cout << "Second index:"  << PointPairs[i].second.index << endl;
		cout << "Second vector:"  << PointPairs[i].second.v.x << " " << PointPairs[i].second.v.y << endl;
	}
}