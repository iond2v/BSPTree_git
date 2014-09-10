#include "Maze.h"


char map1[] = {	//17 x 14
			"#################"
			"##             ##"
			"## ##### ########"
			"##     #   #   ##"
			"## ####### # # ##"
			"## #     #   # ##"
			"## # ### # ### ##"
			"## #   # # #   ##"
			"## ### # ### ####"
			"##   # #       ##"
			"#### # ###  #  ##"
			"##   #   #     ##"
			"######## ########"
			"#################"};

char map2[] = {	//20 x 15
			"####################"
			"#                  #"
			"# # # # # # # # # ##"
			"#                  #"
			"# # # # # # # # # ##"
			"#                  #"
			"# # # # # # # # # ##"
			"#                  #"
			"# # # # # # # # # ##"
			"#                  #"
			"# # # # # # # # # ##"
			"#                  #"
			"# # # # # # # # # ##"
			"#                  #"
			"####################"};




char map3[] = {	//30 x 30 
			"##############################"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # # # # # # # # # ##"
			"#                            #"
			"# # # # # # #   # # # # # # ##"
			"##############################"};
			

char map4[] = { // 50x50

	"##################################################"
	"#       #       #         #     #               ##"
	"# ##### # ##### # ### # ### ### # ######### ### ##"
	"#     # # #   # #   # # #   # # # #       #   # ##"
	"### ### # ### # # ### # # ### # # # ##### ### # ##"
	"# # #   #     #   #   # #   # #   # #         # ##"
	"# # # ####### ##### # ##### # ##### ########### ##"
	"#   #   #   #     # # #     #     #   #   #   # ##"
	"# ##### # # # ##### ### ##### ### ### # # # # # ##"
	"#     # # #   #   #   # # #   # #   #   # # # # ##"
	"# ##### # ##### # ### # # # # # ### ##### # # # ##"
	"# #     # # #   # #   # #   # # #   # #   # #   ##"
	"### ##### # # ### # ### ##### # # ### # ### ######"
	"#   #   #   # #   #   #   #   # #     # # #     ##"
	"# ### # ### # # ##### ### # ### # ##### # ##### ##"
	"#   # #     # #       #   # #   # #     #   #   ##"
	"# # # ####### ### ##### ### # ### # ##### # # ####"
	"# # # #     #   # #   # #   #   # #   #   # # # ##"
	"### # ##### ### ### # # # # ### # ### # ##### # ##"
	"#   #     #   # #   #   # #   # #   # #         ##"
	"# ####### # # # # ########### # # ### # ####### ##"
	"#     #   # # #   #           #   #   # #   # # ##"
	"# ### # ### # ##### ### # ##### ### ##### # # # ##"
	"# # #   #   # # #     # # #   #   #     # # #   ##"
	"# # ##### ### # # ##### # # # ### ##### # # ######"
	"#         #   #   #   # # # # #       #   #     ##"
	"# ##### ##### ##### # # ### # ####### ######### ##"
	"# #   # #   #       # #     #     # #     #   # ##"
	"# # # ### # ######### ### ####### # ##### ### # ##"
	"# # #   # # #           #     #   #     #     # ##"
	"# # ### # # ########### ####### ##### ##### ### ##"
	"# #   # # #         # # #       #     #   # #   ##"
	"##### # # ######### # # # # ##### # ### # # # ####"
	"#     #   #   #     #     # #   # #     #   #   ##"
	"# ######### # # ######### ### # ########### ### ##"
	"# #   #   # # #     #   # #   #           # # # ##"
	"# # # # # ### ##### # # ### ############# # # # ##"
	"# # #   #   # #     # #     #   #       # #   # ##"
	"# # ####### # # ##### ####### # # ##### # ### # ##"
	"# #   # #   #         #   #   # # #     #   # # ##"
	"# ### # # ############# # # # ### # ### ### # # ##"
	"# #   # #         #     #   #     # #     # # # ##"
	"# # ### ######### ####### ######### # ##### ### ##"
	"#   #         #   #     #         # # #   #     ##"
	"# ####### ### # # # ### ######### # ### # ##### ##"
	"# #     #   # # # #   # #       # #   # #     # ##"
	"# # ### ##### # ##### # ### # ### ### # ### ### ##"
	"#   #         #       #   # #       #     #     ##"
	"########################  ########################"
	"##################################################"

};

