#include "BSPTreeCreator.h"

//#include <thread>

//stats
int leaf = 0, dividers = 0, portalsNumber = 0, portals_inserted = 0;

/*
gets vertex array from Maze->getVertexArray
mazeName is name to save resulting structure as mazeName.pvs
*/
BSPTreeCreator::BSPTreeCreator(std::vector<GLfloat> *vertexArray, std::string mazeName) {

	std::cout << "BSPTreeCreator constructed\n";
	if(vertexArray == nullptr || vertexArray->empty()){
		std::cout << "INVALID vertexArray!\n";
		return;
	}
	Control *control = Control::getInstance();

	time_points.push_back(time_item(clock(), "starting point"));
	

	/*
	threads = std::thread::hardware_concurrency();
	
	if(threads == 0)
		threads = 1;
	*/

	//vertexArray format x, y, z, 1.0
	//second half are colors

//	drawPortals = false;
	name = mazeName;

	std::vector<BSPTreePolygon> polygons;


	polygons.reserve(vertexArray->size() / 4 / 3); // 4 - coordinates in one point, 3 - points in polygon

	if((vertexArray->size() / 2) % 24 != 0)   //unit of wall has 6 vertices * 4
		return;

	std::vector<vec3> p;
	p.resize(3);


	//conversion from vec4 into vec3 and BSPTreePolygons   
	for(unsigned int i = 0; i < vertexArray->size() / 2; i = i + 12){
	
		
		p[0] = vec3(vertexArray->at(i)    , vertexArray->at(i + 1), vertexArray->at(i + 2));
		p[1] = vec3(vertexArray->at(i + 4), vertexArray->at(i + 5), vertexArray->at(i + 6));
		p[2] = vec3(vertexArray->at(i + 8), vertexArray->at(i + 9), vertexArray->at(i + 10));

		
		polygons.insert(polygons.end(), BSPTreePolygon(p[0], p[1], p[2]));
	}
	
	time_points.push_back(time_item(clock(), "after copying to BSPTreePolygons.\n"+std::to_string(polygons.size())+" polygons created."));
		

	
	RootNode = new BSPTreeCreatorNode(polygons, this);
	
	time_points.push_back(time_item(clock(), "after creation of RootNode"));
	

	std::cout << "generating BSP Tree\n";
	//create tree from root node
	generateBSPTree(RootNode);
	node_count = NodeSet.size();

	time_points.push_back(time_item(clock(), "after generation of BSPTreeCreator"));
	
	
	placePortals();
	
	time_points.push_back(time_item(clock(), "after Portal placing"));
	portals_inserted = PortalSet.size();
	

	computePVS();
	std::cout << "\n";

	time_points.push_back(time_item(clock(), "after PVS computation"));
	
//	deleteNodePolygons(RootNode);

	this->initVertices(RootNode);

	time_points.push_back(time_item(clock(), "after converting back to vertices"));


	savePVS();  //save bsp too

	time_points.push_back(time_item(clock(), "after Saving"));


	control->runReport->append("portals generated "+ std::to_string(portalsNumber) +" portals inserted "+std::to_string(portals_inserted)+"\n", true);

	std::stringstream string;
	for(uint i = 0; i < time_points.size(); i++){
		string << std::setw(6) << "At time: " << time_points.at(i).seconds(time_points.at(0)) << "s " << time_points.at(i).comment << std::endl; 

		if(i > 0){
			float time_taken = time_points.at(i).seconds(time_points.at(0)) - time_points.at(i - 1).seconds(time_points.at(0));
			float overall_time = time_points.back().seconds(time_points.at(0));

			string << "Time taken: " << time_taken << "s"  <<", percent of time taken: "  << std::fixed << std::setprecision(2) << time_taken / overall_time * 100 << "%" << std::endl << std::endl;
		} else {
			string << std::endl;
		}
	}

	control->runReport->append(string.str(), true);
}


BSPTreeCreator::~BSPTreeCreator(void)
{

	//dont bother traversing tree.. just delete it from NodeSet, because I can..
	for(uint i = 0; i < NodeSet.size(); i++){
		delete NodeSet[i];	
	}

	std::cout << "BSPTreeCreator destructed\n";
}



/*
This merges and deletes portals that are overlapping or touching edges, leaving only
single portal in place of several. One hole one portal.
Reamining portals are added to PortalSet of tree.
*/
void BSPTreeCreator::mergeOverlappingPortals(){

	for(uint i = 0; i < NodeSet.size(); i++){//for all nodes with non zero portal set
		BSPTreeCreatorNode *node = NodeSet[i];

	if(node->PortalSet.size() > 0){
		

		for(uint a = 0; a < node->PortalSet.size(); a++){ //for all combination of portals..
		for(uint b = 0; b < node->PortalSet.size(); b++){
			
		


			if(a == b || node->PortalSet[a] == nullptr || node->PortalSet[b] == nullptr)
				continue;

				
			if(node->PortalSet[a]->shareSpace(node->PortalSet[a], node->PortalSet[b]) || node->PortalSet[a]->touch(node->PortalSet[a], node->PortalSet[b]))
				node->mergePortals(node->PortalSet[a], node->PortalSet[b]);
		
		}
		}

		auto it = std::find(node->PortalSet.begin(), node->PortalSet.end(), nullptr);
		
		while(it != node->PortalSet.end()){
			node->PortalSet.erase(it);
			it = std::find(node->PortalSet.begin(), node->PortalSet.end(), nullptr);		
		}

		PortalSet.insert(PortalSet.end(), node->PortalSet.begin(), node->PortalSet.end());

	}
	}

	
	
}


//deletes unnecesary polygon in non-leaf nodes
void BSPTreeCreator::deleteNodePolygons(BSPTreeCreatorNode *node){

	if(node->isLeaf())
		return;
	
	//std::cout << node->PolygonSet.size() << std::endl;
	node->PolygonSet.clear();


		deleteNodePolygons(node->LeftChild);
		deleteNodePolygons(node->RightChild);
	
}


/*
Merges the portals into one and the other is nullptr.
Must overlap or touch edges..
Constructs square portal in place of first and nullptr in place of other.
*/
void BSPTreeCreatorNode::mergePortals(BSPTreePolygon *&portal1, BSPTreePolygon *&portal2){

	/*
	1---2
	| \ |
	0---3
	*/

	//some existing point values
	float xmax = portal1->p[0].x;
	float ymax = portal1->p[0].y;
	float zmax = portal1->p[0].z;

	float xmin = portal1->p[0].x;
	float ymin = portal1->p[0].y;
	float zmin = portal1->p[0].z;
	
	//get extremes for both portals
	for(uint j = 0; j <  portal1->p.size(); j++){

		xmax = max(portal1->p[j].x, xmax);
		ymax = max(portal1->p[j].y, ymax);
		zmax = max(portal1->p[j].z, zmax);

		xmin = min(portal1->p[j].x, xmin);
		ymin = min(portal1->p[j].y, ymin);
		zmin = min(portal1->p[j].z, zmin);

	}
	
	for(uint j = 0; j <  portal2->p.size(); j++){

		xmax = max(portal2->p[j].x, xmax);
		ymax = max(portal2->p[j].y, ymax);
		zmax = max(portal2->p[j].z, zmax);

		xmin = min(portal2->p[j].x, xmin);
		ymin = min(portal2->p[j].y, ymin);
		zmin = min(portal2->p[j].z, zmin);

	}


	
	portal1->p.clear();

	portal1->p.push_back(vec3(xmin, ymin, zmax));
	portal1->p.push_back(vec3(xmin, ymax, zmax));
	portal1->p.push_back(vec3(xmax, ymax, zmin));
	portal1->p.push_back(vec3(xmax, ymin, zmin));

	portal1->computeNormal();

	delete portal2;
	portal2 = nullptr;

}

