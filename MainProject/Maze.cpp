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



/*This creates static predefined maze.*//*
Maze::Maze(void){
	//std::cout << "Maze constructed\n";

	x = -1;
	y = -1;
	max_index = x * y - 1;

	srand((unsigned int) time(nullptr));

	generateMaze();
}
*/
/*This creates random maze with set parameters. Currently clamped to 80x80.*/
Maze::Maze(int width, int length) : x(width), y(length){

	mazeVisitedCell = nullptr;
	mazeMap = nullptr;

	x = glm::min(200, x);   //generation of bsp from this takes over 20 min
	y = glm::min(200, y);

	//std::cout << "Maze constructed\n";
	max_index = x * y - 1;
	srand((unsigned int) time(nullptr));
	generateMaze();
	
}

/*
Creates preset mazeMap.
*/
Maze::Maze(int number = 1){

	srand((unsigned int) time(nullptr));

	mazeVisitedCell = nullptr;
	mazeMap = nullptr;
	
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

	mazeMap = new bool[x * y];

	for(unsigned int i = 0; i <= max_index; i++){
		if(map[i] == '#'){
			mazeMap[max_index - i] = WALL;
		} else mazeMap[max_index - i] = SPACE;
	}

	printMaze();

}


//creates maze
void Maze::generateMaze(){
	
		if(x == -1 && y == -1){
			x = 17;
			y = 14;
			max_index = x * y - 1;

			mazeMap = new bool[x * y];
			mazeVisitedCell = new bool[x * y];

			std::fill(mazeMap, mazeMap + x * y * sizeof(bool), SPACE);
			std::fill(mazeVisitedCell, mazeVisitedCell + x * y * sizeof(bool), false);
	
			char map[] = {	//17 x 14
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
			"######## ########"};
			



			for(unsigned int i = 0; i <= max_index; i++){
				if(map[i] == '#'){
					mazeMap[max_index - i] = WALL;
				} else mazeMap[max_index - i] = SPACE;
			}
			printMaze();
		return;
		}
		


	mazeMap = new bool[x * y];
	mazeVisitedCell = new bool[x * y];

	std::fill(mazeMap, mazeMap + x * y * sizeof(bool), SPACE);
	std::fill(mazeVisitedCell, mazeVisitedCell + x * y * sizeof(bool), false);









	//put walls at borders and set 
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

	walkMaze(x / 2);

	printMaze();
		
}


//walks through maze space and fills it
void Maze::walkMaze(int start){
	
	//mazeMap[start] = SPACE; //maze entrance/exit
	doStep(start, NONE);
	
		for(unsigned int i = 0; i < y; i++){ //rows
			for(unsigned int j = 0; j < x; j++){ //columns

				//all currently unvisited cells must be walls
				if(mazeVisitedCell[j + i * x] == false){
					mazeMap[j + i * x] = WALL;
				}
				
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
vector<float> *Maze::getVertexArray(/*scale?*/){

	vector<GLfloat>* vertexArray = new vector<float>();

	vector<GLfloat> verticesOnlyArray;
	vector<GLfloat> colorsOnlyArray;

	
	
	for(unsigned int row = 0; row < y; row++){ //y - column => walk rows
		for(unsigned int column = 0; column < x; column++){
			
			if(mazeMap[column + row * x] == SPACE){
				modelAddItem(&verticesOnlyArray, &colorsOnlyArray, column, row, floorVertices, floorColors, sizeof(floorVertices));

				modelAddItem(&verticesOnlyArray, &colorsOnlyArray, column, row, wallCeilingVertices, wallCeilingColors, sizeof(wallCeilingVertices));

				//if to right is space/nothing  -> place left wall
				if(column + row * x + 1 < max_index && mazeMap[column + row * x + 1] == WALL)
					modelAddItem(&verticesOnlyArray, &colorsOnlyArray, column + 1, row, wallLeftVertices, wallLeftColors, sizeof(wallLeftColors));
	
				//if to left is space/nothing -> place wall
				if(column + row * x - 1 >= 0 && mazeMap[column + row * x - 1] == WALL)
					modelAddItem(&verticesOnlyArray, &colorsOnlyArray, column - 1, row, wallRightVertices, wallRightColors, sizeof(wallRightVertices));			

				//if up is space/nothing -> place wall
				if(column + row * x + x < max_index && mazeMap[column + row * x + x] == WALL)
					modelAddItem(&verticesOnlyArray, &colorsOnlyArray, column, row + 1, wallFrontVertices, wallFrontColors, sizeof(wallRightVertices));

				//if down is space/nothing -> place wall
				if(column + row * x - x >= 0 && mazeMap[column + row * x - x] == WALL)
					modelAddItem(&verticesOnlyArray, &colorsOnlyArray, column, row - 1, wallBackVertices, wallBackColors, sizeof(wallRightVertices));


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
	


//copy inside return vector allocated on heap
vertexArray->reserve(verticesOnlyArray.size() + colorsOnlyArray.size());
vertexArray->insert(vertexArray->end(), verticesOnlyArray.begin(), verticesOnlyArray.end());
vertexArray->insert(vertexArray->end(), colorsOnlyArray.begin(), colorsOnlyArray.end());

return vertexArray;
}

void Maze::modelAddItem(vector<float> *verticesOnlyArray, vector<float> *colorsOnlyArray, int column, int row, const float *vertices, const float *colors, int size){

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
	colorsOnlyArray->insert(colorsOnlyArray->end(), colors, colors + size / sizeof(float));
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

//marks current position and all positions between current and last as visited
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

void Maze::printMaze(){
	
	/*std::cout << "visited cells\n";

		//print visited cells to console from top row
		for(unsigned int i = y - 1; i >= 0; i--){
			for(unsigned int j = 0; j < x; j++){

				if(mazeVisitedCell[j + i * x] == true){
					std::cout << '.';
				} else if(mazeVisitedCell[j + i * x] == false){
					std::cout << 'x';
				}	
			}
		std::cout<< '\n';
		}

		std::cout<< '\n';
		std::cout<< '\n';
		*/

		std::cout << "created maze\n";

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