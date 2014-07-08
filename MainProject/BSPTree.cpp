#include "BSPTree.h"



GLuint BSPTree::colorUniform = 0; 
int BSPTreeNode::node_count = 0;

BSPTree::BSPTree(std::string filename, vec3 thisToWorldVector, GLuint color)  : sample_query(GL_SAMPLES_PASSED){  ///filename here, leave default constructor enmpty..

	std::cout << "BSP tree constructed\n";

	BSPTree::colorUniform = color;
	modelToWorldVector = thisToWorldVector;
	modelToWorldMatrix.Translate(modelToWorldVector); //position entrance in front of camera

	drawPortals = false;

	drawing_method = drawing_method_enum::BSP;

	depth = 0;
	draw_calls = 0;
	query_draw_calls = 0;
	node_count = 0;
	leaves = 0;

	draw_calls_sum = 0;
	draw_calls_sum_with_queries = 0;
	draw_cycles = 0;
	samples_passed_sum = 0;
	
	max_draw_calls = 0;
	max_draw_calls_with_query = 0;
	min_draw_calls = INT_MAX;
	min_draw_calls_with_query = INT_MAX;

	name = filename;

	if(not loadPVS())
		return;
	
	//load model
	model = std::unique_ptr<Model>(new Model("../Models/simple_bunny_fixed_holes.obj"));
	model->loadModel();
	model->initVAO();

	srand(::time(nullptr));
	initVAOs(this->RootNode);


	max_depth = getDepth(this->RootNode, 0) - 1;  //count to index conversion

	control = Control::getInstance();
	depthInfo = std::unique_ptr<dynamicText>(new dynamicText(control->font.get()));

	depthInfo->addTextData("Depth: "+std::to_string(depth)+"/"+std::to_string(max_depth), control->windowHeight, control->windowWidth - 180, 30, 15);
	depthInfo->uploadData();

	drawCalls = std::unique_ptr<dynamicText>(new dynamicText(control->font.get()));
	drawCalls->addTextData("No. of draw calls: "+ std::to_string(draw_calls), control->windowHeight, control->windowWidth / 2 - 8*15 , 30, 15);
	drawCalls->uploadData();


	mazeTexture.loadFromFile("../Textures/compositeTexture/compositeTextureStarryCeiling2.png");

	program = control->getProgram("maze")->id;
	mazeSamplerUniform = glGetUniformLocation(program, "mazeSampler");

}


BSPTree::~BSPTree(void){

	if(RootNode == nullptr)
		return;

	deleteTree(RootNode);



	std::cout << "Undeleted nodes: " << BSPTreeNode::node_count << "\n" << std:: endl;
	std::cout << "BSP tree destructed\n";
}


/*
Recursive deletion of nodes
*/
void BSPTree::deleteTree(BSPTreeNode *node) {
	
	if(node->isLeaf()){
		delete node;
	return;
	}

	//this is post order traversal..
	deleteTree(node->RightChild);
	deleteTree(node->LeftChild);
	delete node;
}




BSPTreeNode::~BSPTreeNode(void){
//everything except pointers to children are values.
	node_count--;

	deleteVBO();

}


/*
So far only walls.
Later
- objects - rabbits
- ceiling - jumps
- floor - gravity?
- uneven terrain?
- other cases..?
*/
void BSPTreeNode::checkCollisions(Camera &camera, float radius){

	vec3 position = camera.position - Tree->modelToWorldVector;
	 
	float collision_distance = 2 * radius;  //2*radius because I want it so..
	
	//possible cases..    
	//-with wall							//easy
	//-with wall at portal to another node	//not a problem - portal could be trouble in case of angle between nodes < 180.. not a case here..
	//-with multiple walls					//solved by checking against all triangles in node
	//--corner or more						//the same

	
	//have triangles..
	//for all
		//get distance
		//nothing
		//or collision reaction  --shouldnt cause another collision

	float distance_from_plane;

	for(unsigned int i = 0; i < triangles.size(); i++){
		if((distance_from_plane = triangles[i].planePointDistance(position)) <= collision_distance){ 
			
			//to prevent triangle plane to block passage to another node check if this collision happens in its area
			vec3 opposite_normal = triangles[i].normal * -1.f;
			vec3 vector = position + opposite_normal;
			vec3 intersection_point = triangles[i].planeVectorIntersection(position, vector);

			if(triangles[i].isPointInPolygon(intersection_point))
				//react - move position in direction of plane normal a difference between its distance from plane and set collision distance 
				position = position + (collision_distance - distance_from_plane) * triangles[i].normal;
		}
	}
	
//update camera position with corrected value
camera.setPosition(position + Tree->modelToWorldVector);

}