/*
If both children are nullptr, the node is leaf.
*/
/*bool BSPTreeCreatorNode::isLeaf(){

	return (this->LeftChild == nullptr && this->RightChild == nullptr);
}
*/
/*
Saves
	//Id of node
	//Id of children - 0 if none
	//Divider polygon // or default values - 0 0 0
	//Ids of immediate neighbours
	//Ids of PVNodes
	//Colors
	//No of Polygons and No of Portals
	//Ids of immediate neighbours connected with portals
	//vertices
*/
bool BSPTreeCreator::savePVS(){


using namespace std;

cout << "saving PVS\n" << endl;

	fstream file;
	file.open(name, ios::out);

	if(not file.is_open()){
		cout << "error opening  " << name << " for writing.";
	return false;
	}

	for(uint i = 0; i < NodeSet.size(); i++){

		//Id of node
		file << NodeSet.at(i)->Id << endl;

		//Id of children - 0 if none
		if(NodeSet.at(i)->isLeaf()){
			file << "0 0" << endl;
		} else {
			file << NodeSet.at(i)->LeftChild->Id << " " << NodeSet.at(i)->RightChild->Id << endl;
		}

		//Divider polygon // or default values - 0 0 0
		file << NodeSet.at(i)->Divider.p.at(0).x << " " << NodeSet.at(i)->Divider.p.at(0).y << " " << NodeSet.at(i)->Divider.p.at(0).z << endl;
		file << NodeSet.at(i)->Divider.p.at(1).x << " " << NodeSet.at(i)->Divider.p.at(1).y << " " << NodeSet.at(i)->Divider.p.at(1).z << endl;
		file << NodeSet.at(i)->Divider.p.at(2).x << " " << NodeSet.at(i)->Divider.p.at(2).y << " " << NodeSet.at(i)->Divider.p.at(2).z << endl;


		//Ids of immediateNeighbours
		for(uint j = 0; j < NodeSet[i]->immediateNeighbours.size(); j++){
			file << NodeSet.at(i)->immediateNeighbours[j];

			if(j != NodeSet[i]->immediateNeighbours.size() - 1){
				file << " ";
			} else file << endl;
		}


		//Ids of PVNodes
		for(uint j = 0; j < NodeSet[i]->PVNodes.size(); j++){
			file << NodeSet.at(i)->PVNodes[j];

			if(j != NodeSet[i]->PVNodes.size() - 1){
				file << " ";
			} else file << endl;
		}

		//Colors
		file << NodeSet.at(i)->red << " " << NodeSet.at(i)->green << " " << NodeSet.at(i)->blue << endl;
	
		if(NodeSet.at(i)->isLeaf() && (NodeSet.at(i)->PolygonSet.size() + NodeSet.at(i)->PortalSet.size() * 2) * 3 != NodeSet.at(i)->vertices.size())
			Sleep(1);



		//No of Polygons and No of Portals
		if(NodeSet.at(i)->isLeaf()){
			file << NodeSet.at(i)->PolygonSet.size() << " " << NodeSet.at(i)->PortalSet.size() * 2 << endl;   //!!! PortalSet has portals as squares - 2 triangles 
		}else{ 
			file << "0 0\n";
		}

		//Ids of immediate neighbours connected with portals
		for(unsigned int p = 0; p < NodeSet.at(i)->PortalSet.size(); p++){

			file << NodeSet.at(i)->PortalSet.at(p)->connectedId;

			if(p != NodeSet.at(i)->PortalSet.size() - 1){
				file << " ";
			} else file << endl;
		}

		//vertices
		for(uint k = 0; k < NodeSet.at(i)->vertices.size(); k++){
					
			file << NodeSet.at(i)->vertices.at(k)[0] << " ";
			file << NodeSet.at(i)->vertices.at(k)[1] << " ";
			file << NodeSet.at(i)->vertices.at(k)[2] << endl;
		
		}

		file << endl << endl;
	}

	//cout << "PVS saved" << endl;
file.close();
return true;
}



/*
implements adding the node and returning its id in NodeSet.
*/
int BSPTreeCreator::addMe(BSPTreeCreatorNode *node){

	int id;

	NodeSet.push_back(node);
	id = NodeSet.size() - 1;

return id;
}

/*
Traverses tree. Calls initVertices for every node.
*/
void BSPTreeCreator::initVertices(BSPTreeCreatorNode *node){

	if(node->isLeaf()){
		node->initVertices();
	return;
	}

		initVertices(node->RightChild);
		initVertices(node->LeftChild);

}

/*
Duplicates vertices to format them for drawing from BSPTreePolygons to vertices vector.
Puts points from PolygonSet and PortalSet to vertices.
+ generates come color based on node Id
PRESUMES triangles in polygonset and squares in portalset
*/
void BSPTreeCreatorNode::initVertices(){

	//toto je dost kostrbate.. 
	vertices.reserve((PolygonSet.size() + PortalSet.size() * 2) * 3);

	for(uint i = 0; i < PolygonSet.size(); i++){

		for(uint j = 0; j < PolygonSet.front().p.size(); j++){
		vertices.insert(vertices.end(), PolygonSet[i].p[j]);
		}
	}

	

	//generate color
	red = this->Id * 0.703f;
	green = this->Id * 0.305f;
	blue = this->Id * this->Id * 0.251f;
	
	//get whole part
	int ired = red;
	int igreen = green;
	int iblue = blue;

	red = red - ired;
	green = green - igreen;
	blue = blue - iblue;


	/*
	int number = rand() % 100; //some random offset to make portal stand out..
	number -= 50;
	number += this->Id;
	*/
//add portals
	for(uint i = 0; i < PortalSet.size(); i++){

		vertices.push_back(PortalSet.at(i)->p.at(3) /*- vec3(number * 0.002f, number * 0.002f, -number * 0.002f)*/);
		vertices.push_back(PortalSet.at(i)->p.at(1) /*- vec3(number * 0.002f, number * 0.002f, -number * 0.002f)*/);
		vertices.push_back(PortalSet.at(i)->p.at(2) /*- vec3(number * 0.002f, number * 0.002f, -number * 0.002f)*/);
		
		vertices.push_back(PortalSet.at(i)->p.at(3) /*- vec3(number * 0.002f, number * 0.002f, -number * 0.002f)*/);
		vertices.push_back(PortalSet.at(i)->p.at(0) /*- vec3(number * 0.002f, number * 0.002f, -number * 0.002f)*/);
		vertices.push_back(PortalSet.at(i)->p.at(1) /*- vec3(number * 0.002f, number * 0.002f, -number * 0.002f)*/);

	}


}





/*
IS-CONVEX-SET
Indata:
PolygonSet – The set of polygons to check for convexity
Outdata:
Whether the set is convex or not
Effect:
Checks each polygon against each other polygon, to see if they are
in front of each other, if any two polygons don’t fulfill that
criteria the set isn’t convex.

IS-CONVEX-SET (PolygonSet)
1 for i f 0 to PolygonSet.Length ()
2 for j f 0 to PolygonSet.Length ()
3 if(i <> j && not POLYGON-INFRONT(PolygonSet[i], PolygonSet[j]))
4 then return false
5 return true
*/
bool BSPTreeCreatorNode::isConvexSet(){

	for(unsigned int i = 0; i < this->PolygonSet.size(); i++){
	for(unsigned int j = 0; j < this->PolygonSet.size(); j++){
			
		if( i != j && not this->PolygonSet[i].polygonInfront(this->PolygonSet[j])) 
			return false;
	}
	}

return true;
}




/*
CHOOSE-DIVIDING-POLYGON
Indata:
PolygonSet – The set of polygons to search for the best dividing
polygon.
Outdata:
The best dividing polygon
Effect:
Searches through the set of polygons and returns the polygons that
splits the set into the two best resulting sets. If the set is
convex no polygon can be returned.


CHOOSE-DIVIDING-POLYGON (PolygonSet)
1 if (IS-CONVEX-SET (PolygonSet))
2 then return NOPOLYGON
3 MinRelation f MINIMUMRELATION
4 BestPolygon f NOPOLYGON
5 LeastSplits f INFINITY
6 BestRelation f 0
Loop to find the polygon that best divides the set.
7 while(BestPolygon = NOPOLYGON)
8 for each polygon P1 in PolygonSet
9 if (basicPolygon P1 has not been used as divider previously
during the creation of the tree)
Count the number of polygons on the positive side, negative side
of and spanning the plane defined by the current polygon.
10 NumPositive f 0, NumNegative f 0, NumSpanning f 0
11 for each polygon P2 in PolygonSet except P1
12 Value = CALCULATE-SIDE(P1, P2)
13 if(Value = INFRONT)
14 NumPositive = NumPositive + 1
15 else if(Value = BEHIND)
16 NumNegative = NumNegative + 1
17 else if(Value = SPANNING)
18 NumSpanning = NumSpanning + 1
Calculate the relation between the number of polygons in the two
sets divided by the current polygon.
19 if (NumPositive < NumNegative)
20 Relation = NumPositive / NumNegative
21 else
22 Relation = NumNegative / NumPositive

Compare the results given by the current polygon to the best this
far. If the this polygon splits fewer polygons and the relation
between the resulting sets is acceptable this is the next candidate
polygon. If the current polygon splits the same amount of polygons
as the best polygon this far and the relation between the two
resulting sets is better -> this polygon is the new candidate
polygon.

23 if (Relation > MinRelation &&(NumSpanning < LeastSplits ||(NumSpanning = LeastSplits &&Relation > BestRelation))
24 BestPolygon f P1
25 LeastSplits f NumSpanning
26 BestRelation f Relation
Decrease the number least acceptable relation by dividing it with
a predefined constant.
27 MinRelation f MinRelation / MINRELATIONSCALE
28 return BestPolygon

Most time consuming part of BSPtree generation.
Must categorize all polygons in current PolygonSet at least once until relation is sufficient.
For bigger sets it gets more time consuming..
*/
int BSPTreeCreatorNode::chooseBestDividingPolygon(){

/*
	if(PolygonSet.size() == 4)
		Sleep(1);
*/

	if(this->isConvexSet())
		return NOPOLYGON;

float minRelation = 0.8f;
int bestPolygon = NOPOLYGON;
int leastSplits = INT_MAX;
float bestRelation = 0.0f;
float minRelationScale = 2.0f;
float relation = 0.0f;




//skip some polygons.. add 2 to each index - doesn't miss anything.. every wall has 2 triangles in the same plane..

	while(bestPolygon == NOPOLYGON){
		for(unsigned int i = 0; i < this->PolygonSet.size(); i = i + 2){
	


			if(this->PolygonSet[i].usedAsDivider)
				continue;
			
			
			int positive = 0, negative = 0, spanning = 0;

			for(unsigned int j = 0; j < this->PolygonSet.size(); j = j + 2){ 
				if(i == j)// dont compare with itself
					continue;

				int value = this->PolygonSet[i].calculateSide(this->PolygonSet[j]);

				if(value == INFRONT || value == COINCIDING){//
					positive++;
				} else if (value == BEHIND){
					negative++;
				} else if (value == SPANNING){
					spanning++;
				}

				

			}// for j
						
			if(positive < negative){
				relation = (float) positive / (float) negative;
			} else {
				relation = (float) negative / (float) positive;
			}
			
			//////////////end of job, returns relation
			//choose best returned relation

			if(relation >= minRelation && //has acceptable relation AND
						(spanning < leastSplits || 
								(spanning == leastSplits && relation > bestRelation))){ //has fewer splits or the same and better relation
				bestPolygon = i;
				leastSplits = spanning;
				bestRelation = relation;
			}
		
		} // for i
		minRelation = minRelation / minRelationScale; // if unsuccessful, lower acceptable relation

	} // while NOPOLYGON
	dividers++;
return bestPolygon;
}





