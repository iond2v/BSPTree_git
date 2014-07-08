#include "Control.h"
#include "Font.h"

Control* Control::instance = nullptr;

/*
Opens file for appending.
*/
Log::Log(std::string filename){
	name = filename;

	logFile.open(name, std::ios::app);

}

/*
Just closes file if it's open.
*/
Log::~Log(){

if(logFile.is_open())
	logFile.close();

}

/*
If file is closed it is opened and message is appended.
*/
void Log::append(std::string msg, bool print){

	if(not logFile.is_open())
		logFile.open(name, std::ios::app);


	if(logFile.is_open())
		logFile << msg;

	if(print)
		std::cout << msg;

}


/*
Closes file. This is for when you won't be needing the file for a while.
*/
void Log::done(){

logFile.close();
}





Control::Control(void)
{
	overallTreeDrawTime = 0.0f;
	pause = false;
	stats = false;
	help = false;
}


Control::~Control(void)
{
	for(uint i = 0; i < programs.size(); i++){
		delete programs.at(i);	
	}
/*
	delete runReport;
	delete helpText;
	delete timeElapsedQuery;
	delete timestampQuery;
	delete font;*/
}

/*
Adds pointer to ShaderProgram to vector of programs.
*/
void Control::addProgram(ShaderProgram *program){
	programs.push_back(program);
}

/*
Returns pointer to ShaderProgram by name.
Currently have only 2 programs so this isn't so bad, but should be rethought/redone in future.
*/
ShaderProgram *Control::getProgram(std::string name){
	
	for(uint i = 0; i < programs.size(); i++){
		if(programs.at(i)->name == name)
			return programs.at(i);	
	}

return nullptr;
}


/*
message to log with this batch of errors.
forced forces error check.. Error checking is skipped otherwise.
  - should be called forced once per frame, if some error found, do check every time called.
*//*
bool Error::checkErrors(std::string message = "\n", bool forced = false, bool print = false){
	
	if(not forced)
		return false;

	error = glGetError();     // not this.. incorporate synchronous error messages instead

	//print message if error found
	if(error != 0)
		errors->append(message, print);

	//print errors under message
	while(error != 0){

		errors->append("    error code: "+std::to_string(error)+"\n", print);
		
		error = glGetError();

	}

}

*/