char map30[] = {	//60 x 60 - currently 1400s to compute PVS
			"############################################################"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"#                                                          #"
			"# # # # # # # # # # # # # # # # # # # # # # # # # # # # # ##"
			"############################################################"};


char map31[] = {	//31 x 31 
			"###############################"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"# # # # # # # # # # # # # # # #"
			"#                             #"
			"###############################"};
using namespace std;

const std::string Maze::save_path = ".\\Mazes\\";


/*
maze naming 

30x20_index_5.maze
50x50_default.maze

*/


/*
This creates random maze with set parameters. Currently clamped to 200x200.
*/
Maze::Maze(int width, int length, std::string type = "default") : x(width), y(length){

	vertexArray = new std::vector<float>();

	//name = "maze"+ std::to_string(x) +"x"+ std::to_string(y);
	name = std::to_string(x) +"x"+ std::to_string(y) + "_" + type;
	string path = save_path + name + ".maze";

	if(PathFileExists(std::string(path).c_str())){
		std::cout << "Maze with set dimensions already exists. Loading that.\n";
		loadMaze(name + ".maze");
		printMaze();
	return;
	}

	this->type = type;
	

	x = glm::min(200, x);   //generation of bsp from this takes over 20 min
	y = glm::min(200, y);

	//std::cout << "Maze constructed\n";
	max_index = x * y - 1;
	srand((unsigned int) time(nullptr));
	generateMaze();
	printMaze();
	
}

/*
Loads maze and fills vertex array too.
*/
Maze::Maze(std::string filename){

	vertexArray = new std::vector<float>();

	loadMaze(filename);
	printMaze();
	
}


/*
Creates preset mazeMap.
*/
Maze::Maze(int number = 1){

	vertexArray = new std::vector<float>();

	srand((unsigned int) time(nullptr));
	
	if(PathFileExists(std::string(save_path + "preset_maze_index_"+std::to_string(number)).c_str())){
		std::cout << "Preset maze with provided index already exists. Loading that.\n";
		loadMaze(name);
	return;
	}


	char *map;

	switch (number){
	
		case 1: 
			x = 17;
			y = 14;
			max_index = x * y - 1;
			map = map1;
		
			break;
		
		case 2: 
			x = 20;
			y = 15;
			max_index = x * y - 1;
			map = map2;
		
			break;
	
		case 3:
			x = 30;
			y = 30;
	
			max_index = x * y - 1;
			map = map3;
		
			break;

		case 4:
			x = 60;
			y = 60;
	
			max_index = x * y - 1;
			map = map30;
		
			break;
	
		case 5:
			x = 50;
			y = 50;
	
			max_index = x * y - 1;
			map = map4;
		
			break;

		case 6:
			x = 31;
			y = 31;
	
			max_index = x * y - 1;
			map = map31;
		
			break;

		case 7:
			x = 180;
			y = 180;

			max_index = x * y - 1;
			map = nullptr;

			break;

		default:
			x = 17;
			y = 14;
			max_index = x * y - 1;
			map = map1;
		
			break;
	
	}

	if(map == nullptr)
		return;

	mazeMap.resize(x * y);
	

	for(unsigned int i = 0; i <= max_index; i++){
		if(map[i] == '#'){
			mazeMap[max_index - i] = WALL;
		} else mazeMap[max_index - i] = SPACE;
	}

	printMaze();

	//name = "preset_maze_index_"+ std::to_string(number);
	name = std::to_string(x) +"x"+ std::to_string(y) + "_index_"+ std::to_string(number);

}