/*
GENERATE-BSP-TREE
Indata:
Node – The sub tree to build of the type BSPTreeCreatorNode.
PolygonSet – The set of polygons to create a BSP-tree from.
Outdata:
A BSP-tree stored in the incoming node.
Effect:
Generates a BSP-tree out of a set of polygons.


GENERATE-BSP-TREE (Node, PolygonSet)
1 if (IS-CONVEX-SET (PolygonSet))
2 Tree f BSPTreeCreatorNode (PolygonSet)
3 Divider f CHOOSE-DIVIDING-POLYGON (PolygonSet)
4 PositiveSet f {}
5 NegativeSet f {}
6 for each polygon P1 in PolygonSet
7 Value f CALCULATE-SIDE (Divider, P1)
8 if(Value = INFRONT)
9 PositiveSet f PositiveSet U P1
10 else if (Value = BEHIND)
11 NegativeSet f NegativeSet U P1
12 else if (Value = SPANNING)
13 Split_Polygon10 (P1, Divider, Front, Back)
14 PositiveSet f PositiveSet U Front
15 NegativeSet f NegativeSet U Back
16 GENERATE-BSP-TREE (Tree.RightChild, PositiveSet)
17 GENERATE-BSP-TREE (Tree.LeftChild, NegativeSet)


*/


void BSPTreeCreator::generateBSPTree(BSPTreeCreatorNode *Node){

	//it's leaf; if it's convex, there's no point in dividing it
	if(Node->isConvexSet()){
		std::cout << "\rleaf             ";
		leaf++;
		Node->computeExtremes();
		return;
	}

	int divider = Node->chooseBestDividingPolygon();

	Node->PolygonSet[divider].usedAsDivider = true;
	Node->Divider = Node->PolygonSet[divider];//copy it to node

	std::cout << "\rdivider chosen      ";

	std::vector<BSPTreePolygon> positiveSet;
	std::vector<BSPTreePolygon> negativeSet;

	for(unsigned int i = 0; i < Node->PolygonSet.size(); i++){
		int side = Node->PolygonSet[divider].calculateSide(Node->PolygonSet[i]);

		if(side == INFRONT){
			positiveSet.insert(positiveSet.end(), Node->PolygonSet[i]);
		
		} else if(side == BEHIND) {
			negativeSet.insert(negativeSet.end(), Node->PolygonSet[i]);

		} else if(side == SPANNING) {
			//SPLIT POLYGON
			std::vector<vec3> front, back;

			Node->PolygonSet[i].splitPolygon(Node->PolygonSet[divider], front, back);

			if(not front.empty())
				positiveSet.insert(positiveSet.end(), BSPTreePolygon(front));
				
			if(not back.empty())
				negativeSet.insert(negativeSet.end(), BSPTreePolygon(back));
			

		} else if (side == COINCIDING){  //Divider polygon; put it to set it's facing - should be always positiveSet
			//int facing = Node->PolygonSet[i].classifyPoint(Node->PolygonSet[i].p[0] + Node->PolygonSet[i].normal);

			//if(facing == INFRONT){
				positiveSet.insert(positiveSet.end(), Node->PolygonSet[i]);
				//printf("front\n");
			//} else if(facing == BEHIND){
			//	negativeSet.insert(negativeSet.end(), Node->PolygonSet[i]);
				//printf("behind\n");
			//}
		}
	
	}

	//add children; 
	Node->RightChild = new BSPTreeCreatorNode(positiveSet, Node->Tree);
	Node->LeftChild = new BSPTreeCreatorNode(negativeSet, Node->Tree);
	std::cout << "\rnode             ";


	
generateBSPTree(Node->RightChild);
generateBSPTree(Node->LeftChild);
}

///leave it for later.. previous one works..
//Creates divider polygon extended to bounding box.
//Figures in which direction of bounding box is divider looking and computes intersection
//points with sides perpendicular to direction of divider plane.
//Allocates new BSPTreePolygon and returns it.
BSPTreePolygon *BSPTreeCreatorNode::getPortal2(){
	
	//node bounding box
	for(uint i = 0; i < this->PolygonSet.size(); i++){
	
		for(uint j = 0; j < 3; j++){

			if(this->PolygonSet[i].p[j].x > this->xmax)
				this->xmax = this->PolygonSet[i].p[j].x;

			if(this->PolygonSet[i].p[j].x < this->xmin)
				this->xmin = this->PolygonSet[i].p[j].x;

			if(this->PolygonSet[i].p[j].y > this->ymax)
				this->ymax = this->PolygonSet[i].p[j].y;

			if(this->PolygonSet[i].p[j].y < this->ymin)
				this->ymin = this->PolygonSet[i].p[j].y;

			if(this->PolygonSet[i].p[j].z > this->zmax)
				this->zmax = this->PolygonSet[i].p[j].z;

			if(this->PolygonSet[i].p[j].z < this->zmin)
				this->zmin = this->PolygonSet[i].p[j].z;

		}
	
	}
	//make it slightly bigger
	this->xmax += 0.5f;
	this->ymax += 0.5; // and more easily observable
	this->zmax += 0.5f;
	this->xmin -= 0.5f;
	this->ymin -= 0.5f;
	this->zmin -= 0.5f;
	
		//idea
		///////we have 12 edges....
		///for each
		///if A && B are on opposite sides of divider plane
		///compute intersection
		////have now 3 to 6 intersection points
		///how to ensure winding order??


		///suggestion
		//find 2 which are on the same face of box - insert to portalPolygon
		//while points not empty
			//find 2 which are on the same face of bounding box AND one of them is last point in portalPolygon


	///practical try

		struct face{
			BSPTreePolygon side;
			std::vector<vec3> intersection;
		
		};

		struct box {
			face top;
			face left;
			face right;
			face bottom;
			face front;
			face back;		
		};





		/*
		//front side point
		facing = this->Divider.calculateSide(BSPTreePolygon(vec3(this->xmin, this->ymin, this->zmax), vec3(this->xmax, this->ymin, this->zmax), vec3(0.0f, this->ymax, this->zmax))); 

		//faces front or back - go around from bottom left to right to top to left
		if(facing == INFRONT || facing == BEHIND) { 
			
			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmin, this->ymin, this->zmin), vec3(this->xmin, this->ymin, this->zmax));
			points.push_back(intersectionPoint);

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmax, this->ymin, this->zmin), vec3(this->xmax, this->ymin, this->zmax));
			points.push_back(intersectionPoint);

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmax, this->ymax, this->zmin), vec3(this->xmax, this->ymax, this->zmax));
			points.push_back(intersectionPoint);

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmin, this->ymax, this->zmin), vec3(this->xmin, this->ymax, this->zmax));
			points.push_back(intersectionPoint);

			if(facing == INFRONT)
				std::reverse(points.begin(), points.end()); 
		} 
		*/
/*
		BSPTreePolygon *portal = new BSPTreePolygon(points[0], points[1], points[2]);
		portal->p.push_back(points[3]);

		this->Divider.usedAsPortal = true;
		portals++;
		*/
//return portal;
		return nullptr;
}


