#include "Maze.h"
#include <glload/gl_3_3.h>

#include <stdio.h>
#include <iostream>
#include <algorithm> 

#define WALL true
#define SPACE false

bool * generateMaze(int width, int length){
	
	//add minimal working dimensions check

	bool *mazeMap = new bool[width * length];
	bool *mazeVisitedCell = new bool[width * length];

	std::fill(mazeVisitedCell, mazeVisitedCell + width * length * sizeof(bool), false);

	//put walls at borders
		for(int i = 0; i < width; i++){ //columns
			for(int j = 0; j < length; j++){ //rows
				
				if(i == 0 || i == width - 1){

					mazeMap[i + j * length] = WALL;
					mazeVisitedCell[i + j * length] = true;

				} else if(j == 0 || j == length - 1){

					mazeMap[i + j * length] = WALL;
					mazeVisitedCell[i + j * length] = true;
				} else {

					mazeMap[i + j * length] = SPACE;
					mazeVisitedCell[i + j * length] = false;
				}

			}
		}




		makeStep();





		//print maze to console
		for(int i = 0; i < width; i++){
			for(int j = 0; j < length; j++){
				if(mazeMap[i + j * length] == WALL){
					std::cout << '#';
					}
				else {
					std::cout << '.';
					}
				
				}

			std::cout<< '\n';
			
			}
		std::cout<< '\n';
		std::cout<< '\n';

		//print visited cells to console
		for(int i = 0; i < width; i++){
			for(int j = 0; j < length; j++){
				if(mazeVisitedCell[i + j * length] == true){
					std::cout << '.';
					}
				else {
					std::cout << 'x';
					}
				
				}

			std::cout<< '\n';
			
			}

	delete[] mazeVisitedCell;
	return mazeMap;
	}