/*
Loads previously saved BSP tree with its PVS.
*/
bool BSPTree::loadPVS(){

using namespace std;

cout << "loading PVS\n";

	FILE *file;

	fopen_s(&file, name.c_str(),"r");

	if(file == nullptr){
		cout << "error opening file " << name << " for load\n";
	return false;
	}

std::vector<BSPTreeNode*> NodeSet; //all nodes
vector<vector<int>> Children;			//their children // format: left, right
vector<vector<int>> PVNIds;				//Ids of PV nodes
vector<vector<int>> immediateNeighboursIds;				//Ids of immediateNeighbour nodes
vector<vector<int>> connectedIds;		//Ids of nodes connected through portals
//these three belong to node at common index and are sorted because they are saved sorted

		//Id of node
		//Id of children - 0 0 if none
		//Divider polygon // or default values - 0 0 0 x3
		//Ids of immediate neighbours
		//Ids of PVNodes x y z a g s
		//Colors	r g b
		//No of Polygons and No of Portals x y
		//Ids of immediate neighbours connected with portals
		//vertices x y z...
		//2x endl

/*
0
2 1
0.5 0.5 0.5
0.5 -0.5 0.5
-0.5 0.5 0.5
0
0 0 0
0 0


1
0 0
0 0 0
0 0 0
0 0 0
1 3 9 15 72 19 60
0.406 0.61 0.00399995
32 10
0.5 0.5 0.5
0.5 -0.5 0.5
-0.5 0.5 0.5
0.5 -0.5 0.5
-0.5 -0.5 0.5
-0.5 0.5 0.5
	*/

	while (true) {

		//create new node
		BSPTreeNode *node = new BSPTreeNode(this);
		NodeSet.insert(NodeSet.end(), node);

		//read Id
		if(fscanf_s(file, "%d\n", &node->Id) <= 0){
			NodeSet.pop_back();  //must remove recently added empty node
			delete node;
			break;
		}

		cout << "\rNode: " << to_string(node->Id) << "          ";

		//Create note of children
		int LeftChild, RightChild;
		fscanf_s(file, "%d %d\n", &LeftChild, &RightChild);
		vector<int> children;
		children.insert(children.end(), LeftChild);
		children.insert(children.end(), RightChild);

		Children.insert(Children.end(), children);

		//get divider
		vec3 poly[3];
		fscanf_s(file, "%f %f %f\n", &poly[0].x, &poly[0].y, &poly[0].z);
		fscanf_s(file, "%f %f %f\n", &poly[1].x, &poly[1].y, &poly[1].z);
		fscanf_s(file, "%f %f %f\n", &poly[2].x, &poly[2].y, &poly[2].z);

		node->Divider = basicPolygon(poly[0], poly[1], poly[2]);


		//Ids of immediateNeighbours
		{
			vector<int> Ids;

			int i;
			string str;
			char c;
		
			while ((c = fgetc(file)) != '\n')
				str.append(1, c);
		

		
			stringstream stream(str);
		
			while(stream.good()){
				stream >> i;
				Ids.insert(Ids.end(), i);
			}
	
			immediateNeighboursIds.insert(immediateNeighboursIds.end(), Ids);

		}

		//Ids of PVNodes
		{
			vector<int> Ids;

			int i;
			string str;
			char c;
		
			while ((c = fgetc(file)) != '\n')
				str.append(1, c);
		

		
			stringstream stream(str);
		
			while(stream.good()){
				stream >> i;
				Ids.insert(Ids.end(), i);
			}
	
			PVNIds.insert(PVNIds.end(), Ids);
		}

		//Colors
		fscanf_s(file, "%f %f %f\n", &node->red, &node->green, &node->blue);

		//No of Polygons and No of Portals
		int polygons, portals;
		fscanf_s(file, "%d %d\n", &polygons, &portals);
		node->numberOfPortalTriangles = portals;


		
		//Ids of immediate neighbours connected with portals
		vector<int> connIds;
		if(portals > 0){
		
			for(int p = 0; p < portals / 2; p++){
				int Id;

				if(p < portals / 2 - 1)
					fscanf_s(file, "%d ", &Id);
				else
					fscanf_s(file, "%d\n", &Id);

				connIds.push_back(Id);
			}
		}

		connectedIds.insert(connectedIds.end(), connIds);
		


		//vertices
		if(polygons + portals != 0){

			leaves = leaves + 1;

			for(int j = 0; j < (polygons + portals) * 3; j++){ //for every vertex
		
				vec3 point;

				fscanf_s(file, "%f %f %f\n", &point.x, &point.y, &point.z);
				node->vertices.push_back(point);
			}

			node->getExtremes();

			for(int j = 0; j < polygons; j = j + 1){
				node->triangles.push_back(basicPolygon(node->vertices[j * 3 + 0], node->vertices[j * 3 + 1], node->vertices[j * 3 + 2]));
			
			}

			//happens that number of polygons + no of portals * 3 != poctu radku v souboru..
			if(node->Id == 7)
			Sleep(1);
		}

		

	/*
	1---2
	| \ |
	0---3
	saved as 312 301
	*/
		int start_index = polygons * 3;

		for(int k = 0; k < portals * 3; k = k + 6){ // 2 portal triangles - 6 points
			vector<vec3> points;

			points.push_back(node->vertices[start_index + k + 4]);
			points.push_back(node->vertices[start_index + k + 2]);
			points.push_back(node->vertices[start_index + k + 3]);
			points.push_back(node->vertices[start_index + k + 1]);

			node->portals.push_back(BSPTreePolygon(points));
			points.clear();
		}

		//separating double endl
		
	}

	cout << "\rBuilding tree structure     ";

	node_count = NodeSet.size();

	this->RootNode = NodeSet.at(0);

	///build tree structure
	for(uint i = 0; i < NodeSet.size(); i++){
	//all is loaded already sorted by Id - i is index to Node with Id == i
	
		//obtain pointers to children - only if nonzero - zero means no child
		if( Children.at(i)[0] != 0 && Children.at(i)[1] != 0 ){
			NodeSet.at(i)->LeftChild  = NodeSet.at(  Children.at(i)[0]  );
			NodeSet.at(i)->RightChild = NodeSet.at(  Children.at(i)[1]  );
			
		}

		//obtain pointers to PVNodes  vector<vector<int>> PVNIds; //Ids of PV nodes
		for(uint j = 0; j < PVNIds.at(i).size(); j++){

			NodeSet.at(i)->PVNodes.insert(  NodeSet.at(i)->PVNodes.end(), NodeSet.at( PVNIds.at(i)[j] )  );

		}

		//obtain pointers to Nodes  vector<vector<int>> immediateNeighboursIds; //Ids of immediateNeighbour nodes
		for(uint j = 0; j < immediateNeighboursIds.at(i).size(); j++){

			NodeSet.at(i)->immediateNeighbours.insert(  NodeSet.at(i)->immediateNeighbours.end(), NodeSet.at( immediateNeighboursIds.at(i)[j] )  );

		}

		NodeSet.at(i)->immediateNeighbours.erase(NodeSet.at(i)->immediateNeighbours.begin()); // remove first item which is Id of this node

		
		//put pointer to nodes into portals' connectedTo
		for(uint j = 0; j < NodeSet.at(i)->portals.size(); j++){
		
			NodeSet.at(i)->portals[j].connectsTo = NodeSet.at(connectedIds[i][j]);  
		
		}
	}




	
	////////////here load vertices also into non leaf nodes...
	//////make possible displaying of process of splitting sets of polygons..
	while(RootNode->vertices.size() == 0){
	
		for(unsigned int i = 0; i < NodeSet.size(); i++){
		
			if(NodeSet[i]->vertices.size() == 0 && NodeSet[i]->RightChild->vertices.size() != 0 && NodeSet[i]->LeftChild->vertices.size() != 0){
				
				NodeSet[i]->vertices.insert(NodeSet[i]->vertices.end(), NodeSet[i]->RightChild->vertices.begin(), NodeSet[i]->RightChild->vertices.end() - NodeSet[i]->RightChild->numberOfPortalTriangles * 3);
				NodeSet[i]->vertices.insert(NodeSet[i]->vertices.end(), NodeSet[i]->LeftChild->vertices.begin(), NodeSet[i]->LeftChild->vertices.end() - NodeSet[i]->LeftChild->numberOfPortalTriangles * 3);

				NodeSet[i]->red = (NodeSet[i]->RightChild->red + NodeSet[i]->LeftChild->red) / 2;
				NodeSet[i]->green = (NodeSet[i]->RightChild->green + NodeSet[i]->LeftChild->green) / 2;
				NodeSet[i]->blue = (NodeSet[i]->RightChild->blue + NodeSet[i]->LeftChild->blue) / 2;

			}	
		
		}
	
	}



cout << "\rPVS loaded               \n";
fclose(file);
return true;
}