//Creates divider polygon extended to bounding box.
//Figures in which direction of bounding box is divider looking and computes intersection
//points with sides perpendicular to direction of divider plane.
//Allocates new BSPTreePolygon and returns it.
BSPTreePolygon *BSPTreeCreatorNode::getPortal(){
	
	//some existing point values
		xmax = PolygonSet[0].p[0].x;
		ymax = PolygonSet[0].p[0].y;
		zmax = PolygonSet[0].p[0].z;

		xmin = PolygonSet[0].p[0].x;
		ymin = PolygonSet[0].p[0].y;
		zmin = PolygonSet[0].p[0].z;

//node bounding box
	for(uint i = 0; i < this->PolygonSet.size(); i++){
	
		for(uint j = 0; j < PolygonSet[i].p.size(); j++){


			xmax = max(PolygonSet[i].p[j].x, xmax);
			ymax = max(PolygonSet[i].p[j].y, ymax);
			zmax = max(PolygonSet[i].p[j].z, zmax);

			xmin = min(PolygonSet[i].p[j].x, xmin);
			ymin = min(PolygonSet[i].p[j].y, ymin);
			zmin = min(PolygonSet[i].p[j].z, zmin);

		}
	
	}

	//make it slightly bigger
	this->xmax += 0.5f;
	this->ymax += 0.0f; // and more easily observable
	this->zmax += 0.5f;
	this->xmin -= 0.5f;
	this->ymin -= 0.5f; //this
	this->zmin -= 0.5f;
	
		vec3 intersectionPoint;
		std::vector<vec3> points;


		//idea
		///////we have 12 edges....
		///for each
		///if A && B are on opposite sides of divider plane
		///compute intersection
		////have now 3 to 6 points
		///how to ensure winding order??


		///suggestion
		//find 2 which are on the same face of box - insert to portalPolygon
		//while points not empty
			//find 2 which are on the same face of bounding box AND one of them is last point in portalPolygon

/*
	///practical try

		struct face{
			BSPTreePolygon side;
			std::vector<vec3> intersection;
		
		};

		struct box {
			face top;
			face left;
			face right;
			face bottom;
			face front;
			face back;		
		};


		*/





		//simplified - not arbitrary direction

		//right side point
		int facing = this->Divider.calculateSide(BSPTreePolygon(vec3(this->xmax, this->ymin, this->zmax), vec3(this->xmax, this->ymin, this->zmin), vec3(this->xmax, this->ymax, 0.0f)));  
		
		//faces right or left - go around from bottom front to back to top to front
		if(facing == INFRONT || facing == BEHIND) { 
			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmin, this->ymin, this->zmax), vec3(this->xmax, this->ymin, this->zmax));
			points.push_back(intersectionPoint);

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmin, this->ymin, this->zmin), vec3(this->xmax, this->ymin, this->zmin));
			points.push_back(intersectionPoint);

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmin, this->ymax, this->zmin), vec3(this->xmax, this->ymax, this->zmin));
			points.push_back(intersectionPoint);

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmin, this->ymax, this->zmax), vec3(this->xmax, this->ymax, this->zmax));
			points.push_back(intersectionPoint);

			if(facing == INFRONT) //correct winding to compute normal with correct direction
				std::reverse(points.begin(), points.end()); 
		}

		//up side point
		facing = this->Divider.calculateSide(BSPTreePolygon(vec3(this->xmin, this->ymax, this->zmax), vec3(this->xmax, this->ymax, this->zmax), vec3(0.0f, this->ymax, this->zmin))); 

		//faces up or down - go around from left front to right to back to left
		if(facing == INFRONT || facing == BEHIND) { 

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmin, this->ymax, this->zmax), vec3(this->xmin, this->ymin, this->zmax));
			points.push_back(intersectionPoint);

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmax, this->ymax, this->zmax), vec3(this->xmax, this->ymin, this->zmax));
			points.push_back(intersectionPoint);

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmax, this->ymax, this->zmin), vec3(this->xmax, this->ymin, this->zmin));
			points.push_back(intersectionPoint);

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmin, this->ymax, this->zmin), vec3(this->xmin, this->ymin, this->zmin));
			points.push_back(intersectionPoint);

			if(facing == INFRONT)
				std::reverse(points.begin(), points.end()); 
		} 

		//front side point
		facing = this->Divider.calculateSide(BSPTreePolygon(vec3(this->xmin, this->ymin, this->zmax), vec3(this->xmax, this->ymin, this->zmax), vec3(0.0f, this->ymax, this->zmax))); 

		//faces front or back - go around from bottom left to right to top to left
		if(facing == INFRONT || facing == BEHIND) { 
			
			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmin, this->ymin, this->zmin), vec3(this->xmin, this->ymin, this->zmax));
			points.push_back(intersectionPoint);

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmax, this->ymin, this->zmin), vec3(this->xmax, this->ymin, this->zmax));
			points.push_back(intersectionPoint);

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmax, this->ymax, this->zmin), vec3(this->xmax, this->ymax, this->zmax));
			points.push_back(intersectionPoint);

			intersectionPoint = this->Divider.planeVectorIntersection(vec3(this->xmin, this->ymax, this->zmin), vec3(this->xmin, this->ymax, this->zmax));
			points.push_back(intersectionPoint);

			if(facing == INFRONT)
				std::reverse(points.begin(), points.end()); 
		} 

		BSPTreePolygon *portal = new BSPTreePolygon(points[0], points[1], points[2]);
		portal->p.push_back(points[3]);

		this->Divider.usedAsPortal = true;
		portalsNumber++;

return portal;
}



//should remove polygons covering only wall
BSPTreePolygon *BSPTreeCreatorNode::removeCoincidingParts(BSPTreePolygon *&portalPolygon){

	
	if(PolygonSet.size() == 0)
		return portalPolygon;


	if(portalPolygon == nullptr)
		return nullptr;

	if(portalPolygon->p.size() != 4)
		return nullptr;

	std::vector<BSPTreePolygon> polygons;
		
					
		for(uint i = 0; i < PolygonSet.size(); i++){
			
			if(PolygonSet[i].calculateSide(*portalPolygon) == COINCIDING){	
				polygons.push_back(PolygonSet[i]);
			}
		}

		if(polygons.size() == 0)
			return portalPolygon;

		//////arbitrary constants.. bad idea..!!!
float xmax = -200.0f, xmin = 200.0f, zmax = -200.0f, zmin = 200.0f;
float pxmax = -200.0f, pxmin = 200.0f, pzmax = -200.0f, pzmin = 200.0f;  // portal extremes

		//get extremes of coinciding set
		for(uint i = 0; i < polygons.size(); i++){
								
				for(uint j = 0; j < 3; j++){
					if(xmax <= polygons[i].p[j].x)
						xmax = polygons[i].p[j].x;

					if(xmin >= polygons[i].p[j].x)
						xmin = polygons[i].p[j].x;

					if(zmax <= polygons[i].p[j].z)
						zmax = polygons[i].p[j].z;
										
					if(zmin >= polygons[i].p[j].z)
						zmin = polygons[i].p[j].z;
				}
			
		}



			for(uint j = 0; j < 4; j++){
					if(pxmax < portalPolygon->p[j].x)
						pxmax = portalPolygon->p[j].x;

					if(pxmin > portalPolygon->p[j].x)
						pxmin = portalPolygon->p[j].x;

					if(pzmax < portalPolygon->p[j].z)
						pzmax = portalPolygon->p[j].z;
										
					if(pzmin > portalPolygon->p[j].z)
						pzmin = portalPolygon->p[j].z;
				}
/*
			if(pxmin <= 11.5f && pxmax >= 13.5f  && pzmin == pzmax && pzmax == -2.5f){
			pzmin = pzmin;
			
			
			}

			*/
		if(xmax == xmin && pxmax == pxmin){ //constant X


		

			for(uint i = 0; i < polygons.size(); i++){//for all potentially coinciding polygons

			float minZ = 200.0f, maxZ = -200.0f;
			float pminZ = 200.f, pmaxZ = -200.0f;
			float avg, pavg;

				for(uint j = 0; j < 3; j++){

					if(polygons.at(i).p.at(j).z >= maxZ)
						maxZ = polygons.at(i).p.at(j).z;

					if(polygons.at(i).p.at(j).z <= minZ)
						minZ = polygons.at(i).p.at(j).z;
				}
			

				for(uint j = 0; j < 4; j++){

					if(portalPolygon->p.at(j).z >= pmaxZ)
						pmaxZ = portalPolygon->p.at(j).z;

					if(portalPolygon->p.at(j).z <= pminZ)
						pminZ = portalPolygon->p.at(j).z;
				}

				avg = (maxZ + minZ) / 2.0f;
				pavg = (pmaxZ + pminZ) / 2.0f;

				if(pmaxZ == pminZ){
					delete portalPolygon;
					return nullptr;
				}

				if(avg < pavg){ // polygon is on the left
				
					for(uint j = 0; j < 4; j++){

						if(portalPolygon->p.at(j).z <= maxZ)
							portalPolygon->p.at(j).z = maxZ;
					}

				} else if (avg > pavg){
				
					for(uint j = 0; j < 4; j++){

						if(portalPolygon->p.at(j).z >= minZ)
							portalPolygon->p.at(j).z = minZ;
					}
				
				} else if(avg == pavg){
				
					for(uint j = 0; j < 4; j++){
						if(pmaxZ <= maxZ + 0.01f && pminZ >= minZ - 0.01f)
							portalPolygon->p.at(j).z = maxZ;
					}
				}
				
				///////////////////////////portal wall portal
				// portal wall wall 
					if(pzmax > zmax){
						for(uint j = 0; j < 4; j++){
							if(portalPolygon->p.at(j).z <= maxZ)
								portalPolygon->p.at(j).z = maxZ;
						
						}
					}
			}

			if(portalPolygon->p.at(0).z == portalPolygon->p.at(1).z && portalPolygon->p.at(0).z == portalPolygon->p.at(2).z){
				delete portalPolygon;
				portalPolygon = nullptr;
				return nullptr;
			}
		


		} else if(zmax == zmin && pzmax == pzmin){ //constant Z


			for(uint i = 0; i < polygons.size(); i++){//for all potentially coinciding polygons
			float minX = 200.0f, maxX = -200.0f;
			float pminX = 200.f, pmaxX = -200.0f;
			float avg, pavg;

				for(uint j = 0; j < 3; j++){

					if(polygons.at(i).p.at(j).x >= maxX)
						maxX = polygons.at(i).p.at(j).x;

					if(polygons.at(i).p.at(j).x <= minX)
						minX = polygons.at(i).p.at(j).x;
				}
			

				for(uint j = 0; j < 4; j++){

					if(portalPolygon->p.at(j).x >= pmaxX)
						pmaxX = portalPolygon->p.at(j).x;

					if(portalPolygon->p.at(j).x <= pminX)
						pminX = portalPolygon->p.at(j).x;
				}

				avg = (maxX + minX) / 2.0f;
				pavg = (pmaxX + pminX) / 2.0f;

				if(avg < pavg){ // polygon is on the left
				
					for(uint j = 0; j < 4; j++){

						if(portalPolygon->p.at(j).x <= maxX)
							portalPolygon->p.at(j).x = maxX;
					}

				} else if (avg > pavg){
				
					for(uint j = 0; j < 4; j++){

						if(portalPolygon->p.at(j).x >= minX)
							portalPolygon->p.at(j).x = minX;
					}
				
				} else if (avg == pavg){
					
					//portal fully coincides with this one wall
					for(uint j = 0; j < 4; j++){
						if(pmaxX <= maxX + 0.01f && pminX >= minX - 0.01f)
							portalPolygon->p.at(j).x = minX;
						
							//portalPolygon->p.at(j).x = minX;
					}

					// portal wall wall 
					if(pxmin < xmin){
						for(uint j = 0; j < 4; j++){
							if(portalPolygon->p.at(j).x >= minX)
								portalPolygon->p.at(j).x = minX;
						
						}
					}



				}
				
			
			}


			if(portalPolygon->p.at(0).x == portalPolygon->p.at(1).x && portalPolygon->p.at(0).x == portalPolygon->p.at(2).x){
				delete portalPolygon;
				portalPolygon = nullptr;
				return nullptr;
			}
		} else {
		
		Sleep(1);
		}
			
return portalPolygon;
}


