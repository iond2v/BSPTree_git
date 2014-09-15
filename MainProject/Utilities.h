#pragma once


#include <string>
#include <Shlwapi.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "basicPolygon.h"

#define not !

class Utilities
{
public:
	Utilities(void);
	~Utilities(void);



	bool addIndexedNormals(std::string filename);


};


