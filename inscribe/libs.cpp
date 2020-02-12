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