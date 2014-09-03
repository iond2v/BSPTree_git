#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sstream>

#include <iomanip> //setw
#include "keyStates.h"


//#include "BSPTree.h"
#include "time_item.h"
#include "BSPTreeNodeTemplate.h"
#include "Control.h"
#include "BSPTreePolygon.h"

#include <Shlwapi.h>  //if PathFileExists

using namespace glm;



class BSPTreeCreatorNode;


#define SIDE1 true
#define SIDE2 false

//not sure yet/////////////
//bool splitOnHoles(BSPTreePolygon *portalPolygon, BSPTreeCreatorNode *node);  //node

//BSPTreePolygon *removeCoincidingParts(BSPTreePolygon *&portalPolygon, std::vector<BSPTreePolygon> &PolygonSet);  //node
bool sideOfLine(vec2 start, vec2 end, vec2 point);



/*
All the work happens in constructor. Creates BSP tree, computes PVS and saves it to file.
*/
class BSPTreeCreator {

public:

	BSPTreeCreator(std::vector<GLfloat> *vertexArray, std::string mazeName);
	~BSPTreeCreator(void);

	int node_count;
	std::string name;

	BSPTreeCreatorNode *RootNode;												//The root node of the tree.
	std::vector<BSPTreeCreatorNode*> NodeSet;									//sorted list to all Nodes by Id
	std::vector<BSPTreePolygon*> PortalSet;								//list of all valid portals for easy access when figuring out neighbours
	std::vector<BSPTreePolygon*> PortalQueue;							//queue for elimination of recursion when creating portals

	std::vector<time_item> time_points;									//for measuring length of singular phases of creation of this object.

	

	int addMe(BSPTreeCreatorNode *node);											//adds node to NodeSet
	
private:
//	int threads;    //how to speed it up?
	static const std::string save_path;
	void generateBSPTree(BSPTreeCreatorNode *Node);
	void placePortals();	//takes care of all portal things

	void computePVS();
	bool savePVS();

	void placePortal(BSPTreeCreatorNode *node, BSPTreePolygon *portalPolygon);	//recursive portal placement
	void mergeOverlappingPortals();


	bool areNodesVisible(BSPTreeCreatorNode *current_leaf, BSPTreeCreatorNode *potentialNeighbour);
	void addNeighbours(std::vector<int> *potentialNeighbours, std::vector<int> *IdSet);  //helper for computePVS
	void addNeighbour(std::vector<int> *potentialNeighbours, int IdSet);				  //helper for computePVS
	void deleteNodePolygons(BSPTreeCreatorNode *node);								//deletes unnecesary polygon in non-leaf nodes - otherwise it would try to save them and crash..	
	void initVertices(BSPTreeCreatorNode *node);							//calls initVertices in leaves
};

  
 

 
class BSPTreeCreatorNode : public BSPTreeNodeTemplate<BSPTreeCreatorNode> { //public BSPTreeCreatorNode {

public:
		BSPTreeCreatorNode(std::vector<BSPTreePolygon> polygons, BSPTreeCreator *tree){ 
	
		PolygonSet = polygons;

		Tree = tree;
	
		Id = tree->addMe(this);
		PVNodes.push_back(Id);		//this is in its own PVNodes
	}

	~BSPTreeCreatorNode(){
		for(unsigned int i = 0; i < PortalSet.size(); i++){
			delete PortalSet[i];
		}
	}

	
	//BSPTreePolygon Divider;				
	//BSPTreeCreatorNode *LeftChild;				//The left sub tree of this node.		
	//BSPTreeCreatorNode *RightChild;			//The right sub tree of this node.		
	BSPTreeCreator *Tree;						//The tree this node belongs to.

	std::vector<BSPTreePolygon> PolygonSet;	

	std::vector<BSPTreePolygon*> PortalQueue;			//queue for processing portal in leaf
	std::vector<BSPTreePolygon*> PortalSet;				//accepted portals
	std::vector<int> PVNodes;			///////////PVS of node 		
	std::vector<int> immediateNeighbours;

	BSPTreePolygon Divider;

	void initVertices();				//converts PolygonSet and PortalSet to vertices for saving
	//bool isLeaf();
	void computeExtremes();

	BSPTreePolygon *getPortal();							
	BSPTreePolygon *getPortal2();//new method not finished..
	int chooseBestDividingPolygon();
	float checkPotentialDividerPolygon(int index);

	bool isConvexSet();
	void processPortals();				//processes candidate portal polygons
	bool clip(BSPTreePolygon *&portalPolygon);

	void mergePortals(BSPTreePolygon *&portal1, BSPTreePolygon *&portal2);		//filters out redundant and empty portals from PortalSet + adds them to Tree PortalSet

	bool rayIntersectsSomething(vec3 start, vec3 end);
	bool splitOnHoles(BSPTreePolygon *portalPolygon); 

	BSPTreePolygon *removeCoincidingParts(BSPTreePolygon *&portalPolygon); 

};


