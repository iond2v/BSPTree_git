#pragma once

#include <vector>
#include<glm/glm.hpp>

//source adapted from http://www.codeproject.com/Articles/30838/Overhauser-Catmull-Rom-Splines-for-Camera-Animatio - CPOL

using namespace glm;

class CRSpline
{
public:

	CRSpline();
    CRSpline(const CRSpline&);
    ~CRSpline();


    void addSplinePoint(const vec3 &point);
	vec3 getInterpolatedSplinePoint(float time);   // t = 0...1; 0=vp[0] ... 1=vp[max]
	int getNumPoints();
	vec3 &getNthPoint(int n);
	int bounds(int index);
	float delta();

    // Static method for computing the Catmull-Rom parametric equation
    // given a time (t) and a vector quadruple (p1,p2,p3,p4).
    static vec3 equation(float t, const vec3 &p1, const vec3 &p2, const vec3 &p3, const vec3 &p4);

private:
    std::vector<vec3> waypoints;
    float delta_t;
};

