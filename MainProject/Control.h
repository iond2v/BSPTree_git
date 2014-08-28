#pragma once


//#include <glm/glm.hpp>    //uint
#include <glload/gl_3_3.h> //GLuint
#include <glutil/glutil.h>
#include <iostream>
#include <memory>

#include "Shader.h"

class Font;
class Text;
class Control;
class dynamicText;


#include "queryBuffer.h"

using namespace glm;

class Log {

public:
	Log(std::string filename);
	Log::~Log();

	std::ofstream logFile;
	std::string name;


	void append(std::string msg, bool print); 
	void done();

};

/*
class Error {

	Error() : error(0), check_everytime(false){
		errors = new Log("errorLog.txt");
	}

	~Error(){
		delete errors;
	}

	GLenum error;
	bool check_everytime;

	bool checkErrors();
	Log *errors;

};
*/

/*
Class for parsing and keeping program parameters.
Handles default values, parameter validation and stuff..
Not universal, but dont have to use boost for this..
*/
class Parameters {

public:
	Parameters(int argc, char **argv);
	unsigned int isPresent(std::string str);

	bool benchmark;
	bool generate;
	std::string type;
	bool go;
	unsigned int width;
	unsigned int depth;
	unsigned int draw_method;
	unsigned int maze_index;

	bool everything_ok;

private:
	int argc;
	char **argv;
};

class Control {

public:
	
	~Control(void);

	float windowWidth;
	float windowHeight;
	float aspectRatio;
	unsigned int frame_number;
	
	bool pause;
	bool stats;			//whether to display statistics
	bool help;			//whether to display help

	std::unique_ptr<Parameters> parameters;    //program parameters

	GLuint globalUniformBlockIndex;
	GLuint modelToWorldMatrixUniform;

	void addProgram(ShaderProgram *program);
	ShaderProgram *getProgram(std::string name);

	std::unique_ptr<Font> font;
	std::unique_ptr<Text> helpText;
	std::unique_ptr<bufferedQuery> timestampQuery;
	std::unique_ptr<bufferedQuery> timeElapsedQuery;

	std::unique_ptr<Log> runReport;
	float overallTreeDrawTime;

	glutil::MatrixStack perspectiveMatrix;
	glutil::MatrixStack orthographicMatrix;

	

	static Control* getInstance(){ 

		if(instance == nullptr){ 
			instance = new Control();
		}

		return instance;
	}

	
private:
	Control(void);
	static Control* instance;
	std::vector<ShaderProgram *> programs;

};

