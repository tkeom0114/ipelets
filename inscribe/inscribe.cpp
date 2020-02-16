// Ipelet for finding maximal triangle which inscribed in a simple polygon

#include "ipelet.h"
#include "ipepath.h"
#include "ipepage.h"
#include "ipereference.h"
#include "libs.h"
#include "point.h"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace ipe;
using namespace std;

/*main class for ipelet*/
class InscribeIpelet : public Ipelet {
public:
/*	
 *	2 public functions for ipelet
 *	need not to modify
 */
	virtual int ipelibVersion() const {return IPELIB_VERSION;}
	virtual bool run(int, IpeletData *data, IpeletHelper *helper);
//user defined functions for graham algorithm
//you can use independent class for algorithm
private:
	vector<Point> triangle;
	vector<Point> polygon;
};


bool InscribeIpelet::run(int, IpeletData *data, IpeletHelper *helper)
{
	Page *page = data->iPage;
	int sel = page->primarySelection();
	if(sel<0)
	{
		helper->message("No selection");
		return false;
	}
	int n=page->count();
	if(n < 2)
	{
		helper->message("Select both polygon and triangle");
		return false;
	}
	else if (n > 2)
	{
		helper->message("Too much polygons are selected");
		return false;
	}
	
	bool getTriangle = false;
	bool getPolygon = false;
	//copy polygon and triangle
	for (int i=0;i<n;i++)
	{
		if (page->object(i)->type() == Object::EPath) 
		{
			Curve const *curve = page->object(i)->asPath()->shape().subPath(0)->asCurve();
			if (curve->countSegments() == 2)
			{
				if (getTriangle)
				{
					helper->message("Triangle is already selected");
					return false;
				}
				setVectors(triangle, curve);
				getTriangle = true;
			}
			else if (curve->countSegments() > 2)
			{
				if (getPolygon)
				{
					helper->message("Polygon is already selected");
					return false;
				}
				setVectors(polygon, curve);
				getPolygon = true;
			}
		}
	}
	//matrix of linear transformation from right isosceles triangle to input triangle
	Linear m(triangle[1].v.x - triangle[0].v.x, triangle[1].v.y - triangle[0].v.y,
			triangle[2].v.x - triangle[0].v.x, triangle[2].v.y - triangle[0].v.y);
	//linear transformation of the polygon
	for (size_t i = 0; i < polygon.size(); i++)
		polygon[i].v = m.inverse() * polygon[i].v;
	
	
	return true;
}

//do not change this function
IPELET_DECLARE Ipelet *newIpelet()
{
  return new InscribeIpelet;
}