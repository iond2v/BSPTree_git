#include "basicPolygon.h"


/*
Returns length of vector.
*/
float length(vec3 vector){

	return sqrtf(dot(vector, vector));
}

/*
Does just dot(vector, vector) to save sqrt operation.
Useful when the exact length is not needed e.g. comparison of length of 2 vectors.
*/
float cheap_length(vec3 vector){

	return dot(vector, vector);
}


basicPolygon::basicPolygon(void){

	p.insert(p.end(), vec3(0.0f, 0.0f, 0.0f));
	p.insert(p.end(), vec3(0.0f, 0.0f, 0.0f));
	p.insert(p.end(), vec3(0.0f, 0.0f, 0.0f));

	normal = vec3(0.0f, 0.0f, 0.0f);

}


basicPolygon::basicPolygon(vec3 point1, vec3 point2, vec3 point3){

		p.insert(p.end(), point1);
		p.insert(p.end(), point2);
		p.insert(p.end(), point3);

	computeNormal();

}


basicPolygon::basicPolygon(std::vector<vec3> points){

	p = points;
	computeNormal();
	
}


basicPolygon::~basicPolygon(){

}


void basicPolygon::computeNormal(){
	normal = cross(p[2] - p[0], p[1] - p[0]);
	
}


bool basicPolygon::isTiny(){

	float maxDifferenceX = 0.0f;
	float maxDifferenceY = 0.0f;
	float maxDifferenceZ = 0.0f;


	for(unsigned int i = 0; i < p.size(); i++){
		int next_index = (i + 1) % p.size();

		maxDifferenceX = max(abs(p[i].x - p[next_index].x), maxDifferenceX);
		maxDifferenceY = max(abs(p[i].y - p[next_index].y), maxDifferenceY);
		maxDifferenceZ = max(abs(p[i].z - p[next_index].z), maxDifferenceZ);
	
	}
	
	if((maxDifferenceX <= 0.05f && maxDifferenceY <= 0.05f) || 
		(maxDifferenceX <= 0.05f && maxDifferenceZ <= 0.05f) || 
		 (maxDifferenceZ <= 0.05f && maxDifferenceY <= 0.05f))
			return true;


	return false;	 

}

/*
POLYGON-INFRONT
Indata:
Polygon1 – The polygon to determine whether the other polygon is
in front of or not
Polygon2 – The polygon to check if it is in front of the first
polygon or not
Outdata:
Whether the second is in front of the first polygon or not.
Effect:
Checks each point in the second polygon is in front of
the first polygon. If so is the case it is considered
to be in the front of it.

POLYGON-INFRONT (Polygon1, Polygon2)
1 for each point p in Polygon2
2 if (CLASSIFY-POINT (Polygon1, p) <> INFRONT)
3 then return false
4 return true

true for infront or coinciding
if one point is behind return false
polygon - polygon to check
*/

bool basicPolygon::polygonInfront(const basicPolygon &polygon){

	//if one point is behind, polygon is not in front
	for(unsigned int i = 0; i < 3; i++){

		if(this->classifyPoint(polygon.p[i]) == BEHIND)
			return false;
	}

return true;
}

// true for behind or coinciding
// if one point is infront return false
//polygon - polygon to check

bool basicPolygon::polygonBehind(const basicPolygon &polygon){

	for(unsigned int i = 0; i < 3; i++){

		if(this->classifyPoint(polygon.p[i]) == INFRONT)
			return false;
	}

return true;
}

