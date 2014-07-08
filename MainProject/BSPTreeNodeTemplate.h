#pragma once

#include "basicPolygon.h"

template <class T>
class BSPTreeNodeTemplate
{
public:
	BSPTreeNodeTemplate(void);
	~BSPTreeNodeTemplate(void);

	T *LeftChild;								//pointers to children
	T *RightChild;
	
	float red, green, blue;						//colour
	float xmax, ymax, zmax, xmin, ymin, zmin;   //extremes
	int Id;										//identification

	basicPolygon Divider;						//dividing plane
	std::vector<vec3> vertices;					//geometry of node - for saving or drawing
	
	bool isLeaf(){
		return (LeftChild == nullptr && RightChild == nullptr);
	}
};


template <class T>
BSPTreeNodeTemplate<T>::BSPTreeNodeTemplate(void){

	//some basic initializations
	RightChild = nullptr;			
	LeftChild = nullptr;

	xmax = 0.0f;
	ymax = 0.0f;
	zmax = 0.0f;
	xmin = 0.0f;
	ymin = 0.0f;
	zmin = 0.0f;

	red = 0.0;
	green = 0.0;
	blue = 0.0;

	Id = -1;
}

template <class T>
BSPTreeNodeTemplate<T>::~BSPTreeNodeTemplate(void)
{
}