/*
Generates mazeMap.
*/
void Maze::generateMaze(){
	
	mazeMap.resize(x * y);
	mazeVisitedCell.resize(x * y);

	std::fill(mazeMap.begin(), mazeMap.end(), SPACE);
	std::fill(mazeVisitedCell.begin(), mazeVisitedCell.end(), false);



	//put walls at borders in mazeMap and set them as visited
	for(unsigned int i = 0; i < y; i++){ //rows
		for(unsigned int j = 0; j < x; j++){ //columns
				
			if(i == 0 || i == y - 1){

				mazeMap[j + i * x] = WALL;
				mazeVisitedCell[j + i * x] = true;

			} else if(j == 0 || j == x - 1){

				mazeMap[j + i * x] = WALL;
				mazeVisitedCell[j + i * x] = true;
			}
		}
	}

	if (type == "default"){
		walkMaze(x / 2);
	
	} else if (type == "columns") {
	
		walkMazeColumns(x / 2);
	}

	
		
}


//walks through maze space and fills it
void Maze::walkMaze(int start){
	
	doStep(start, NONE);
	
		for(unsigned int i = 0; i < this->y; i++){ //rows
			for(unsigned int j = 0; j < this->x; j++){ //columns

				//all currently unvisited cells must be walls
				if(mazeVisitedCell[j + i * this->x] == false){
					mazeMap[j + i * this->x] = WALL;
				}
				
			}
		}
	
}

void Maze::walkMazeColumns(int start){

	for(unsigned int i = 0; i < this->y; i++){ //rows
	for(unsigned int j = 0; j < this->x; j++){ //columns

		if(i % 2 == 0 && j % 2 == 0)
			mazeMap[j + i * this->x] = WALL;

						
	}
	}


}


void Maze::doStep(int currentPosition, int fromDirection){
	
	int direction;
	bool stepped = false;
	
	//mark step path as visited
	connectVisits(currentPosition, fromDirection);


	//If the current cell has any neighbours which have not been visited 
	while(hasVisitableNeighbour(currentPosition)){

		//Choose randomly one of the unvisited neighbours
		direction = rand() % 4;

		switch(direction){
		
			case UP:
				if(canGoUp(currentPosition))
					doStep(currentPosition + x * STEP, DOWN);

				break;

			case DOWN:
				if(canGoDown(currentPosition))
					doStep(currentPosition - x * STEP, UP);
					
				break;

			case LEFT:
				if(canGoLeft(currentPosition))
					doStep(currentPosition - STEP, RIGHT);
					
				break;

			case RIGHT:
				if(canGoRight(currentPosition))
					doStep(currentPosition + STEP, LEFT);

				break;
		}
	}
	
}

