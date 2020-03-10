#include "libs.h"

void setVectors(vector<Point> &polygon, Curve const *curve)
{
	polygon.clear();
    Point p(0,curve->segment(0).cp(0));
	polygon.push_back(p);
	for (int i = 0; i < curve->countSegments(); i++)
	{
        p.index = i + 1; p.v = curve->segment(i).last();
		polygon.push_back(p);
	}
}

inline double triArea(Vector a, Vector b, Vector c)
{
	return (b.x-a.x) * (c.y-a.y) - (c.x-a.x) * (b.y-a.y);
}

double area(vector<Vector> const &polygon)
{
    if (polygon.empty())
    {
        return 0.0;
    }
    double area = 0.0;
    for (size_t i = 1; i < polygon.size() - 1; i++)
    {
        area += triArea(polygon[0], polygon[i], polygon[i+1]);
    }
    return ipe::abs(area);
}

double cross(const Vector &a, const Vector &b)
{
    return a.x*b.y - a.y*b.x;
}