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


/*
Here happens parsing and whatever else needs to be done.
Defaults are no benchmark mode, no maze generation, no camera movement,
width of 22 and depth of 45 and draw method number 4 - PVS with queries.
*/
Parameters::Parameters(int argc, char **argv) : argc(argc), argv(argv){
	
	//status
	everything_ok = true;
	
	//parameters
	benchmark = false;
	generate = false;
	go = false;
	width = 22;
	depth = 45;
	type = "default";
	draw_method = 4;
	maze_index = 0;

	//helpers
	bool gotWidth = false;
	bool gotDepth = false;
	bool gotIndex = false;
	bool gotType = false;

	//new -> [generate|go] [ [index num] | [width num depth num [type "default"|"columns"]] ] [draw_method num] [benchmark]
	//alt -> [gen|go] [ [i num] | [w num d num [t "default"|"columns"]] ] [dm num] [b]

	unsigned int max_index_counter = 0;
	unsigned int position = 0;    //index of 0 is program name and marks not present argument

	if(isPresent("benchmark") != 0 || isPresent("b") != 0){
		benchmark = true;
		max_index_counter++;
	}


	if(isPresent("generate") != 0 || isPresent("gen") != 0){
		generate = true;
		max_index_counter++;		
	}

	if(isPresent("go") != 0){
		go = true;
		max_index_counter++;
	}


	if((position = isPresent("width")) != 0 || (position = isPresent("w")) != 0){
		
		//this is because we are potentially touching memory out of bounds
		if(argc - 1 >= max_index_counter + 2){
			width = atoi(argv[position + 1]);
			max_index_counter += 2;
			gotWidth = true;
		}

	}

	if((position = isPresent("depth")) != 0 || (position = isPresent("d")) != 0){

		if(argc - 1 >= max_index_counter + 2){
			depth = atoi(argv[position + 1]);
			max_index_counter += 2;
			gotDepth = true;
		}
	}

	if((position = isPresent("draw_method")) != 0 || (position = isPresent("dm")) != 0){
	
		if(argc - 1 >= max_index_counter + 2){
			draw_method = atoi(argv[position + 1]);
			max_index_counter += 2;
		}
	}

	if((position = isPresent("type")) != 0 || (position = isPresent("t")) != 0){
	
		if(argc - 1 >= max_index_counter + 2){
			type = argv[position + 1];
			max_index_counter += 2;
			gotType = true;
		}
	}

	if((position = isPresent("index")) != 0 || (position = isPresent("i")) != 0){
	
		if(argc - 1 >= max_index_counter + 2){
			maze_index = atoi(argv[position + 1]);
			max_index_counter += 2;
			gotIndex = true;
		}
	}

	//check if number of arguments fits number of found and expected arguments
	if(argc - 1 != max_index_counter){
		std::cout << "Number of arguments does not match the expected number of arguments.\n";
		std::cout << "Possible typo or ommission.\n";
		everything_ok = false;
	}

	//generate / go exclusivity
	if(generate && go){
		std::cout << "'generate' and 'go' parameters are mutually exclusive.\n";
		std::cout << "Proceeding as if only generate was set.\n";
			
	}

	//index / width/dept soft exclusivity
	if(gotIndex && (gotWidth || gotDepth)){
		std::cout << "'index' and 'width/depth' parameters are mutually exclusive.\n";
		std::cout << "Proceeding as if only index was set.\n";
			
	}

	//width/depth completeness
	if(not (gotWidth && gotDepth)){
		std::cout << "If 'depth' is present, 'width' parameter must be also present and vice versa.\n";
			
		if(not gotIndex)
			everything_ok = false;
	}


	if(gotType && not generate){
		std::cout << "Specifying type when not generating maze and BSPTree is without effect.\n";
		
	}


}


/*
Tests all argument values for match with string str.
When is comparison successfull it returns index to argv.
When unsuccessfull it returns 0.
*/
unsigned int Parameters::isPresent(std::string str){

	//can skip first as it is the name of the program
	for(unsigned int i = 1; i < argc; i++){
	
		if(std::strcmp(str.c_str(), argv[i]) == 0);
			return i;
	
	}

return 0;
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