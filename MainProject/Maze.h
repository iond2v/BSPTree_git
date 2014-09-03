#pragma once

#include <glload/gl_3_3.h>
#include <glm/glm.hpp>

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm> 
#include <time.h>
#include <vector>
#include <Windows.h>   //for CreateFolder
#include "Shlwapi.h"   //for PathFileExists
#include "Resources.h"

#define WALL true
#define SPACE false
#define STEP 2

#define not !

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define NONE 4

using namespace std;



class Maze {

public:
	int x;
	int y;
	int max_index;
	std::vector<bool> mazeMap;
	void printMaze();	//prints mazeMap into std cout
	void generateVertexArray(); 
	std::vector<float> *vertexArray;
	std::string name;

	bool saveMaze(); //saves current maze in file with map
	bool loadMaze(std::string filename); //loads map and vertex array

	/*This loads saved maze.*/
	Maze(std::string filename);

	/*This loads maze or if not available creates random maze with set parameters.*/
	Maze(int width, int length, std::string type);

	//This loads preset maze or generates new
	Maze(int number);

	~Maze(void){
		//std::cout << "Maze destructed\n";
		delete vertexArray;
	}



private:

	std::string type;
	std::vector<bool> mazeVisitedCell;
	static const std::string save_path;
	void generateMaze();
	
	void walkMaze(int start);
	void walkMazeColumns(int start);

	void doStep(int currentPosition, int fromDirection);

	void connectVisits(int currentPosition, int fromDirection);

	bool hasVisitableNeighbour(int currentPosition);
	bool canGoUp(int currentPosition);
	bool canGoDown(int currentPosition);
	bool canGoLeft(int currentPosition);
	bool canGoRight(int currentPosition);
	
	void modelAddItem(vector<GLfloat> *verticesOnlyArray, int column, int row, const float *vertices, int size);
};

