// Ipelet for finding maximal triangle which inscribed in a simple polygon

#include "ipelet.h"
#include "ipepath.h"
#include "ipepage.h"
#include "ipereference.h"
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
	vector<Vector> triangle;
	vector<Vector> polygon;
};

void setVectors(vector<Vector> &polygon, Curve const *curve)
{
	polygon.clear();
	polygon.push_back(curve->segment(0).cp(0));
	for (int i = 0; i < curve->countSegments(); i++)
	{
		polygon.push_back(curve->segment(i).last());
	}
}


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
					helper->message("Triangle is already exist");
					return false;
				}
				setVectors(triangle, curve);
				getTriangle = true;
			}
			else if (curve->countSegments() > 2)
			{
				if (getPolygon)
				{
					helper->message("Polygon is already exist");
					return false;
				}
				setVectors(polygon, curve);
				getPolygon = true;
			}
		}
	}
	//matrix of linear transformation from right isosceles triangle to input triangle
	Linear m(triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y,
			triangle[2].x - triangle[0].x, triangle[2].y - triangle[0].y);
	//linear transformation of the polygon
	for (int i = 0; i < polygon.size(); i++)
		polygon[i] = m.inverse() * polygon[i];
	

	return true;
}

//do not change this function
IPELET_DECLARE Ipelet *newIpelet()
{
  return new InscribeIpelet;
}