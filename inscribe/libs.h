#include "ipelet.h"
#include "ipepath.h"
#include "ipepage.h"
#include "ipereference.h"
#include "point.h"
#include <vector>
#include <algorithm>

using namespace ipe;
using namespace std;

void setVectors(vector<Point> &polygon, Curve const *curve);