void BSPTreeNode::getExtremes(){

	if(vertices.size() > 0){
	//some existing point values
		xmax = vertices[0].x;
		ymax = vertices[0].y;
		zmax = vertices[0].z;

		xmin = vertices[0].x;
		ymin = vertices[0].y;
		zmin = vertices[0].z;
	
	}

	//for each polygon in polygonset
	for(unsigned int i = 0; i < vertices.size() - numberOfPortalTriangles * 3; i++){
				
		xmax = max(vertices[i].x, xmax);
		ymax = max(vertices[i].y, ymax);
		zmax = max(vertices[i].z, zmax);
			
		xmin = min(vertices[i].x, xmin);
		ymin = min(vertices[i].y, ymin);
		zmin = min(vertices[i].z, zmin);
	
	}

	center = vec3((xmax + xmin) / 2.0, (ymax + ymin) / 2.0f, (zmin + zmax) / 2.0f);



}

/*
bool BSPTreeNode::isLeaf(){

	return (this->LeftChild == nullptr && this->RightChild == nullptr);
}
*/
/* ARB_occlusion_query2.txt

	Occlusion Queries (target SAMPLES_PASSED or ANY_SAMPLES_PASSED)

	Issues

    1) Can an occlusion query with a target of SAMPLES_PASSED
       be active at the same time as an occlusion query with a
       target of ANY_SAMPLES_PASSED?

       Resolved.  No!  Specifically, it is an error to BeginQuery for
       the targets SAMPLES_PASSED or ANY_SAMPLES_PASSED if the active
       query is non-zero for EITHER target.

	3) Can we limit the number of query objects active?

       Unresolved.  This draft doesn't.

       But, yes, we can limit the number of query objects active.
       The language to do so would be straightforward, but why?

       There can only be ONE occlusion query active at a time.  The query object
       created (or re-used) at BeginQuery contains the state to save the
       boolean.  At EndQuery the boolean for the current query is copied to
       the query object's state.

       (Clearly, OES can make another choice.  But not sure why right now.)
	*/