//function specific to portalpolygon
//splits in case when portal polygon covers entrance and some wall too
bool BSPTreeCreatorNode::splitOnHoles(BSPTreePolygon *portalPolygon){

	//////should rework this
	///detect hole ideally independently of orientation
	//split
	//portal wall portal scenario - same center, but different lengths - portal spans polygonset
	////hole in the middle
	//what about there are multiple rows or coinciding mesh of triangles..??

	//compute orientation
	if(PolygonSet.size() == 0)
		return true;


	if(portalPolygon == nullptr)
		return true;

	if(portalPolygon->p.size() != 4)
		return true;


	std::vector<BSPTreePolygon> polygons;
		
	//get set of polygons coinciding with portalpolygon -- due to convexity of node's PolygonSet its either empty or some polygons are coinciding
	for(uint i = 0; i < PolygonSet.size(); i++){
			
		if(PolygonSet[i].calculateSide(*portalPolygon) == COINCIDING && PolygonSet[i].shareSpace(portalPolygon, &PolygonSet[i])){	// AND share space...
			polygons.push_back(PolygonSet[i]);
		}
	}

	if(polygons.size() == 0)
		return false;


	//some existing point values
	float xmax = polygons[0].p[0].x;
	float zmax = polygons[0].p[0].z;

	float xmin = polygons[0].p[0].x;
	float zmin = polygons[0].p[0].z;

	//extremes of coinciding set
	for(uint i = 0; i < polygons.size(); i++){
	
		for(uint j = 0; j < polygons[i].p.size(); j++){

			xmax = max(polygons[i].p[j].x, xmax);
			zmax = max(polygons[i].p[j].z, zmax);

			xmin = min(polygons[i].p[j].x, xmin);
			zmin = min(polygons[i].p[j].z, zmin);

		}
	
	}


//some existing point values
	float pxmax = portalPolygon->p[0].x;
	float pzmax = portalPolygon->p[0].z;

	float pxmin = portalPolygon->p[0].x;
	float pzmin = portalPolygon->p[0].z;

	//extremes of portal
	for(uint j = 0; j < portalPolygon->p.size(); j++){

		pxmax = max(portalPolygon->p[j].x, pxmax);
		pzmax = max(portalPolygon->p[j].z, pzmax);

		pxmin = min(portalPolygon->p[j].x, pxmin);
		pzmin = min(portalPolygon->p[j].z, pzmin);

	}
	
	
		
		float avgZ = (zmax + zmin) / 2.0f;
		float pavgZ = (pzmax + pzmin) / 2.0f;
		float avgX = (xmax + xmin) / 2.0f;
		float pavgX = (pxmax + pxmin) / 2.0f;


		if(xmax == xmin && pxmax == pxmin){ //constant X

			//entry point is a special case - has no geometry to clip portal, makes avgZ and pavgZ different; so...
			if(pzmax >= 0.99f)
				pavgZ = avgZ; //eliminated entry point means no need for this




			//portal wall portal scenario - same center, but different lengths - portal spans polygonset
			if(avgZ == pavgZ && pzmin < zmin && pzmax > zmax){
			
			
				BSPTreePolygon *portalLeft = new BSPTreePolygon();
				BSPTreePolygon *portalRight = new BSPTreePolygon();

				*portalLeft = *portalPolygon;
				*portalRight = *portalPolygon;

				//split portalPolygon on edge zmin

					//remove part above zmin
					for(uint i = 0; i < 4; i++){
						if(portalLeft->p[i].z >= zmin)
							portalLeft->p[i].z = zmin;
					}

					//remove part below zmin
					for(uint i = 0; i < 4; i++){
						if(portalRight->p[i].z <= zmin)
							portalRight->p[i].z = zmin;
					}
			
					PortalQueue.push_back(portalLeft);
					PortalQueue.push_back(portalRight);

			return true;
			}


			float polygonWallLength = zmax - zmin;

			////hole in the middle
				if(polygonWallLength * 2 > polygons.size()){  //  1 of lentgth = 2 triangles
					//wall has a hole - where??? - test point traveling along it
					//is inside?

					int side1_index = -1, side2_index = 0;
			
					vec3 point(xmin, 0.1f, zmax - 0.5f); //x doesnt matter, y in the center, zmax - 0.5f on the dividing line between two triangles

					//find indices of sides of hole
					for(uint i = 0; i < polygons.size(); i++){
						point.y *= -1.0f;					//not sure about - on dividing line accurancy

						if(not polygons.at(i).isPointInPolygon(point)){

							if(point.z == (pzmax - pzmin) / 2.0f && (pzmax - pzmin) == 1.0f)
								return false;


							side1_index = i - 1;
							side2_index = i;
						break;
						}

						if(i % 2 == 1)
							point.z -= 1.0f;
					}
					////////////////////// wall portal wall

					if((pzmax <= point.z - 0.5f || pzmin >= point.z + 0.5f) || (pzmin >= point.z - 0.5f && pzmax <= point.z + 0.5f)) //dont split - is fully inside or outside of hole
						return false;




					BSPTreePolygon *portalLeft = new BSPTreePolygon();
					BSPTreePolygon *portalCenter = new BSPTreePolygon();
					BSPTreePolygon *portalRight = new BSPTreePolygon();

					*portalLeft = *portalPolygon;
					*portalCenter = *portalPolygon;
					*portalRight = *portalPolygon;

					

					for(uint i = 0; i < 4; i++){
						if(portalLeft->p[i].z >= point.z - 0.5f)
							portalLeft->p[i].z = point.z - 0.5f;
					}	

					for(uint i = 0; i < 4; i++){
						if(portalCenter->p[i].z >= point.z + 0.5f)
							portalCenter->p[i].z = point.z + 0.5f;

						if(portalCenter->p[i].z <= point.z - 0.5f)
							portalCenter->p[i].z = point.z - 0.5f;
					}

					for(uint i = 0; i < 4; i++){
						if(portalRight->p[i].z <= point.z + 0.5f)
							portalRight->p[i].z = point.z + 0.5f;
					}

				if(portalLeft->p.at(0).z == portalLeft->p.at(1).z && portalLeft->p.at(0).z == portalLeft->p.at(2).z){
					delete portalLeft;
					portalLeft = nullptr;
				}

				if(portalCenter->p.at(0).z == portalCenter->p.at(1).z && portalCenter->p.at(0).z == portalCenter->p.at(2).z){
					delete portalCenter;
					portalCenter = nullptr;
				}
				
				if(portalRight->p.at(0).z == portalRight->p.at(1).z && portalRight->p.at(0).z == portalRight->p.at(2).z){
					delete portalRight;
					portalRight = nullptr;
				}

			
				if(portalLeft != nullptr)
					PortalQueue.push_back(portalLeft);
				
				if(portalCenter != nullptr)
					PortalQueue.push_back(portalCenter);

				if(portalRight != nullptr)
					PortalQueue.push_back(portalRight);


				return true;
				} // if hole

			return false;

		} else if(zmax == zmin && pzmax == pzmin){ //constant Z

			//entry point is a special case - has no geometry to clip portal, makes avgZ and pavgZ different; so...
			if(pxmax >= 0.99f)
				pavgX = avgX;



			//portal wall portal scenario - same center, but different lengths - portal spans polygonset
			if(avgX == pavgX && pxmin < xmin && pxmax > xmax){
			
			
				BSPTreePolygon *portalLeft = new BSPTreePolygon();
				BSPTreePolygon *portalRight = new BSPTreePolygon();

				*portalLeft = *portalPolygon;
				*portalRight = *portalPolygon;

				//split portalPolygon on edge zmin

					//remove part above zmin
					for(uint i = 0; i < 4; i++){
						if(portalLeft->p[i].x >= xmin)
							portalLeft->p[i].x = xmin;
					}

					//remove part below zmin
					for(uint i = 0; i < 4; i++){
						if(portalRight->p[i].x <= xmin)
							portalRight->p[i].x = xmin;
					}
			
				
					PortalQueue.push_back(portalLeft);
					PortalQueue.push_back(portalRight);

				return true;
			}



			float polygonWallLength = xmax - xmin;
			////hole in the middle
				if(polygonWallLength * 2 > polygons.size()){  //  1 of lentgth = 2 triangles
					//has a hole - where??? - test point traveling along it
					//is inside?

					int side1_index = -1, side2_index = 0;
			
					vec3 point(xmin + 0.5f, -0.1f, zmax); //z doesnt matter, y in the center, xmax + 0.5f on the dividing line between two triangles

					//find indices of sides of hole
					for(uint i = 0; i < polygons.size(); i++){
						point.y *= -1.0f;					//not sure about - on dividing line accurancy

						if(not polygons.at(i).isPointInPolygon(point)){
							if(point.x == ((pxmax - pxmin) / 2.0f) && ((pxmax - pxmin) == 1.0f))
								return false;

							side1_index = i - 1;
							side2_index = i;
						break;
						}

						if(i % 2 == 1)
							point.x += 1.0f;
					}

					if((pxmax <= point.x - 0.5f || pxmin >= point.x + 0.5f) || (pxmin >= point.x - 0.5f && pxmax <= point.x + 0.5f)) //dont split
						return false;

					BSPTreePolygon *portalLeft = new BSPTreePolygon();
					BSPTreePolygon *portalCenter = new BSPTreePolygon();
					BSPTreePolygon *portalRight = new BSPTreePolygon();

					*portalLeft = *portalPolygon;
					*portalCenter = *portalPolygon;
					*portalRight = *portalPolygon;

					for(uint i = 0; i < 4; i++){
						if(portalLeft->p[i].x >= point.x - 0.5f)
							portalLeft->p[i].x = point.x - 0.5f;
					}	

					for(uint i = 0; i < 4; i++){
						if(portalCenter->p[i].x >= point.x + 0.5f)
							portalCenter->p[i].x = point.x + 0.5f;

						if(portalCenter->p[i].x <= point.x - 0.5f)
							portalCenter->p[i].x = point.x - 0.5f;
					}

					for(uint i = 0; i < 4; i++){
						if(portalRight->p[i].x <= point.x + 0.5f)
							portalRight->p[i].x = point.x + 0.5f;
					}

				if(portalLeft->p.at(0).x == portalLeft->p.at(1).x && portalLeft->p.at(0).x == portalLeft->p.at(2).x){
					delete portalLeft;
					portalLeft = nullptr;
				}

				if(portalCenter->p.at(0).x == portalCenter->p.at(1).x && portalCenter->p.at(0).x == portalCenter->p.at(2).x){
					delete portalCenter;
					portalCenter = nullptr;
				}
				
				if(portalRight->p.at(0).x == portalRight->p.at(1).x && portalRight->p.at(0).x == portalRight->p.at(2).x){
					delete portalRight;
					portalRight = nullptr;
				}


				if(portalLeft != nullptr)
					PortalQueue.push_back(portalLeft);
				
				if(portalCenter != nullptr)
					PortalQueue.push_back(portalCenter);

				if(portalRight != nullptr)
					PortalQueue.push_back(portalRight);

				return true;

		}// if hole

		return false;
		}

return false;
}