/*
CALCULATE-SIDE
Indata :
Polygon1 – The polygon to classify the other polygon against
Polygon2 – The polygon to classify
Outdata :
Which side of polygon1 polygon 2 is located on.
Effect:
Classifies each point in the second polygon versus the
first polygon. If there are points on the positive side but no
points on the negative side, Polygon2 is considered to be in front
of Polygon1. If there are points on the negative side but no
points on the positive side, Polygon2 is considered to be behind
Polygon1. If all points are coinciding polygon2 is considered to
be coinciding with Polygon1. The last possible case is that there
are points on both the positive and the negative side, then
polygon2 is considered to be spanning Polygon1.


CALCULATE-SIDE (Polygon1, Polygon2)
1 NumPositive f 0, NumNegative f 0
2 for each point p in Polygon2
3 if (CLASSIFY-POINT (Polygon1, p) = INFRONT)
4 then NumPositive = NumPositive + 1
5 if (CLASSIFY-POINT (Polygon1, p) = BEHIND)
6 then NumNegative = NumNegative + 1
7 if (NumPositive > 0 && NumNegative = 0)
8 then return INFRONT
9 else if(NumPositive = 0 && NumNegative > 0)
10 then return BEHIND
11 else if(NumPositive = 0 && NumNegative = 0)
12 then return COINCIDING
13 else return SPANNING


polygon1 - polygon to check against
polygon2 - polygon to check
returns INFRONT, BEHIND, COINCIDING, SPANNING
*/

int basicPolygon::calculateSide(const basicPolygon &polygon){

	int positive = 0, negative = 0;
	
	for(unsigned int i = 0; i < polygon.p.size(); i++){
		if (this->classifyPoint(polygon.p[i]) == INFRONT)
			positive++;

		if (this->classifyPoint(polygon.p[i]) == BEHIND)
			negative++;
	}


	if(positive > 0 && negative == 0){
		return INFRONT;
	} else if (positive == 0 && negative > 0){
		return BEHIND;
	} else if (positive == 0 && negative == 0){
		return COINCIDING;
	} else return SPANNING;

}


/*		
vec3 vector = pointB - pointA;
float distance_fraction = - dividingPolygon.planePointDistance(pointA) / dot(dividingPolygon.normal, vector);
return pointA + (vector * distance_fraction);
*/

vec3 basicPolygon::planeVectorIntersection(const vec3 &pointA, const vec3 &pointB){
		vec3 vector = pointB - pointA;

		float A = - this->planePointDistance(pointA);
		float B = dot(normalize(this->normal), vector);


		float distance_fraction = A / B;
		return pointA + (vector * distance_fraction);
}


/*
Splits polygon by plane dividingPolygon.
Returns points in supplied front and back vectors.
from ftp://ftp.sgi.com/other/bspfaq/faq/bspfaq.html#8.txt

Goes around this polygon and checks if the points of its line segments are
on the same side of dividing polygon. If not, intersection is computed.
All points are put to either infront or behind vectors. Done.
*/

void basicPolygon::splitPolygon(basicPolygon &dividingPolygon, std::vector<vec3> &front, std::vector<vec3> &back) {

	uint count = this->p.size();

	vec3 ptA, ptB;
	int sideA, sideB;

	//start with edge defined by last and first point
	ptA = this->p[count - 1];	
	sideA = dividingPolygon.classifyPoint(ptA);

   for (uint i = 0; i < count; i++){

      ptB = this->p[i];
	  sideB = dividingPolygon.classifyPoint(ptB);  


	  if (sideA != sideB && sideA != COINCIDING && sideB != COINCIDING) {

		vec3 intersectionPoint = dividingPolygon.planeVectorIntersection(ptA, ptB);

		if(sideA == INFRONT){

			front.insert(front.end(), intersectionPoint );
			back.insert(back.end(),  intersectionPoint );
			back.insert(back.end(), ptB );

		} 
		
		if (sideA == BEHIND) {

			front.insert(front.end(), intersectionPoint );
            front.insert(front.end(), ptB );
			
			back.insert(back.end(),  intersectionPoint );
         }

      } else {

         if (sideA == INFRONT || sideB == INFRONT) {
			 			
			front.insert(front.end(), ptB );
         }

		 if(sideA == BEHIND || sideB == BEHIND){
		 		 
			back.insert(back.end(), ptB );
		 
		 }

		 if(sideA == COINCIDING && sideB == COINCIDING){
			front.insert(front.end(), ptB );
			front.insert(front.end(), ptB );
		 }
	  }
	  //move to next edge in polygon - efectively travels around polygon
      ptA = ptB;
      sideA = sideB;

   }

	
}



