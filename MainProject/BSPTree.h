#pragma once


#include <glm/glm.hpp>
#include <vector>
#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <sys/stat.h>
#include <sstream>
#include <queue>
#include <ctime>
#include <memory>

#include "keyStates.h"
#include "BSPTreePolygon.h"
#include "Control.h"
#include "Font.h"

#include "Texture.h"
#include "time_item.h"
#include "Model.h"
#include "BSPTreeNodeTemplate.h"
#include "Camera.h"

using namespace glm;

class BSPTreeCreatorNode;
class BSPTreeNode;

/*
Basic BSP Tree class with only necessary data member and methods.
Intended for loading bsp and pvs and drawing.
For computation of pvs and bsp structure + saving there is class BSPTreeCreator.
*/
class BSPTree {
public:
	
	BSPTree(std::string filename, vec3 thisToWorldVector, GLuint color);
	~BSPTree(void);

	
	static enum drawing_method_enum { BSP, BSP_BACK, PVS, NODES, BSP_QUERY, PVS_QUERY, PVS_BACK };

	int drawing_method;
	bool drawPortals;									//switch for displaying portals

	//tree parameters
	int depth;			//current depth
	int max_depth;
	int node_count;
	int leaves;

	int draw_calls;
	int query_draw_calls;
	int previous_draw_calls;
	int max_draw_calls;
	int max_draw_calls_with_query;
	int min_draw_calls;
	int min_draw_calls_with_query;

	int draw_calls_sum;
	int draw_calls_sum_with_queries;
	int draw_cycles;
	GLuint64 samples_passed_sum;
	GLuint64 samples_passed;

	bufferedQuery sample_query;
	Query simple_query;	    //for use in BSP with QUERY instead of sample query.. because why not.. im using simple queries there anyway..
	time_item time;

	//float avg_draw_calls;   //draw_calls_sum / draw_cycles

	
	Texture mazeTexture;
	std::unique_ptr<Sampler> samplerObject;
	GLuint program;

	//It's a static model only shifted into all nodes. Drawn in every leaf
	std::unique_ptr<Model> model;		//model to draw in every rendered leaf to simulate more expensive leaf and mask small overhead of some draw methods


	GLuint mazeSamplerUniform;
	static GLuint colorUniform;											//constant for use in drawPolygons

	std::string name;					//file name to load from
	BSPTreeNode *RootNode;	
	std::queue<BSPTreeNode *> queue;  //for use in drawBSPTree_withQuery
	std::vector<BSPTreeNode *> visited;		//must check visited nodes because of circular paths..

	std::unique_ptr<dynamicText> depthInfo;				
	std::unique_ptr<dynamicText> drawCalls;
	Control *control;


	glutil::MatrixStack modelToWorldMatrix;
	vec3 modelToWorldVector;		

	void draw(vec3 position);					//draws tree based on drawing method
	bool loadPVS();

	BSPTreeNode *getNode(BSPTreeNode *node, vec3 position);			//traverses bsptree and returns id of leaf

	void initVAOs(BSPTreeNode *node);										//calls initVBO in leaves

private:
	void drawBSPTree(BSPTreeNode *node, vec3 position);		//draws the whole tree
	void drawBSPTreeFromBack(BSPTreeNode *node, vec3 position);
	void drawBSPTree_withQuery(vec3 position);
	void drawPVSet(vec3 position);			//draws only nodes in around position
	void drawPVSetFromBack(vec3 position);
	void drawPVSet_withQuery(vec3 position);	//with portal query - eliminates drawing behind and not actually visible nodes
	void drawNodes(BSPTreeNode *node, vec3 position, int depth);
	int getDepth(BSPTreeNode *node, int depth);
	void deleteTree(BSPTreeNode * node);


};

/*
class drawable{

VAO
vector<GLuint> VBO

//+ something else?
}

*/




///inheritance from template object made from this class..
//Enables inheriting structure of tree and basic universal operations over it - isLeaf method
class BSPTreeNode : public BSPTreeNodeTemplate<BSPTreeNode>
{
public:
	
	BSPTreeNode(void) : occlusionQuery(GL_SAMPLES_PASSED){
		Tree = nullptr;
		
		numberOfPortalTriangles = 0;
		node_count++;
		visible = true;
	}

	BSPTreeNode(BSPTree *tree) : occlusionQuery(GL_SAMPLES_PASSED){
		Tree = tree;
		
		numberOfPortalTriangles = 0;
		node_count++;
		visible = true;
		
	}

	~BSPTreeNode(void);

	static int node_count;

	GLuint VAO;
	GLuint VBO;
	GLuint textureVBO;


	std::vector<BSPTreeNode *> PVNodes;			///////////PVS of node 		//save
	std::vector<BSPTreeNode *> immediateNeighbours;				//idea.. make PVNodes not pointers but copies of minimal necessary structure for drawing
	std::vector<basicPolygon> triangles;		//keeps geometry of node for checking collisions n stuff
	std::vector<BSPTreePolygon> portals;

	//make this pointers and initialize only in leaves...
	bufferedQuery occlusionQuery;			//for occlusion query of node portals.
	Query simple_query;					//for occlusion query of node portals but with wait

	bool visible;
	GLuint64 samples;

	int numberOfPortalTriangles;
	int numOfVertices;
	
	vec3 center;
	glutil::MatrixStack modelMatrix;	//transform for placing model in node

	BSPTree *Tree;						//The tree this node belongs to.
	
	void drawPortals();
	void drawPolygonSet();
	void initVBO();				//creates vbo, vao and sets its data + mapping into vao
	void deleteVBO();			//cleanup version of initVBO
	void inferTextureMapping(std::vector<vec2> &textureCoordinates); 
	void issueQuery();
	void issueSimpleQuery();
	void checkCollisions(Camera &camera, float radius = 0.1f);    //for checking camera collisions - radius should be approx. at the distance of near plane

	void getExtremes();


};