/*
Implements process of portal placing.
First gets root divider polygon into queue and while it's not empty calls placePortal.
Once leaves are populated with portal geometry, they are processed. That means 
split and filtered until only portals over holes in geometry remain.
Then those portals get merget when required and put to PortalSet of Tree.
Done.
*/
void BSPTreeCreator::placePortals(){
		
	std::cout << "\rPlacing portals\n";


	//get initial portal polygon
	PortalQueue.push_back(RootNode->getPortal());

	BSPTreePolygon *portalPolygon;

	do {

		portalPolygon = PortalQueue.back();
		PortalQueue.pop_back();

		placePortal(RootNode, portalPolygon);
	
	} while(not PortalQueue.empty());



	//check and accept/delete portals
	for(unsigned int i = 0; i < NodeSet.size(); i++){
		if(NodeSet[i]->isLeaf())
			NodeSet[i]->processPortals();
	
	}


	mergeOverlappingPortals();

}


/*
PLACE-PORTALS
Indata:
PortalPolygon – basicPolygon to push down the tree
Node – The node that we are currently visiting.
Outdata:
None
Effect:
Pushes a portal polygon down through the tree clipping when it
needs it. The output of this function will be that each node
contains a list of portal polygons where each portal connects
exactly two nodes.


PLACE-PORTALS (PortalPolygon, Node)
1 if (IS-LEAF (Node))
 
The portal is checked against every polygon in the node. When the
portal polygon is spanning the plane defined by a polygon it will
be clipped against that plane. The two resulting parts will be
sent down from the top of the tree again.
2 for (each polygon P2 in Node)
3 IsClipped f false
4 if (CALCULATE-SIDE (P2, PortalPolygon) = SPANNING)
5 IsClipped f true
6 (RightPart, LeftPart) f CLIP-POLYGON (P2, PortalPolygon)
7 PLACE-PORTALS (RightPart, RootNode)
8 PLACE-PORTALS (LeftPart, RootNode)
9 if (not IsClipped)
10 Remove the parts of the portal polygon that coincide with
other polygons in this node. w see description below
11 Add this node to the set of connected nodes in this
portal polygon.
12 else
13 if (the dividing polygon of this node hasn’t been pushed down
the tree)
14 Create a polygon P that is larger than the bounding box that
contains all polygons in the sub trees of this node that
lies in the same plane as the dividing polygon.
15 PLACE-PORTALS (P, Node.LeftChild)
16 PLACE-PORTALS (P, Node.RightChild)
17 Side f CALCULATE-SIDE (Node.Divider, PortalPolygon)
18 if (Side = POSITIVE)
19 (RightPart, LeftPart) f CLIP-POLYGON(P2, PortalPolygon)
20 PLACE-PORTALS (RightPart, RootNode)
21 PLACE-PORTALS (LeftPart, RootNode)
22 if (Side = POSITIVE or COINCIDING)
23 PLACE-PORTALS (PortalPolygon, Node.RightChild)
24 if (Side = NEGATIVE or COINCIDING)
25 PLACE-PORTALS (PortalPolygon, Node.LeftChild)

			
The general idea is that each portal in the tree must be coinciding with a plane defined by
a dividing polygon in the tree. Out of each of these planes a portal polygon is created,
that portal polygon is initially a four-sided polygon that exceeds the bounding box of
the node it is located in.
			
Recursive function to be called with RootNode.
Creates new portal polygons from dividers of nodes and sends them to its children.
Then splits the incoming portals, if needed, and sends them again from top - RootNode.
Finally when having candidate portal, it is split, shortened and deleted until there is only one portal covering each entrance.
Pointer to accepted portal is stored in node and in BSPTreeCreator too.

///Recursion is problem when dealing with larger tree...
//Every new portal/split causes recursion one level deeper and returns after all fragments get placed or discarded
which can be too much..


//what about internal portal queue?
//1 one at tree level used for creating and distribution of portals into leaves (internal portal queues) and
//2 then one for clipping, splitting, checking and accepting/refusing portals from internal queue to PortalList
//also split this function into two.. 1 while cycle to populate leaves with portals
//second to check, clip and accept them

so.. new meaning -> place portal in appropriate leaf, split if necessary
*/
void BSPTreeCreator::placePortal(BSPTreeCreatorNode *node, BSPTreePolygon *portalPolygon){

	//switch from recursion to iterative portal queue
	//viability gets limited only by generation of BSPtree and time, not crashes






	if(not node->isLeaf()){
		
		//portal actions..
			//generate new if not leaf
			//split into two
			//recursive routing through tree to leaf
			//save

		/*
		Quote: each portal in the tree must be coinciding with a plane defined by
		a dividing polygon in the tree. Out of each of these planes a portal polygon is created,
		that portal polygon is initially a four-sided polygon that exceeds the bounding box of
		the node it is located in. Then each portal polygon is pushed down the sub trees of the
		node it is in.
		*/

		
		//create portal if not previously done
		//clip incoming portal by divider -> send both parts from top of the tree again
		if(not node->Divider.usedAsPortal){ // produces portals from dividers

			BSPTreePolygon *portal = node->getPortal(); 
						
			//put to queue oly once; it gets copied if needed on the way down..
			PortalQueue.push_back(portal);

		}


		if(portalPolygon->p.size() != 4)// not happening, but in case of general direction is possible 3-6
			return;
	
		
		/* Quote: Then each portal polygon is pushed down the sub trees of the
		node it is in. When a portal polygon passes through a node in one of its sub trees the
		plane defined by the dividing polygon in that node clips it */
		int side = node->Divider.calculateSide(*portalPolygon);

		if(side == SPANNING){ // produces many portalPolygons. split by all the dividers if it spans them
			
			std::vector<vec3> front, back;

			// If a polygon is clipped, the two resulting parts are sent down from the top of the tree.
			portalPolygon->splitPolygon(node->Divider, front, back);
				
			delete portalPolygon;
					
			if(not front.empty())
				PortalQueue.push_back(new BSPTreePolygon(front));
				
			if(not back.empty())
				PortalQueue.push_back(new BSPTreePolygon(back));

			return;
		} //end of not recursive part




		/*	When a portal polygon is not in need of any
		clipping, it is sent down to the sub trees of the node currently visiting.*/
			
		//This means that if it is on the positive side of the plane it will be sent down the right sub tree,
		//But if it is coinciding with the plane defined by the dividing polygon in the current node it will be 
		//sent down both sub trees.

		//recursive routing of portalPolygons through the tree
		if(side == INFRONT ){	
			placePortal(node->RightChild, portalPolygon);	

		} else if(side == BEHIND ){
			placePortal(node->LeftChild, portalPolygon);	

		} else if(side == COINCIDING){
			BSPTreePolygon *portal2 = new BSPTreePolygon();
			*portal2 = *portalPolygon;

			placePortal(node->RightChild, portalPolygon);				
			placePortal(node->LeftChild, portal2);			
		}


	} else {   //is leaf VV

		node->PortalQueue.push_back(portalPolygon);  //Portal reached the edge of tree.. save it for later into leaf portal queue

	}

}


