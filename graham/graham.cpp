// Ipelet for finding convex hull using graham scan

#include "ipelet.h"
#include "ipepath.h"
#include "ipepage.h"
#include "ipereference.h"
#include <vector>
#include <algorithm>

using namespace ipe;
using namespace std;

/*main class for ipelet*/
class ConvexIpelet : public Ipelet {
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
	struct comp{
		comp(const ConvexIpelet& info):m_info(info){}
		const ConvexIpelet& m_info;

		bool operator () (Vector v1,Vector v2)
		{
			Vector p1(v1.x-m_info.lower.x,v1.y-m_info.lower.y);
			Vector p2(v2.x-m_info.lower.x,v2.y-m_info.lower.y);
			return p1.angle()<p2.angle();
		}
	};
	Angle ccw(Vector v1,Vector v2,Vector v3);
	Vector lower;
	vector <Vector> pts;
};

Angle ConvexIpelet::ccw(Vector v1,Vector v2,Vector v3){
	Vector p31(v3.x-v1.x,v3.y-v1.y);
	Vector p21(v2.x-v1.x,v2.y-v1.y);
	if (p31.angle()*p21.angle()<0)
		return p21.angle()-p31.angle(); 
	else return p31.angle()-p21.angle();
}

bool ConvexIpelet::run(int, IpeletData *data, IpeletHelper *helper)
{
	Page *page = data->iPage;
	int sel = page->primarySelection();
	if(sel<0){
		helper->message("No selection");
		return false;
	}
	int n=page->count();
	//copy mark(points)
	for(int i=0;i<n;i++){
		if(page->object(i)->type()!=Object::EReference) return false;
		Reference *ref=page->object(i)->asReference();
		pts.push_back(ref->position());
	}

//Start of the algorithm
	//find botton point
	lower=pts[0];
	int tmp=0;
	for(int i=1;i<n;i++)
		if(pts[i].y<lower.y){
			tmp=i;
			lower=pts[tmp];
		}
	pts[tmp]=pts[0];
	pts[0]=lower;
	//sort points by angle with pts[0] 
	sort(pts.begin(),pts.end(),comp(*this));
	//
	pts.push_back(lower);
	int m=1;
	for(int i=2;i<=n;i++){
		while(ccw(pts[m-1],pts[m],pts[i])<=0){
			if(m>1)
				m--;
			else if (i==n)
				break;
			else i++;
		}
		m++;
		//Vector swap=pts[m];
		pts[m]=pts[i];
		//pts[i]=swap;
	}

//drawing
	//make lines
	Curve *sp=new Curve;
	for(int i=0;i<m;i++){
		sp->appendSegment(pts[i],pts[i+1]);
	}
	sp->setClosed(true);
	Shape shape;
	shape.appendSubPath(sp);
	Path *obj = new Path(data->iAttributes, shape);
    page->append(ESecondarySelected, data->iLayer, obj);
	helper->message("Created Convex hull");
	return true;
}

//do not change this function
IPELET_DECLARE Ipelet *newIpelet()
{
  return new ConvexIpelet;
}