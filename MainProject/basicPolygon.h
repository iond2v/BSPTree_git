#pragma once

#include <glm/glm.hpp> //vec3
#include <vector>

using namespace glm;


#define INFRONT 1
#define BEHIND 2
#define COINCIDING 3
#define SPANNING 4

#define NOPOLYGON -1


class basicPolygon {
public:

	basicPolygon(void);
	basicPolygon(vec3 point1, vec3 point2, vec3 point3);
	basicPolygon(std::vector<vec3> points);
	~basicPolygon(void);

	std::vector<vec3> p;							//vertices of polygon
	vec3 normal;
	
	void computeNormal();

	void getPoints(std::vector<vec3> &points);
	bool isHit(vec3 start, vec3 end);
	bool isPointInPolygon(const vec3 pt);
	int classifyPoint(const vec3 point);
	bool polygonInfront(const basicPolygon &polygon);
	bool polygonBehind(const basicPolygon &polygon);
	int calculateSide(const basicPolygon &polygon);
	float planePointDistance(const vec3 point);
	vec3 planeVectorIntersection(const vec3 &pointA, const vec3 &pointB);
	void splitPolygon(basicPolygon &dividingPolygon, std::vector<vec3> &front, std::vector<vec3> &back);
	bool shareSpace(basicPolygon *first, basicPolygon *second, float epsilon = 0.01f);
	bool touch(basicPolygon *first, basicPolygon *second, float epsilon = 0.01f);
	bool isTiny();

};



