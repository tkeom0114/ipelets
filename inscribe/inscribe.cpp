// Ipelet for finding maximal triangle which inscribed in a simple polygon

#include "ipelet.h"
#include "ipepath.h"
#include "ipepage.h"
#include "ipereference.h"
#include "point.h"
#include "libs.h"
#include "cut.h"
#include "polygon.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <set>
#include <queue>

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
	Polygon polygon;
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
	int numpol=page->count();
	if(numpol < 2)
	{
		helper->message("Select both polygon and triangle");
		return false;
	}
	else if (numpol > 2)
	{
		helper->message("Too much polygons are selected");
		return false;
	}
	
	bool getTriangle = false;
	bool getPolygon = false;
	//copy polygon and triangle
	for (int i = 0;i < numpol;i++)
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
				polygon.setPoints(curve);
				getPolygon = true;
			}
		}
	}
	//matrix of linear transformation from right isosceles triangle to input triangle
	Linear m(triangle[1].v.x - triangle[0].v.x, triangle[1].v.y - triangle[0].v.y,
			triangle[2].v.x - triangle[0].v.x, triangle[2].v.y - triangle[0].v.y);
	//linear transformation of the polygon
	//polygon.transformPoints(m);

	if (!polygon.computeVis(helper)) return false;
	int count = 0;
	for (auto &&poly : polygon.divide(DIR::VER))
	{
		cout << "Polygon " << count << endl;
		cout << poly << endl;
		count++;
	}
	cout << "Input polygon" << endl;
	cout << polygon << endl;
	//debugging
	/*for (size_t i = 0; i < polygon.sliceLines.size(); i++)
	{
		Curve *sp=new Curve;
		sp->appendSegment(polygon.sliceLines[i].first.v, polygon.sliceLines[i].second.v);
		sp->setClosed(false);
		Shape shape;
		shape.appendSubPath(sp);
		Path *obj = new Path(data->iAttributes, shape);
		page->append(ESecondarySelected, data->iLayer, obj);
	}*/
	for (size_t i = 0; i < polygon.points.size(); i++)
	{
		for (size_t dir = 0; dir < 3; dir++)
		{
			Curve *sp=new Curve;
			sp->appendSegment(polygon.points[i].visible[dir].first.v, polygon.points[i].visible[dir].second.v);
			sp->setClosed(false);
			Shape shape;
			shape.appendSubPath(sp);
			Path *obj = new Path(data->iAttributes, shape);
			page->append(ESecondarySelected, data->iLayer, obj);
		}
	}
	/*for (size_t i = 0; i < polygon.edgePairs.size(); i++)
	{
		Curve *sp=new Curve;
		sp->appendSegment(polygon.edgePairs[i].first.seg.iQ, polygon.edgePairs[i].second.seg.iQ);
		sp->setClosed(false);
		Shape shape;
		shape.appendSubPath(sp);
		Path *obj = new Path(data->iAttributes, shape);
		page->append(ESecondarySelected, data->iLayer, obj);
	}*/
	return true;
}
//do not change this function
IPELET_DECLARE Ipelet *newIpelet()
{
  return new InscribeIpelet;
}