/*
Manages drawing. Calls correct drawing method and collects statistics.
*/
void BSPTree::draw(vec3 position){

	//all geometry is in model coordinates and camera is in world coordinates
	position = position - modelToWorldVector;//correction of position to model coordinates

	previous_draw_calls = draw_calls + query_draw_calls;  //to be able to detect change after drawing
	
	draw_calls = 0;
	query_draw_calls = 0;

	


	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, mazeTexture.id);
		
	glProgramUniform1i(program, mazeSamplerUniform, 0);	//set GLSL sampler to sample from texure unit 0 - default


	samplerObject->bindToUnit(0);



		switch (drawing_method){
	
		case drawing_method_enum::PVS:			//static PVS

			drawPVSet(position);

			break;

		case drawing_method_enum::PVS_BACK:			//static PVS

			drawPVSetFromBack(position);

			break;

		case drawing_method_enum::PVS_QUERY:		//PVS + portal queries
					
			drawPVSet_withQuery(position);	//this draws only visible parts + portals when toggled on, iterative
			

			break;

		case drawing_method_enum::BSP:		//just BSP

			glBeginQuery(GL_SAMPLES_PASSED, sample_query.query());

				drawBSPTree(RootNode, position);    //this draws all + portals when toggled on, recursive

			glEndQuery(GL_SAMPLES_PASSED);

			sample_query.getResult(samples_passed);

			samples_passed_sum = samples_passed_sum + samples_passed;	

			break;

		case drawing_method_enum::BSP_BACK:		//just BSP drawn from back to front - not z buffer friendly..

			glBeginQuery(GL_SAMPLES_PASSED, sample_query.query());

				drawBSPTreeFromBack(RootNode, position);    //this draws all + portals when toggled on, recursive

			glEndQuery(GL_SAMPLES_PASSED);

			sample_query.getResult(samples_passed);

			samples_passed_sum = samples_passed_sum + samples_passed;	

			break;

		case drawing_method_enum::BSP_QUERY:		//BSP + portal query
			
				{
				BSPTreeNode *node = getNode(RootNode, position);
				queue.push(node);
				visited.clear();

				while(not queue.empty()){
					drawBSPTree_withQuery(position);
				}

		
			break;
			}

		case drawing_method_enum::NODES:		//division demonstration..
	
			drawNodes(RootNode, position, this->depth);  //no measurment here..
			samples_passed = 0;
			break;

	
		default:
			drawBSPTree(RootNode, position);    //this draws all + portals when toggled on
			break;
		}

	


	
	glBindTexture(GL_TEXTURE_2D, 0);
	samplerObject->unbindFromUnit(0);
	////Now take care of some statistics.


	draw_calls_sum = draw_calls_sum + draw_calls;
	draw_calls_sum_with_queries = draw_calls_sum_with_queries + draw_calls + query_draw_calls;

	
	//collect min/max values
	max_draw_calls = max(max_draw_calls, draw_calls);
	max_draw_calls_with_query = max(max_draw_calls_with_query, draw_calls + query_draw_calls);

	min_draw_calls = min(min_draw_calls, draw_calls);
	min_draw_calls_with_query = min(min_draw_calls_with_query, draw_calls + query_draw_calls);


	draw_cycles++;



	time_item now;
	float difference = now.seconds(time);
	
	//update text only at change or after half a second
	if(previous_draw_calls != draw_calls + query_draw_calls || difference >= 0.5f){
		time = now;

		//samples passed depth test per draw cycle / window pixel count  
		float overdraw = samples_passed_sum / draw_cycles / (control->windowHeight * control->windowWidth) * 100.0f;		////////suprisingly constant in BSP and PVS.. but i guess it makes sense.. 

		drawCalls->addTextData("No. of draw calls: "+ std::to_string(draw_calls)+" + "+std::to_string(query_draw_calls)+
			"\nOverdraw: "+std::to_string(overdraw)+"%", control->windowHeight, control->windowWidth / 2 - 8 * 15 , 30, 15);

		drawCalls->uploadData();
	}

}