/*
Generates geometry from mazeMap.
*/
void Maze::generateVertexArray(/*scale?*/){


	vector<GLfloat> verticesOnlyArray;
		
	
	for(int row = 0; row < y; row++){ //y - column => walk rows
		for(int column = 0; column < x; column++){
			
			if(mazeMap[column + row * x] == SPACE){
				modelAddItem(&verticesOnlyArray,  column, row, floorVertices, sizeof(floorVertices));

				modelAddItem(&verticesOnlyArray, column, row, wallCeilingVertices, sizeof(wallCeilingVertices));

				//if to right is space/nothing  -> place left wall
				if(column + row * x + 1 < max_index && mazeMap[column + row * x + 1] == WALL)
					modelAddItem(&verticesOnlyArray, column + 1, row, wallLeftVertices, sizeof(wallLeftColors));
	
				//if to left is space/nothing -> place wall
				if(column + row * x - 1 >= 0 && mazeMap[column + row * x - 1] == WALL)
					modelAddItem(&verticesOnlyArray, column - 1, row, wallRightVertices, sizeof(wallRightVertices));			

				//if up is space/nothing -> place wall
				if(column + row * x + x < max_index && mazeMap[column + row * x + x] == WALL)
					modelAddItem(&verticesOnlyArray, column, row + 1, wallFrontVertices, sizeof(wallRightVertices));

				//if down is space/nothing -> place wall
				if(column + row * x - x >= 0 && mazeMap[column + row * x - x] == WALL)
					modelAddItem(&verticesOnlyArray, column, row - 1, wallBackVertices, sizeof(wallRightVertices));


			} else if(mazeMap[column + row * x] == WALL){
			
				
				/*
				//bottom row -> place front walls
				if(row == 0)
					modelAddItem(&verticesOnlyArray, &colorsOnlyArray, column, row, wallFrontVertices, wallFrontColors, sizeof(wallCeilingVertices));		
				
				//top row -> place back walls
				if(row == y - 1)
					modelAddItem(&verticesOnlyArray, &colorsOnlyArray, column, row, wallBackVertices, wallBackColors, sizeof(wallCeilingVertices));
				 
				//left column -> place left walls
				if(column == 0 )
					modelAddItem(&verticesOnlyArray, &colorsOnlyArray, column, row, wallLeftVertices, wallLeftColors, sizeof(wallCeilingVertices));
				 
				//right column -> place right walls
				if(column == x - 1)
					modelAddItem(&verticesOnlyArray, &colorsOnlyArray, column, row, wallRightVertices, wallRightColors, sizeof(wallCeilingVertices));			
				*/

			}//else if WALL

		}// for column
	}//for row
		

	


/*
	// do a cube
	verticesOnlyArray.insert(verticesOnlyArray.end(),	boxVertices,	boxVertices +	(sizeof(boxVertices) / sizeof(float)));
	colorsOnlyArray.insert(colorsOnlyArray.end(),		boxColors,		boxColors +		(sizeof(boxColors) / sizeof(float)));

	*/
	

vertexArray->clear();

//copy inside return vector allocated on heap
vertexArray->reserve(verticesOnlyArray.size());
vertexArray->insert(vertexArray->end(), verticesOnlyArray.begin(), verticesOnlyArray.end());


}


/*
Adds item e.g wall/ceiling etc. into vertices array.
*/
void Maze::modelAddItem(vector<float> *verticesOnlyArray, int column, int row, const float *vertices, int size){

	vector<float> tmp;
	tmp.insert(tmp.end(), vertices, vertices + size / sizeof(float));

	for(unsigned int i = 0; i < tmp.size(); i++){
		
		if(i % 4 == 0){ // x coordinate
			tmp[i] += (float) column;		//add column offset to coordinates being added to model
		} else if(i % 4 == 2) { // z coordinate
			tmp[i] -= (float) row;
		}	
	}

	verticesOnlyArray->insert(verticesOnlyArray->end(), tmp.begin(), tmp.end());
}


bool Maze::canGoUp(int currentPosition){

	if((currentPosition + x * STEP) <= max_index && mazeVisitedCell[currentPosition + x * STEP] == false )
		return true;

	return false;
	}

bool Maze::canGoDown(int currentPosition){

	if((currentPosition - x * STEP) >= 0 && mazeVisitedCell[currentPosition - x * STEP] == false)
		return true;

	return false;
	}

bool Maze::canGoLeft(int currentPosition){

	int current_row = currentPosition / x;
	int row_after_step = (currentPosition - STEP) / x;


	if((currentPosition - STEP) >= 0 && current_row == row_after_step && mazeVisitedCell[currentPosition - STEP] == false)
		return true;

	return false;
	}

bool Maze::canGoRight(int currentPosition){

	int current_row = currentPosition / x;
	int row_after_step = (currentPosition + STEP) / x;

	if((currentPosition + STEP) <= max_index && current_row == row_after_step && mazeVisitedCell[currentPosition + STEP] == false)
		return true;

	return false;
	}

bool Maze::hasVisitableNeighbour(int currentPosition){
	
	if(canGoRight(currentPosition) || canGoLeft(currentPosition) || canGoDown(currentPosition) || canGoUp(currentPosition) || canGoDown(currentPosition))
		return true;

	return false;
	}