bool BSPTreeCreatorNode::clip(BSPTreePolygon *&portalPolygon){

	//it is also clipped by the polygons in that node if the node is a leaf.
	for(uint i = 0; i < PolygonSet.size(); i++){
		
		if(PolygonSet[i].calculateSide(*portalPolygon) == SPANNING){

			std::vector<vec3> front, back;

			portalPolygon->splitPolygon(PolygonSet[i], front, back);

			if(not front.empty())
				PortalQueue.push_back(new BSPTreePolygon(front));
				
			if(not back.empty())
				PortalQueue.push_back(new BSPTreePolygon(back));

			
			return true;

		}
	} // for

return false;
}

/*
Function for leaf.
*/
void BSPTreeCreatorNode::processPortals(){

		//portal actions
			//clip
			//check
			//accept or delete

	
	while(not PortalQueue.empty()){


		BSPTreePolygon *portalPolygon = PortalQueue.back();
		PortalQueue.pop_back();

	
		if(clip(portalPolygon)){ //if portalPolygon got split by geometry go again with new portal from queue
			delete portalPolygon; //original got split
			continue;
		}
		

		
		bool convex = false;
		bool inside = true;
		int behind = 0, infront = 0;

		//check if portal is in some way inside convex polygon set
		for(unsigned int j = 0; j < PolygonSet.size() && inside; j++){
			
			if(not PolygonSet[j].polygonInfront(*portalPolygon)) //accepts coinciding too
				inside = false;
		}
	
		//check if all polygons of this set are infront or behind portal - portal is on edge of the set - not necessarily exclusive with previous check - could be in the middle of space.. 
		for(unsigned int j = 0; j < PolygonSet.size() && inside && not (behind != 0 && infront != 0); j++){
			
			if(not portalPolygon->polygonInfront(PolygonSet[j])){ //means exclusively behind
				behind++;
			} else if(not portalPolygon->polygonBehind(PolygonSet[j])){//means exclusively infront
				infront++;
			}

		}
			
		if((behind == 0 && infront >= 0) || (infront == 0 && behind >= 0)) 
			convex = true;

		if(convex && inside){ // wasnt clipped and doesnt break convexity of polygonset


			//puts split polygons into queue.. continue as usual..
			if(splitOnHoles(portalPolygon) || portalPolygon->isTiny()){ //splits in case when portal polygon covers entrance and some wall too
				delete portalPolygon;
				continue; // new split polygons are at the end of queue
			}
				
			portalPolygon = removeCoincidingParts(portalPolygon);  //should remove polygons covering only wall - polygons over holes should remain
				
			if(portalPolygon != nullptr){

				//portal is accepted, put into tree and node PortalSet and pointer to owner node is added
				portalPolygon->ownerNode = this;

				PortalSet.push_back(portalPolygon);
			
			}


		} else { //convex && inside
			delete portalPolygon;
		}

	} // while


}


//Ids which are not already in potentialNeighbours are added to back of this list
void BSPTreeCreator::addNeighbours(std::vector<int> *potentialNeighbours, std::vector<int> *IdSet){

	for(uint i = 0; i < IdSet->size(); i++){

		if(std::find( potentialNeighbours->begin(), potentialNeighbours->end(), IdSet->at(i)) == potentialNeighbours->end()){ // didnt find it
			potentialNeighbours->push_back(IdSet->at(i));
		
		}
	}
}

//Ids which are not already in potentialNeighbours are added to back ot this list
void BSPTreeCreator::addNeighbour(std::vector<int> *potentialNeighbours, int IdSet){

	if(std::find( potentialNeighbours->begin(), potentialNeighbours->end(), IdSet) == potentialNeighbours->end()){ // didnt find it
		potentialNeighbours->push_back(IdSet);
	}
}


/*
We have: 
list of pointers to all portals in tree
pointers to portals in node
Id in node
empty list of Ids of potentially visible nodes
list of all nodes sorted by Id
*/
void BSPTreeCreator::computePVS(){

	std::cout << "Computing PVS\n";

	/*
	//connects all immediately neighbouring nodes - easy
	foreach portal in all portals
		if portal shares same space - not coincides(as with planes)
			put Id of the node that other portal is in to PVNodes*/
	for(uint i = 0; i < PortalSet.size(); i++){
		for(uint j = 0; j < PortalSet.size() && PortalSet[i]->connectedId == -1; j++){ //skip this entirely if already has a neighbour
	
			if(i == j)
				continue;
		

			if(PortalSet[i]->shareSpace(PortalSet[i], PortalSet[j])){  // home node index gets inserted into PVNOdes at object creation.
				int I = PortalSet[j]->ownerNode->Id;
				PortalSet[i]->ownerNode->PVNodes.push_back(I); //add Id of the other node to PVNodes of current node
				//PortalSet[i]->neighbourNode = PortalSet[j]->ownerNode;

				PortalSet[i]->connectedId = I;
				break; //from inner for cycle.. portal cannot share space with multiple portals...
			}		
	
		}
	}


	//portal clippings at the entrance must be disregarded..
	//should still have default connectedId value of -1
	for(unsigned int i = 0; i < PortalSet.size(); i++){
		if(PortalSet[i]->connectedId == -1)
			PortalSet.erase(PortalSet.begin() + i);
	}


	//+need to remove them from node PortalSets too, because from there they are eventually exported
	for(unsigned int i = 0; i < NodeSet.size(); i++){
		if(not NodeSet[i]->isLeaf())
			continue;

		for(unsigned int j = 0; j < NodeSet[i]->PortalSet.size(); j++){
			if(NodeSet[i]->PortalSet[j]->connectedId == -1){
				delete NodeSet[i]->PortalSet[j];
				NodeSet[i]->PortalSet.erase(NodeSet[i]->PortalSet.begin() + j);
			}
		}
		
	}

	//copy the result so far into immediate neighbours vector
	for(uint i = 0; i < NodeSet.size(); i++){
		if(NodeSet[i]->PVNodes.size() > 0){

			NodeSet[i]->immediateNeighbours.resize(NodeSet[i]->PVNodes.size());
			std::copy(NodeSet[i]->PVNodes.begin(), NodeSet[i]->PVNodes.end(), NodeSet[i]->immediateNeighbours.begin());
		
		}
	}

	//carry on..



	//what to do:
	/*

	//connects all PVNodes nodes - harder
	foreach portal in all portals
		if portal sees other portal 
			put Id of the node that other portal is in to PVNodes

	*/


	
	//for all leaves
	for(uint current_leaf_index = 0; current_leaf_index < NodeSet.size(); current_leaf_index++){

		if(not NodeSet[current_leaf_index]->isLeaf())  
			continue;

		std::vector<int> potentialNeighbours;
		BSPTreeCreatorNode *current_leaf = NodeSet[current_leaf_index];

		//for all neighbours of current leaf get their neigbours
		for(uint i = 1; i < current_leaf->PVNodes.size(); i++){ // get neighbours and their PVNodes

			BSPTreeCreatorNode *neighbour = NodeSet[  current_leaf->PVNodes[i]  ]; //neighbours are in curret leaf's PVNodes       

			addNeighbours(&potentialNeighbours, &neighbour->PVNodes); //for each put its PVNodes vector into current leaf's PVNodes vector
		}


		for(uint i = 0; i < potentialNeighbours.size(); i++){

			if(potentialNeighbours[i] == current_leaf_index) //if indexes match, continue
				continue;


			bool visible = false;
			BSPTreeCreatorNode *potentialNeighbour = NodeSet[potentialNeighbours[i]];

			visible = areNodesVisible(current_leaf, potentialNeighbour);

			
			if(visible){
				//put potential neighbour into pvs of current leaf 
				addNeighbour(&current_leaf->PVNodes, potentialNeighbour->Id);

				//add its neighbours into list of potential neighbours - no duplicates
				addNeighbours(&potentialNeighbours, &potentialNeighbour->immediateNeighbours);
			}
						
					
		} //for all potential neighbours

	} // for all leaves

}