/*
w DRAW-BSP-TREE
w Indata:
w Node – The node to draw.
w Position – The viewer’s position.
w Outdata:
w None
w Effect:
w Draws the polygons contained in the node and its sub trees.

DRAW-BSP-TREE (Node, Position)
1 if (IS-LEAF(Node))
2 DRAW-POLYGONS (Node.PolygonSet)
3 return
 
w Calculate which sub tree the viewer is in.
4 Side f CLASSIFY-POINT (Node.Divider, Position)
 
w If the viewer is in the right sub tree draw that tree before the
w left.
5 if (Side = INFRONT || Side = COINCIDING)
6 DRAW-BSP-TREE (Node.RightChild, Position)
7 DRAW-BSP-TREE (Node.LeftChild, Position)
 
w Otherwise draw the left first.
8 else if(Value = BEHIND)
9 DRAW-BSP-TREE (Node.LeftChild, Position)
10 DRAW-BSP-TREE (Node.RightChild, Position)
*/
void BSPTree::drawBSPTree(BSPTreeNode *node, vec3 position){
	

	if(node->isLeaf()){
		node->drawPolygonSet();
	return;
	}

	int side = node->Divider.classifyPoint(position);

	if(side == INFRONT || side == COINCIDING){

		drawBSPTree(node->RightChild, position);
		drawBSPTree(node->LeftChild, position);
		
	} else if (side == BEHIND){

		drawBSPTree(node->LeftChild, position);
		drawBSPTree(node->RightChild, position);

	}
}


void BSPTree::drawBSPTreeFromBack(BSPTreeNode *node, vec3 position){
	

	if(node->isLeaf()){
		node->drawPolygonSet();
	return;
	}

	int side = node->Divider.classifyPoint(position);

	if(side == INFRONT || side == COINCIDING){
		
		drawBSPTreeFromBack(node->LeftChild, position);
		drawBSPTreeFromBack(node->RightChild, position);

	} else if (side == BEHIND){
		drawBSPTreeFromBack(node->RightChild, position);
		drawBSPTreeFromBack(node->LeftChild, position);
		
	} 
}

/*
Walks visible leaves through portals.
Essentially BFS with leaves of tree.
*/
void BSPTree::drawBSPTree_withQuery(vec3 position){

	BSPTreeNode *node = queue.front();
	queue.pop();
	

	//if not visited
	if(std::find(visited.begin(), visited.end(), node) == visited.end()){
		
		glBeginQuery(GL_SAMPLES_PASSED, simple_query.id); 

			node->drawPolygonSet();
			visited.push_back(node);
	
		glEndQuery(GL_SAMPLES_PASSED);

		simple_query.getResult(samples_passed);

		samples_passed_sum = samples_passed_sum + samples_passed;

		//do portal query for all neighbours except previously visited node.
		//for(unsigned int i = 0; i < node->portals.size(); i++){
		for(auto portal : node->portals){
			auto neighbour = portal.connectsTo;

			if(std::find(visited.begin(), visited.end(), neighbour) == visited.end()){  //must avoid running backwards.. + reissuing query
				
				//neighbour->issueQuery();   

				//because query_buffer lags with corrrect result by one frame, it means that
				//when very long corridor - lets say 20+ nodes - is all invisible and suddenly should be visible,
				//due to the nature of this algorithm it means that it reports first node as visible next frame
				//and doesnt ask next in row behing the first because it assumes its hidden.
				//next frame the first node reports as visible and the whole story is repeated for the 2,3..100+ node 
				//comprising the very long corridor. In effect the whole corridor is drawn after nodes.size() draw cycles which is definitely noticeable..
						//solution - use simple query with wait for result - slower but without artifacts..

				neighbour->issueSimpleQuery();


				if(not neighbour->visible){

					float distance = abs(portal.planePointDistance(position));  //winding of portals is not constant so use +-distance

					//if I'm close to one
					if(distance <= 0.1f && distance >= 0.0f){

						//override possible opposite result
						portal.connectsTo->visible = true;

					}
				}


				//note visible ones for visit
				if(neighbour->visible){
					queue.push(neighbour);
				

				}
			}

		}

	}

}




/*
Recursive check for depth of (sub)tree of node.
*/
int BSPTree::getDepth(BSPTreeNode *node, int depth){
	
	int current_depth = depth;

	if(node->isLeaf()){
		return depth + 1;
	}

	depth = glm::max(getDepth(node->RightChild, current_depth), depth);

	depth = glm::max(getDepth(node->LeftChild, current_depth), depth);

return depth + 1;
}