/*
marks current position and all positions between current and last as visited.
*/
void Maze::connectVisits(int currentPosition, int fromDirection){

	switch(fromDirection){
		
		case UP: 
			for(unsigned int i = 0; i < STEP; i++){
				mazeVisitedCell[currentPosition + x * i] = true;
				}
			break;
		case DOWN:
			for(unsigned int i = 0; i < STEP; i++){
				mazeVisitedCell[currentPosition - x * i] = true;
				}
			break;
		case LEFT:
			for(unsigned int i = 0; i < STEP; i++){
				mazeVisitedCell[currentPosition - i] = true;
				}
			break;
		case RIGHT:
			for(unsigned int i = 0; i < STEP; i++){
				mazeVisitedCell[currentPosition + i] = true;
				}
			break;
		case NONE:
			break;

		default:
			fprintf(stderr, "connectVisits  from direction default");
			break;

		}
	
	}




/*
saves maze in file with its map
Appends to maze name .maze

---start of file---
width depth type
######
# # ##
#    #
######
0.5 0.5 0.5
0.5 0.5 0.5
0.5 0.5 0.5
...
..
.
.
---end of file---
*/
bool Maze::saveMaze(){
	
	if(vertexArray->empty() || mazeMap.empty()){
		std::cout << "vertex array missing or mazeMap is empty. Maze not saved\n";
		return false;
	}

	std::string filename = save_path + name + ".maze";

	if(not PathFileExists(save_path.c_str()))
		CreateDirectory(save_path.c_str(), NULL);

	if(PathFileExists(filename.c_str())){
		std::cout << "Maze with such name is already saved.\n";
		return false;

	}


	ofstream file;
	file.open(filename, ios::out);

	if(not file.is_open()){
		std::cout << "error opening  " << filename << " for writing.";
	return false;
	}

	//first line
	file << this->x << " " << this->y << " " << this->type << std::endl;


	//map of maze
	for(int row = this->y - 1; row >= 0 ; row--){
		std::string maze_row;

		for(unsigned int column = 0; column < this->x; column++){
			
			if(mazeMap[row * x + column] == WALL){
				maze_row.append("#");
			} else {
				maze_row.append(" ");
			}
		}
	
		file << maze_row << std::endl;
		maze_row.clear();
	}


	//vertices - in resources there are 4 coordinates for each vertex
	for(unsigned int i = 0; i < vertexArray->size(); i = i + 4){
		file << vertexArray->at(i) << " " << vertexArray->at(i + 1) << " " << vertexArray->at(i + 2) << " " << vertexArray->at(i + 3) << std::endl;
	}

return true;
} 

/*
Loads the maze from its saved file.
Sets x, y, type and name
*/
bool Maze::loadMaze(std::string filename){

	vertexArray->clear();
	mazeMap.clear();

	if(not PathFileExists(save_path.c_str()))
		CreateDirectory(save_path.c_str(), NULL);

	ifstream file;
	file.open(save_path+filename, ios::in);

	if(not file.is_open()){
		std::cout << "error opening  " << filename << " for reading.";
	return false;
	}

	//first line
	file >> this->x >> this->y >> this->type;
	mazeMap.resize(x * y);


	//map of maze
	std::string maze_row;
	std::getline(file, maze_row);   //read endline

	for(unsigned int row = 0; row < this->y; row++){
		std::getline(file, maze_row);
		 
		for(int column = this->x - 1; column >= 0; column--){
			
			if(maze_row[column] == '#'){
				mazeMap[row * x + column] = WALL;
			
			} else {
				mazeMap[row * x + column] = SPACE;
			}
			
		}
	
	}

	
	//vertices
	float tmp;
	while(file >> tmp)
		vertexArray->push_back(tmp);

	name = std::to_string(x) +"x"+ std::to_string(y) + "_" + type;

	
	std::cout << "Maze "+ name + " loaded.\n";

return true;

} 

/*
Prints mazeMap to std cout.
*/
void Maze::printMaze(){
	

		if(x > 50){
			std::cout << "Maze is too big to fit in here.\n";
		} else {

			//print maze to console from top row
			for(int i = y - 1; i >= 0; i--){
				for(unsigned int j = 0; j < x; j++){

					if(mazeMap[j + i * x] == WALL){
						std::cout << '#';
					} else {
						std::cout << ' ';
					}
				}
			std::cout<< '\n';
			}
		}
}