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
Defaults are no benchmark mode, no camera movement,
maze width of 22 and depth of 45, type default and draw method number 4 - PVS with queries.

new -> [go] [index num] | [width num depth num [type "default"|"columns"]] [draw_method (1-7)] [benchmark num]
alt -> [go] [i num] | [w num d num [t "default"|"columns"]]  [dm num] [b num]
    

Implicit loading of saved PVS or maze with set name.. when file with that name is not found
new maze and/or PVS is generated.
*/
Parameters::Parameters(int argc, char **argv) : argc(argc), argv(argv){
	
	//status
	everything_ok = true;
	
	//parameters
	benchmark = false;
	number_of_frames = 10000;
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

	//for validation
	std::vector<std::string> valid_types;
	valid_types.push_back("default");
	valid_types.push_back("columns");
	unsigned int max_draw_method_index = 7;
	unsigned int min_draw_method_index = 1;
	unsigned int minimal_width = 3;
	unsigned int minimal_depth = 3;
	unsigned int min_number_of_frames = 1;




	unsigned int max_index_counter = 0;
	unsigned int position = 0;    //index of 0 is program name and marks not present argument

	if(isPresent("benchmark") != 0 || isPresent("b") != 0){
		if(argc - 1 >= max_index_counter + 2){
			benchmark = true;
			
			if(isNumber(argv[position + 1], strlen(argv[position + 1]))){  		//this is because we are potentially touching memory out of bounds
				number_of_frames = atoi(argv[position + 1]);
				
			} else {
				std::cout << "benchmark parameter has non numeric part.\n";
				everything_ok = false;
			}

			max_index_counter += 2;
		}
	}


	if(isPresent("go") != 0){
		go = true;
		max_index_counter++;
	}


	if((position = isPresent("width")) != 0 || (position = isPresent("w")) != 0){
		
		if(argc - 1 >= max_index_counter + 2){
			gotWidth = true;

			if(isNumber(argv[position + 1], strlen(argv[position + 1]))){
				width = atoi(argv[position + 1]);
			} else {
				std::cout << "width parameter has non numeric part.\n";
				everything_ok = false;
			}

			max_index_counter += 2;
		}

	}

	if((position = isPresent("depth")) != 0 || (position = isPresent("d")) != 0){

		if(argc - 1 >= max_index_counter + 2){
			gotDepth = true;

			if(isNumber(argv[position + 1], strlen(argv[position + 1]))){
				depth = atoi(argv[position + 1]);
			} else {
				std::cout << "depth parameter has non numeric part.\n";
				everything_ok = false;
			}

			max_index_counter += 2;
		}
	}

	if((position = isPresent("draw_method")) != 0 || (position = isPresent("dm")) != 0){
	
		if(argc - 1 >= max_index_counter + 2){
			if(isNumber(argv[position + 1], strlen(argv[position + 1]))){
				draw_method = atoi(argv[position + 1]);
			} else {
				std::cout << "draw_method parameter has non numeric part.\n";
				everything_ok = false;
			}

			max_index_counter += 2;
		}
	}

	if((position = isPresent("type")) != 0 || (position = isPresent("t")) != 0){
	
		if(argc - 1 >= max_index_counter + 2){
			gotType = true;

			if(std::find(valid_types.begin(), valid_types.end(), argv[position + 1]) != valid_types.end()){
				type = argv[position + 1];
			} else {
				std::cout << "type parameter has not accepted value " << argv[position + 1] << std::endl;
				everything_ok = false;
			}
			max_index_counter += 2;
			
		}
	}

	if((position = isPresent("index")) != 0 || (position = isPresent("i")) != 0){
	
		if(argc - 1 >= max_index_counter + 2){
			gotIndex = true;

			if(isNumber(argv[position + 1], strlen(argv[position + 1]))){
				maze_index = atoi(argv[position + 1]);
			} else {
				std::cout << "index parameter has parameter has non numeric part.\n";
				everything_ok = false;
			}
			max_index_counter += 2;
		}
	}


	////some more validation



	if(draw_method > max_draw_method_index || draw_method < min_draw_method_index){
		std::cout << "draw_method is out of valid range (1-7)\n";
		everything_ok = false;
	}

	if(width < minimal_width){
		std::cout << "width is below minimal value (3)\n";
		everything_ok = false;
	}
	
	if(width < minimal_depth){
		std::cout << "depth is below minimal value (3)\n";
		everything_ok = false;
	}

	if(number_of_frames < min_number_of_frames){
		std::cout << "value of benchmark parameter number_of_frames is below minimal value (1)\n";
		everything_ok = false;
	}

	//check if number of arguments fits number of found and expected arguments
	if(argc - 1 != max_index_counter){
		std::cout << "Number of arguments does not match the expected number of arguments.\n";
		std::cout << "Possible typo or ommission.\n";
		everything_ok = false;
	}

	
	//index / width/dept soft exclusivity
	if(gotIndex && (gotWidth || gotDepth)){
		std::cout << "'index' and 'width/depth' parameters are mutually exclusive.\n";
		std::cout << "Proceeding as if only index was set.\n";
			
	}

	//width/depth completeness
	if((gotWidth != gotDepth)){
		std::cout << "If 'depth' is present, 'width' parameter must be also present and vice versa.\n";
			
		if(not gotIndex)
			everything_ok = false;
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
	
		if(std::strcmp(str.c_str(), argv[i]) == 0)
			return i;
	
	}

return 0;
}

bool Parameters::isNumber(char *str, unsigned int length){

	for(unsigned int i = 0; i < length; i++){
		if(not std::isdigit(str[i]))
			return false;
	}

return true;
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