/*
Function similar to drawBSPTree, but draws only nodes at specified depth in tree..
*/
void BSPTree::drawNodes(BSPTreeNode *node, vec3 position, int depth){

	if(depth == 0 || node->isLeaf()){
		node->drawPolygonSet();
	return;
	}

	int side = node->Divider.classifyPoint(position);

	if(side == INFRONT || side == COINCIDING){
		
		drawNodes(node->RightChild, position, depth - 1);
		drawNodes(node->LeftChild, position, depth - 1);

	} else if (side == BEHIND){

		drawNodes(node->LeftChild, position, depth - 1);
		drawNodes(node->RightChild, position, depth - 1);
	} 
}

/*
must be called before drawing tree
creates VAOs at all tree levels to enable display of the division process.
*/
void BSPTree::initVAOs(BSPTreeNode *node){

	
	//if(node->isLeaf()){
		node->initVBO();
//	return;
	//}
		if(node->isLeaf())
			return;


		initVAOs(node->RightChild);
		initVAOs(node->LeftChild);

}


/*
Gets node the position is in and draws its PVNodes.
No exclusion of nodes behind or not actually visible.
*/
void BSPTree::drawPVSet(vec3 position){

	BSPTreeNode *currentNode = getNode(RootNode, position);

	
	std::vector<BSPTreeNode *> *PVNodes = &currentNode->PVNodes;

	
	glBeginQuery(GL_SAMPLES_PASSED, sample_query.query());

	
	//draw all with all visible attributes correctly set
	for(unsigned int i = 0; i < PVNodes->size(); i++){

		PVNodes->at(i)->drawPolygonSet();

	}

	glEndQuery(GL_SAMPLES_PASSED);
		

	sample_query.getResult(samples_passed);

	samples_passed_sum = samples_passed_sum + samples_passed;

}

/*
Gets node the position is in and draws its PVNodes.
No exclusion of nodes behind or not actually visible.
*/
void BSPTree::drawPVSetFromBack(vec3 position){

	BSPTreeNode *currentNode = getNode(RootNode, position);

	
	std::vector<BSPTreeNode *> *PVNodes = &currentNode->PVNodes;

	
	glBeginQuery(GL_SAMPLES_PASSED, sample_query.query());

	
	//draw all with all visible attributes correctly set
	for(unsigned int i = PVNodes->size() - 1; i >= 0 ; i--){

		PVNodes->at(i)->drawPolygonSet();

	}

	glEndQuery(GL_SAMPLES_PASSED);
		

	sample_query.getResult(samples_passed);

	samples_passed_sum = samples_passed_sum + samples_passed;

}

/*
Gets node the position, performs occlusion query on portals and draws visible.
*/
void BSPTree::drawPVSet_withQuery(vec3 position){

	BSPTreeNode *currentNode = getNode(RootNode, position);

	/*std::cout << " Id: " << currentNode->Id;
	std::cout << " Nghbs: ";
	for(unsigned int i = 0; i < currentNode->immediateNeighbours.size(); i++){
		std::cout << currentNode->immediateNeighbours[i]->Id << " ";
	}
	
	std::cout <<" pTriangles: " << currentNode->numberOfPortalTriangles << std::endl;
	*/


	//retest only after N frames..?
	//but then make nodes visible by default to prevent possible black spots..

	std::vector<BSPTreeNode *> *PVNodes = &currentNode->PVNodes;

	

	
	//problem... near clipping plane causes close portal to not draw -> no samples => next node is not drawn when you dont see any other portal..
	//if very close to own portal - less than fzNear

	//go through my portals
	for(unsigned int i = 0; i < currentNode->portals.size(); i++){

		float distance = abs(currentNode->portals[i].planePointDistance(position));  //winding of portals is not constant so use +-distance

		//if I'm close to one
		if(distance <= 0.1f && distance >= 0.0f){

			//override possible opposite result
			currentNode->portals[i].connectsTo->visible = true;
			break;

		}
	}

	//draw self
	currentNode->visible = true;
	
	glBeginQuery(GL_SAMPLES_PASSED, sample_query.query());

	//draw all with all visible attributes correctly set
	for(unsigned int i = 0; i < PVNodes->size(); i++){

		if(PVNodes->at(i)->visible)
			PVNodes->at(i)->drawPolygonSet();

	}
	
	glEndQuery(GL_SAMPLES_PASSED);
		

	//first retrieve results from last frame + issue new ones
	//wow.. it looks that this works even though no other geometry is drawn first.. - no it doesnt..
	//dont query self.. self is always visible.
	for(unsigned int i = 1; i < PVNodes->size(); i++){

		PVNodes->at(i)->issueQuery();

	}



	sample_query.getResult(samples_passed);

	samples_passed_sum = samples_passed_sum + samples_passed;

}


