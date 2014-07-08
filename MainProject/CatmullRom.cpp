#include "CatmullRom.h"


/*
The Catmull-Rom splines, also called Overhauser splines, belong to a class of curves known as Hermite splines.
They are uniform rational cubic polynomial curves that interpolate between N control points and pass through 
exactly N-2 control points (except the first and last one). They are uniform, because the control points 
(also known as knots) are spaced at equal intervals with respect to the curve's parameter (t). 
The interpolation is performed in a piecewise manner: a new cubic curve is defined between each pair of points.
*/
CRSpline::CRSpline()
: waypoints(), delta_t(0.0f)
{
}

CRSpline::CRSpline(const CRSpline& s)
{
    for (int i = 0; i < s.waypoints.size(); i++)
        waypoints.push_back(s.waypoints[i]);
    delta_t = s.delta_t;
}

CRSpline::~CRSpline()
{}

/*
Solve the Catmull-Rom parametric equation for a given time(t) and vector quadruple (p1,p2,p3,p4) of control points.
*/
vec3 CRSpline::equation(float t, const vec3 &p1, const vec3 &p2, const vec3 &p3, const vec3 &p4){

    float t2 = t * t;   //t^2
    float t3 = t2 * t;	//t^3

    float b1 = 0.5f * (    -t3 + 2 * t2 - t    );  //this is just T vector multiplied by M matrix
    float b2 = 0.5f * ( 3 * t3 - 5 * t2     + 2);
    float b3 = 0.5f * (-3 * t3 + 4 * t2 + t    );
    float b4 = 0.5f * (     t3 -     t2        );

return (p1 * b1 + p2 * b2 + p3 * b3 + p4 * b4);  //and here multiplied by vector P of points
}


/*
Adds waypoint to the back and readjusts delta_t.
delta_t is 1.0 / waypoints.size()
*/
void CRSpline::addSplinePoint(const vec3& point){

    waypoints.push_back(point);
    delta_t = 1.0f / (waypoints.size() - 1);
}

/*
Safety function to check index into waypoints vector.
*/
int CRSpline::bounds(int index){
		if (index < 0) {
			return 0;
		} else if (index >= waypoints.size() - 1) {
			return waypoints.size() - 1;

		} else return index;
}

/*
time works from 0.0 to 1.0, but it's safe to be out of this interval.
for constant speed presumes linear distribution of points.
*/
vec3 CRSpline::getInterpolatedSplinePoint(float time)
{
    //Find out in which interval we are on the spline
    int index = (int)(time / delta_t);

    //get local control point indices
    int p0 = bounds(index - 1);
    int p1 = bounds(index);
    int p2 = bounds(index + 1);
    int p3 = bounds(index + 2);


	//(current time - exact time at index) = absolute time from index
	//absolute time from index / length of section = relative time within section
	float local_time = (time - delta_t * (float)index) / delta_t;

	//Interpolate
    return CRSpline::equation(local_time, waypoints[p0], waypoints[p1], waypoints[p2], waypoints[p3]);
}


/*
effectively waypoints.size()
*/
int CRSpline::getNumPoints()
{
	return waypoints.size();
}

/*
Returns waipoint at index.
*/
vec3& CRSpline::getNthPoint(int index)
{
	return waypoints[index];
}

/*
Gets delta.
*/
float CRSpline::delta(){

	return delta_t;
}

