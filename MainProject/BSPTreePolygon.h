#pragma once

#include "basicPolygon.h"


class BSPTreeCreatorNode;
class BSPTreeNode;


///make separate class portal extending polygon?
class BSPTreePolygon : public basicPolygon
{
public:
	BSPTreePolygon(void) : basicPolygon() {
		usedAsDivider = false;
		usedAsPortal = false;

		connectsTo = nullptr;
		connectedId = -1;
	};

	BSPTreePolygon(vec3 point1, vec3 point2, vec3 point3) : basicPolygon(point1, point2, point3) {
		usedAsDivider = false;
		usedAsPortal = false;

		connectsTo = nullptr;
		connectedId = -1;
	};
	BSPTreePolygon(std::vector<vec3> points) : basicPolygon(points){
		usedAsDivider = false;
		usedAsPortal = false;

		connectsTo = nullptr;
		connectedId = -1;
	};
	~BSPTreePolygon(void){};

	

	BSPTreeCreatorNode *ownerNode;				//bsptree creator, pvs comp.
	BSPTreeNode *connectsTo;     //used in bsptree, portal
	int connectedId;					//in bsptreecreator, pvs computation

	bool usedAsDivider;			//bsptree creator, bsp generation
	bool usedAsPortal;			//bsptree creator, portal creation
	


};