/*
Draws portals inside scope of occlusion query and retrieves
result from previous one and sets visible to true or false.
*/
void BSPTreeNode::issueQuery(){
		
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);   //prevent writing into depth and frame buffer
	glDepthMask(GL_FALSE);

	//glBeginQuery(GL_ANY_SAMPLES_PASSED, occlusionQuery.query(0));
	glBeginQuery(GL_SAMPLES_PASSED, occlusionQuery.query());
	 
		drawPortals();	//draw portals
		
	//glEndQuery(GL_ANY_SAMPLES_PASSED);
	glEndQuery(GL_SAMPLES_PASSED);
	

	//get result of previous query
	occlusionQuery.getResult(samples);
	
	//check if some samples passed
	if(samples == 0){
		visible = false;
	} else {
		visible = true;
		Tree->samples_passed_sum = 	Tree->samples_passed_sum + samples; //add this ammount to global samples passed sum
	}
	//std::cout << Id  << " " << visible << " " << samples << "\n";
	

	
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

}


/*
Draws portals inside scope of occlusion query and retrieves
result. USES QUERY WITH WAIT CYCLE. Sets visible to true or false.
*/
void BSPTreeNode::issueSimpleQuery(){

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);   //prevent writing into depth and frame buffer
	glDepthMask(GL_FALSE);

	glBeginQuery(GL_SAMPLES_PASSED, simple_query.id);

		drawPortals();	//draw portals

	glEndQuery(GL_SAMPLES_PASSED);

	//get result of this query
	simple_query.getResult(samples);

	//check if some samples passed
	if(samples == 0){
		visible = false;
	} else {
		visible = true;
		Tree->samples_passed_sum = 	Tree->samples_passed_sum + samples; //add this ammount to global samples passed sum
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);


}


/*
Recursive function to go through tree and return node in which is position of camera.
Node is root of tree or branch to search.
*/
BSPTreeNode *BSPTree::getNode(BSPTreeNode *node, vec3 position){
	
	if(node->isLeaf())	
		return node;
	

	int side = node->Divider.classifyPoint(position);

	if(side == INFRONT || side == COINCIDING){
		
		return getNode(node->RightChild, position);

	} else if (side == BEHIND){

		return getNode(node->LeftChild, position);
	} 

	return nullptr;
}

/*
Infers from vertices if it's wall, floor, ceiling or portal and creates mapping for them
to apropriate part of texture.
*/
void BSPTreeNode::inferTextureMapping(std::vector<vec2> &textureCoordinates){

	enum {wall, floor, ceiling, portal};
	int type;

	for(unsigned int i = 0; i < vertices.size(); i = i + 6){
	//portal check first to not test positive on wall..
		if(i >= (vertices.size() - numberOfPortalTriangles * 3)){
		
			type = portal;		
		
		} else if(vertices.at(i).y <= 0.0f && vertices.at(i + 1).y <= 0.0f && vertices.at(i + 2).y <= 0.0f){
		
			type = floor;

		} else if(vertices.at(i).y >= 0.0f && vertices.at(i + 1).y >= 0.0f && vertices.at(i + 2).y >= 0.0f){
		
			type = ceiling;
				
		} else {
		
			type = wall;
		}

		//wall, floor, ceiling, portal

switch(type){

	case wall:
		
		textureCoordinates.push_back(vec2(0.24f, 1.0f));
		textureCoordinates.push_back(vec2(0.0f, 0.0f));
		textureCoordinates.push_back(vec2(0.0f, 1.0f));
		
		textureCoordinates.push_back(vec2(0.24f, 1.0f));
		textureCoordinates.push_back(vec2(0.24f, 0.0f));
		textureCoordinates.push_back(vec2(0.0f, 0.0f));
	
	break;

	case floor:
		textureCoordinates.push_back(vec2(0.49f, 1.0f));
		textureCoordinates.push_back(vec2(0.26f, 0.0f));
		textureCoordinates.push_back(vec2(0.26f, 1.0f));
		
		textureCoordinates.push_back(vec2(0.49f, 1.0f));
		textureCoordinates.push_back(vec2(0.49f, 0.0f));
		textureCoordinates.push_back(vec2(0.26f, 0.0f));	
	
	break;

	case ceiling:
		textureCoordinates.push_back(vec2(0.74f, 1.0f));
		textureCoordinates.push_back(vec2(0.51f, 0.0f));
		textureCoordinates.push_back(vec2(0.51f, 1.0f));
		
		textureCoordinates.push_back(vec2(0.74f, 1.0f));
		textureCoordinates.push_back(vec2(0.74f, 0.0f));
		textureCoordinates.push_back(vec2(0.51f, 0.0f));	

	break;

	case portal:
		textureCoordinates.push_back(vec2(1.0f, 1.0f));
		textureCoordinates.push_back(vec2(0.76f, 0.0f));
		textureCoordinates.push_back(vec2(0.76f, 1.0f));
		
		textureCoordinates.push_back(vec2(1.0f, 1.0f));
		textureCoordinates.push_back(vec2(1.0f, 0.0f));
		textureCoordinates.push_back(vec2(0.76f, 0.0f));	

	break;

}
	}

/* template
	//one square - 2 triangles
	for(unsigned int i = 0; i < vertices.size(); i = i + 6){
		textureCoordinates.push_back(vec2(1.0f, 1.0f));
		textureCoordinates.push_back(vec2(0.0f, 0.0f));
		textureCoordinates.push_back(vec2(0.0f, 1.0f));
		
		textureCoordinates.push_back(vec2(1.0f, 1.0f));
		textureCoordinates.push_back(vec2(1.0f, 0.0f));
		textureCoordinates.push_back(vec2(0.0f, 0.0f));
	
	}

	*/

}

