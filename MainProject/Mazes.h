



class Maze
{
public:

	static int width;
	static int length;

	Maze(int width, int length){
	width = width;
	length = length;
	
	}

	bool *mazeMap;
	bool *mazeVisitedCell;

	
	bool * generateMaze(int width, int length);

};