/*
Potential neighbour is not necessarily immediate neighbour. There may be another node(s) between them.
IDEA - IS TRULY NECESSARY TO TEST ALL PORTALS AGAINST ALL OTHERS? NODES ARE CONVEX AFTER ALL... - could be in plane...
BUT.. test one point from first, then second, third portal - could shave off some time, but disproving visibility still hard..
*/
bool BSPTreeCreator::areNodesVisible(BSPTreeCreatorNode *current_leaf, BSPTreeCreatorNode *potentialNeighbour){

	if(not current_leaf->isLeaf() || not potentialNeighbour->isLeaf())
		return false;

	if(current_leaf->Id == potentialNeighbour->Id)
		return false;



	//got current node and one from potential neighbours which isnt self
	//for every combination of portals
		//generate 2 vectors of points inside portals
		//use them to construct rays
			//if at least one comes through without hitting something - win


	for(uint p1 = 0; p1 < current_leaf->PortalSet.size(); p1++){

		std::vector<vec3> portal1_points;
		current_leaf->PortalSet.at(p1)->getPoints(portal1_points);
	
	for(uint p2 = 0; p2 < potentialNeighbour->PortalSet.size(); p2++){


		std::vector<vec3> portal2_points;
		potentialNeighbour->PortalSet.at(p2)->getPoints(portal2_points);
		
	
	
		//for every combination of points
		for(uint i = 0; i < portal1_points.size(); i++){
		for(uint j = 0; j < portal2_points.size(); j++){

			bool hit = false;

			//test against current potentially visible neighbours if the line connecting portals intersects ANY of their geometry
			//only if it doesnt hit anything in all the PVNodes we can say the two portal are mutually visible
			for(uint pvnode = 0; pvnode < current_leaf->PVNodes.size(); pvnode++){

				auto PV_Node = NodeSet[current_leaf->PVNodes.at(pvnode)];
				
				if(PV_Node == potentialNeighbour)
					continue;
		
				vec2 start(portal1_points.at(i).swizzle(glm::X, glm::Z));
				vec2 end(portal2_points.at(j).swizzle(glm::X, glm::Z));
				
				


				bool pointA = sideOfLine(start, end, vec2(PV_Node->xmin - 0.5f, PV_Node->zmax + 0.5f));
				bool pointB = sideOfLine(start, end, vec2(PV_Node->xmax + 0.5f, PV_Node->zmax + 0.5f));

				bool pointC = sideOfLine(start, end, vec2(PV_Node->xmax + 0.5f, PV_Node->zmin - 0.5f));
				bool pointD = sideOfLine(start, end, vec2(PV_Node->xmin - 0.5f, PV_Node->zmin - 0.5f));

				//if all squares are on the same side of line, there is no need to check this node
				if((pointA + pointB + pointC + pointD) % 4 == 0){  //all false or all true
					continue;
				}
				
				//expensive check..	
				hit |= PV_Node->rayIntersectsSomething(portal1_points.at(i), portal2_points.at(j));
					
						
			}
			

			if(not hit)	{		//if at least once something didn't hit, it's visible
				std::cout << "\r" << current_leaf->Id << "/" << current_leaf->Tree->node_count - 1 << " - " << potentialNeighbour->Id << "                  "; //in case of smaller numbers.. cover reamining characters with spaces
				return true;
			}

		
		}// for points in portal2
		}// for points in portal1


	}// for p1
	}// for p2

return false;
}


/*
Uses cross product to determine whether point is on side1 or side2 of line.
Points on line returns as on side1.
*/
bool sideOfLine(vec2 start, vec2 end, vec2 point){

	float result = ((end.x - start.x) * (point.y - start.y) - (end.y - start.y) * (point.x - start.x));

	if(result >= 0.0f){
		return SIDE1;
	} else return SIDE2;
}





void BSPTreeCreatorNode::computeExtremes(){

	if(PolygonSet.size() > 0){
	//some existing point values
		xmax = PolygonSet[0].p[0].x;
		ymax = PolygonSet[0].p[0].y;
		zmax = PolygonSet[0].p[0].z;

		xmin = PolygonSet[0].p[0].x;
		ymin = PolygonSet[0].p[0].y;
		zmin = PolygonSet[0].p[0].z;
	
	}

	//for each polygon in polygonset
	for(auto polygon : PolygonSet){
		for(unsigned int i = 0; i < 3; i++){
			
			xmax = max(polygon.p[i].x, xmax);
			ymax = max(polygon.p[i].y, ymax);
			zmax = max(polygon.p[i].z, zmax);

			xmin = min(polygon.p[i].x, xmin);
			ymin = min(polygon.p[i].y, ymin);
			zmin = min(polygon.p[i].z, zmin);
	
	
		}
	}


}



/*
RAY-INTERSECTS-SOMETHING-IN-TREE
Indata:
Node – The node to trace through
Ray – The ray to test for intersection.
Outdata:
Whether the ray intersected something or not.
Effect:
Checks if the ray intersects something in this node or any of this
node’s sub trees.


RAY-INTERSECTS-SOMETHING-IN-TREE (Node, Ray)
1 for each polygon P in Node
2 POLYGON-IS-HIT (P, Ray)
3 startSide f CLASSIFY-POINT (Ray.StartPoint, Node.Divider)
4 endSide f CLASSIFY-POINT (Node.EndPoint, Node.Divider)
If the ray spans the splitting plane of this node or if the ray is
coinciding with the plane, send it down to both children
5 if ((startSide = COINCIDING and endSide = COINCIDING) or
startSide <> endSide and startSide <> COINCIDING and
endSide <> COINCIDING)
6 if (RAY-INTERSECTS-SOMETHING-IN-TREE (Node.LeftChild, Ray))
7 return true
8 if (RAY-INTERSECTS-SOMETHING-IN-TREE (Node.RightChild, Ray))
9 return true
If the ray is only on the positive side of the splitting plane
send the ray down the right child. The or in the if statement is
because one of the points might be coinciding with the plane.
10 if (startSide = INFRONT or endSide = INFRONT)
11 if(RAY-INTERSECTS-SOMETHING-IN-TREE (Node.RightChild, Ray))
12 return true
If the ray is only on the positive side of the splitting plane
send the ray down the right child. The or in the if statement is
because one of the points might be coinciding with the plane.
13 if (startSide = BEHIND or endSide = BEHIND)
14 if (RAY-INTERSECTS-SOMETHING-IN-TREE (Node.LeftChild, Ray))
15 return true
There was no intersection anywhere, pass that upwards
16 return false

Tries to hit all polygons in node with line segment.
--ray is slightly misleading.
true if hit any polygon in polygonset
*/
bool BSPTreeCreatorNode::rayIntersectsSomething(vec3 start, vec3 end){

bool hit = false;


	if(isLeaf()){
		
		for(uint i = 0; i < PolygonSet.size() && not hit; i++){

			if(PolygonSet[i].isHit(start, end))		
				hit =  true;	
		}
	}


return hit;
}