//creates VBO for vertices of node
void BSPTreeNode::initVBO(){

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


		glBufferData(GL_ARRAY_BUFFER, (vertices.size()) * 3 * sizeof(float), vertices.data(), GL_STATIC_DRAW); //again size in Bytes!!!
	

		glEnableVertexAttribArray(0);
			
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);//asociates vertex data in currently bound VBO to VAO; not part of vao state
				

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	////////generate texture mapping... different for floor and walls and ceiling...
	std::vector<vec2> textureCoordinates;
	inferTextureMapping(textureCoordinates); //different part of texture for floors, ceiling, walls and portals...



	glGenBuffers(1, &textureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);


		glBufferData(GL_ARRAY_BUFFER, (textureCoordinates.size()) * 2 * sizeof(float), textureCoordinates.data(), GL_STATIC_DRAW); //again size in Bytes!!!
	

		glEnableVertexAttribArray(1);
			
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);



	//dont need it anymore.. store size and discard - not true now.. use it for collision detection
	numOfVertices = vertices.size();
	//vertices.clear();


	float numberX = ((rand() % 70) - 35) / 100.0f;  //some variance -0.35 to 0.35 from center
	float numberZ = ((rand() % 70) - 35) / 100.0f;



	//set transform matrix for model..
	//maybe randomly bump it around too... 
	modelMatrix.Translate(vec3(center.x + numberX, -0.499f, center.z + numberZ));
	modelMatrix.Translate(Tree->modelToWorldVector);
	//modelMatrix.Scale(0.02f);

}

void BSPTreeNode::deleteVBO(){

glDeleteBuffers(1, &VBO);
glDeleteBuffers(1, &textureVBO);

glDeleteBuffers(1, &VAO);


}



//draws vertices of node.
void BSPTreeNode::drawPolygonSet(){
  	
	glUniform4f(BSPTree::colorUniform, red, green, blue , 1.0f);
			
	



	glUniformMatrix4fv(Tree->control->modelToWorldMatrixUniform, 1, GL_FALSE, glm::value_ptr(Tree->modelToWorldMatrix.Top()));  //model to world

	glBindVertexArray(VAO);	
	


	glDrawArrays(GL_TRIANGLES, 0, numOfVertices - numberOfPortalTriangles * 3);//last parameter is number of VERTICES!!!! to be rendered.
	Tree->draw_calls++;

	if(Tree->drawPortals){
		
		glDisable(GL_CULL_FACE);
			glDrawArrays(GL_TRIANGLES, numOfVertices - numberOfPortalTriangles * 3, numberOfPortalTriangles * 3);
			Tree->draw_calls++;

		glEnable(GL_CULL_FACE);
	}


	glBindVertexArray(0);		
	
	glUniformMatrix4fv(Tree->control->modelToWorldMatrixUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));  //rabbit model to world

	//glDisable(GL_CULL_FACE);
	Tree->model->draw();
	Tree->draw_calls++;
	//glEnable(GL_CULL_FACE);  
}

/*
Draws only portals.. mainly for purposes of occlusion query.
Doesn't count towards overall draw calls (technically it does but I'm not counting it)
*/
void BSPTreeNode::drawPortals(){

	glBindVertexArray(VAO);	
		
	glUniform4f(BSPTree::colorUniform, red, green, blue , 1.0f);
	glUniformMatrix4fv(Tree->control->modelToWorldMatrixUniform, 1, GL_FALSE, glm::value_ptr(Tree->modelToWorldMatrix.Top()));  //model to world

		glDisable(GL_CULL_FACE);
			glDrawArrays(GL_TRIANGLES, numOfVertices - numberOfPortalTriangles * 3, numberOfPortalTriangles * 3);
			Tree->query_draw_calls++;
		glEnable(GL_CULL_FACE);

	glBindVertexArray(0);		

}