/*
CLASSIFY-POINT
Indata:
basicPolygon – The polygon to classify the point versus.
Point - 3D-point to classify versus the plane defined
by the polygon.
Outdata:
Which side the point is of the polygon.
Effect:
Determines on which side of the plane defined by the polygon the
point is located.

CLASSIFY-POINT (basicPolygon, Point)
1 SideValue f basicPolygon.Normal * Point
2 if (SideValue = basicPolygon.Distance)
3 then return COINCIDING
4 else if (SideValue < basicPolygon.Distance)
5 then return BEHIND
6 else return INFRONT

The dot product therefore has the geometric interpretation as the length of the projection of  onto the unit vector  when the two vectors are placed so that their tails coincide.
This is because a*b = 0 implies the angle between a and b is 90 degrees. a*b > 0 implies the angle is less than 90 degrees. a*b < 0 implies it is greater than 90 degrees
Returns INFRONT, BEHIND, COINCIDING
*/

int basicPolygon::classifyPoint(const vec3 point){

	float SideValue = this->planePointDistance(point); //signed distance
	
	if (SideValue >= -0.01f && SideValue <= 0.01f){ //small tolerance
		return COINCIDING;

	} else if (SideValue < 0.01f){	
		return BEHIND;

	} else return INFRONT;

}

/*
return dot(normalize(this->normal), point - this->p[0]).
*/
float basicPolygon::planePointDistance(const vec3 point){

return dot(normalize(this->normal), point - this->p[0]);
}


/*
    P = A + u * (C - A) + v * (B - A)       // Original equation
    (P - A) = u * (C - A) + v * (B - A)     // Subtract A from both sides
    v2 = u * v0 + v * v1                    // Substitute v0, v1, v2 for less writing
    
    // We have two unknowns (u and v) so we need two equations to solve
    // for them.  Dot both sides by v0 to get one and dot both sides by
    // v1 to get a second.
    (v2) . v0 = (u * v0 + v * v1) . v0
    (v2) . v1 = (u * v0 + v * v1) . v1

    // Distribute v0 and v1
    v2 . v0 = u * (v0 . v0) + v * (v1 . v0)
    v2 . v1 = u * (v0 . v1) + v * (v1 . v1)

    // Now we have two equations and two unknowns and can solve one 
    // equation for one variable and substitute into the other.  Or
    // if you're lazy like me, fire up Mathematica and save yourself
    // some handwriting.
    Solve[v2.v0 == {u(v0.v0) + v(v1.v0), v2.v1 == u(v0.v1) + v(v1.v1)}, {u, v}]
    u = ((v1.v1)(v2.v0)-(v1.v0)(v2.v1)) / ((v0.v0)(v1.v1) - (v0.v1)(v1.v0))
    v = ((v0.v0)(v2.v1)-(v0.v1)(v2.v0)) / ((v0.v0)(v1.v1) - (v0.v1)(v1.v0))
http://www.blackpawn.com/texts/pointinpoly/default. 

basicPolygon must be convex.
Also this is 2D.. presumes the point is known to be on the plane... ???????? no? do a manual check..
*/

bool basicPolygon::isPointInPolygon(const vec3 pt){

	bool in = false;

	//for pentagon its 3 triangles, for triangle its one
	for(unsigned int i = 0; i < this->p.size() - 2; i++){

		vec3 v0 = p[i + 2] - p[0];
		vec3 v1 = p[i + 1] - p[0];
		vec3 v2 = pt - p[0];

		float dot00 = dot(v0, v0);
		float dot01 = dot(v0, v1);
		float dot02 = dot(v0, v2);
		float dot11 = dot(v1, v1);
		float dot12 = dot(v1, v2);

		float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;


	// Check if point is in triangle number one or two etc.
	in |= (u >= 0.0f) && (v >= 0.0f) && (u + v < 1.0f);
	}

return in;
}





/*
creates points distributed inside polygon.
|\
|* \
|* * \
|* * * \

*/

void basicPolygon::getPoints(std::vector<vec3> &points){

	float interval = 0.4f; //can possibly move to function arguments.. shorter interval => more points => geometrically more checks when not immediately visible
							//0.4f makes 4 points on 1x1 square

	//for pentagon its 3 triangles, for triangle its one
	for(unsigned int i = 0; i < p.size() - 2; i++){
		

		vec3 v0 = p[i + 2] - p[0];
		vec3 v1 = p[i + 1] - p[0];

		
		float repetitions = length(v0) / interval;
		vec3 increment = v0 / repetitions;
		vec3 temp = p[0];


		std::vector<vec3> seeds;

		//create row of seeds along one side of triangle
		for(unsigned int j = 0; j < repetitions - 1; j++)
			seeds.push_back(temp = temp + increment);
		
		//points.insert(points.end(), seeds.begin(), seeds.end()); keep away from edges!!

		//now create increment in direction of the other vector
		repetitions =  length(v1) / interval;
		increment = v1 / repetitions;
		

		//generate point net from seeds
		//generates double the required points... half is outside triangle..

		for(unsigned int j = 0; j < seeds.size(); j++){
			for(unsigned int k = 0; k < repetitions; k++){
			
				seeds[j] = seeds[j] + increment;

				if(isPointInPolygon(seeds[j])){			//half is outside + if once outside, then rest will be too.
					points.push_back(seeds[j]);
				} else break;

			}
		}



	} //for polygon triangles

}

/*
Does line - triangle intersection but checks whether the intersection is further than the length of the line.
*/

bool basicPolygon::isHit(vec3 start, vec3 end){


	//make small vector in direction of ray
	//vec3 direction = normalize(end - start) / 4.0f;

	
	vec3 intersectionPoint = this->planeVectorIntersection(start, end); //goes both directions

	//test if intersection point is in triangle
	if(this->isPointInPolygon(intersectionPoint)){
		

		int startSide = this->classifyPoint(start);
		int endSide = this->classifyPoint(end);

		//vector is on the same side and is not coinciding - check if it isnt going in opposite direction
		if(startSide == endSide && startSide != COINCIDING && endSide != COINCIDING){

			float startDistance = this->planePointDistance(start);
			float endDistance = this->planePointDistance(end);

			if(abs(endDistance) > abs(startDistance)) //goes away
				return false;
		}


		//vector intersects something - is it further than end?
		if(length(end - start) < length(intersectionPoint - start))
			return false;
		

	return true;
	}
		

return false;	
}



/*
Function to check if two polygons touch edges to merge them later. Along X or Z axes only.
Doesnt take crossing or arbitrary direction into consideration.
*/

bool basicPolygon::touch(basicPolygon *first, basicPolygon *second, float epsilon){

	//some existing point values
	float xmax1 = first->p[0].x;
	float ymax1 = first->p[0].y;
	float zmax1 = first->p[0].z;

	float xmin1 = first->p[0].x;
	float ymin1 = first->p[0].y;
	float zmin1 = first->p[0].z;

	float xmax2 = second->p[0].x;
	float ymax2 = second->p[0].y;
	float zmax2 = second->p[0].z;

	float xmin2 = second->p[0].x;
	float ymin2 = second->p[0].y;
	float zmin2 = second->p[0].z;


	for(uint j = 0; j < min(first->p.size(), second->p.size()); j++){


		xmax1 = max(first->p[j].x, xmax1);
		ymax1 = max(first->p[j].y, ymax1);
		zmax1 = max(first->p[j].z, zmax1);

		xmin1 = min(first->p[j].x, xmin1);
		ymin1 = min(first->p[j].y, ymin1);
		zmin1 = min(first->p[j].z, zmin1);

		xmax2 = max(second->p[j].x, xmax2);
		ymax2 = max(second->p[j].y, ymax2);
		zmax2 = max(second->p[j].z, zmax2);

		xmin2 = min(second->p[j].x, xmin2);
		ymin2 = min(second->p[j].y, ymin2);
		zmin2 = min(second->p[j].z, zmin2);

	}
	

	//works when along X or Z axis
	if(zmin1 + epsilon >= zmax2 &&  zmin1 - epsilon <= zmax2 && zmin2 + epsilon >= zmax1 && zmin2 - epsilon <= zmax1){  //same plane Z
					 
		if((xmax1 + epsilon >= xmin2 && xmax1 - epsilon <= xmin2 && xmin1 < xmin2)  || (xmin1 - epsilon <= xmax2 && xmin1 + epsilon >= xmax2 && xmax1 > xmax2))
			return true;
					
	} else if(xmin1 + epsilon >= xmax2 && xmin1 - epsilon <= xmax2 && xmin2 + epsilon >= xmax1 && xmin2 - epsilon <= xmax1){   //same plane X
								
		if((zmax1 + epsilon >= zmin2 && zmax1 - epsilon <= zmin2 && zmin1 < zmin2) || (zmin1 - epsilon <= zmax2 && zmin1 + epsilon >= zmax2 && zmax1 > zmax2))
			return true;
	}

return false;
}




/*
Function to check if two polygons overlap. Along X or Z axes only.
Doesnt take crossing or arbitrary direction into consideration.
*/

bool basicPolygon::shareSpace(basicPolygon *first, basicPolygon *second, float epsilon){

	//more general...
	//check if coinciding, then if edge vectors cross...?


	//some existing point values
	float xmax1 = first->p[0].x;
	float ymax1 = first->p[0].y;
	float zmax1 = first->p[0].z;

	float xmin1 = first->p[0].x;
	float ymin1 = first->p[0].y;
	float zmin1 = first->p[0].z;

	float xmax2 = second->p[0].x;
	float ymax2 = second->p[0].y;
	float zmax2 = second->p[0].z;

	float xmin2 = second->p[0].x;
	float ymin2 = second->p[0].y;
	float zmin2 = second->p[0].z;


	for(uint j = 0; j < min(first->p.size(), second->p.size()); j++){


		xmax1 = max(first->p[j].x, xmax1);
		ymax1 = max(first->p[j].y, ymax1);
		zmax1 = max(first->p[j].z, zmax1);

		xmin1 = min(first->p[j].x, xmin1);
		ymin1 = min(first->p[j].y, ymin1);
		zmin1 = min(first->p[j].z, zmin1);

		xmax2 = max(second->p[j].x, xmax2);
		ymax2 = max(second->p[j].y, ymax2);
		zmax2 = max(second->p[j].z, zmax2);

		xmin2 = min(second->p[j].x, xmin2);
		ymin2 = min(second->p[j].y, ymin2);
		zmin2 = min(second->p[j].z, zmin2);

	}
	

	
	//works when along X or Z axis and when one is fully inside other
	if(zmin1 + epsilon >= zmax2 && zmin1 - epsilon <= zmax2 && zmin2 + epsilon >= zmax1 && zmin2 - epsilon <= zmax1){  //same plane Z
					 
		if((xmax1 + epsilon >= xmax2 && xmin1 - epsilon <= xmin2) || (xmax2 + epsilon >= xmax1 && xmin2 - epsilon <= xmin1))
			return true;
					
	} else if(xmin1 + epsilon >= xmax2 && xmin1 - epsilon <= xmax2 && xmin2 + epsilon >= xmax1 && xmin2 - epsilon <= xmax1){   //same plane X
								
		if((zmax1 + epsilon >= zmax2 && zmin1 - epsilon <= zmin2) || (zmax2 + epsilon >= zmax1 && zmin2 - epsilon <= zmin1))
			return true;
	}

return false;
}
