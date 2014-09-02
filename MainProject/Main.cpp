
#include <algorithm>
#include <string>
#include <vector>

#include <exception>
#include <stdexcept>
#include <string.h>
#include <cstdint> //print uint64
#include <memory>

#include <glload/gl_3_3.h>
#include <glload/gll.hpp>
#include <glutil/glutil.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>   



/*
basic initialization structure of program based on parts from http://www.arcsynthesis.org/gltut/index.html tutorial 
and its sources at https://bitbucket.org/alfonse/gltut/downloads

Learning Modern 3D Graphics Programming
Jason L. McKesson

Copyright © 2012 Jason L. McKesson 
*/

/////some links
//http://www.freetype.org/freetype2/docs/tutorial/step1.html
//http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=12

//http://www.opengl.org/registry/specs/ARB/timer_query.txt
//http://www.codeproject.com/Articles/30838/Overhauser-Catmull-Rom-Splines-for-Camera-Animatio - CPOL


#include "keyStates.h"
#include "Maze.h"
#include "Camera.h"
#include "BSPTreeCreator.h"
#include "BSPTree.h"
#include "Shader.h"
#include "fps_counter.h"
#include "Control.h"

#include "Font.h"



///* Setting Environment to working order

// as in http://glsdk.sourceforge.net/docs/html/pg_use.html
// built glsdk with help of premake utility http://industriousone.com/premake-quick-start in folder ..\GLSDK 0.4.4
// added to C/C++  Additional Include Directories: ..\GLSDK 0.4.4\glm;..\GLSDK 0.4.4\glutil\include;..\GLSDK 0.4.4\glmesh\include;..\GLSDK 0.4.4\glload\include;..\GLSDK 0.4.4\glimg\include;..\GLSDK 0.4.4\glfw\include;..\GLSDK 0.4.4\freeglut\include;..\freetype-2.5.1\include;%(AdditionalIncludeDirectories)
// added to Linker Additional Library Directories: ..\GLSDK 0.4.4\glutil\lib;..\GLSDK 0.4.4\glmesh\lib;..\GLSDK 0.4.4\glload\lib;..\GLSDK 0.4.4\glimg\lib;..\GLSDK 0.4.4\glfw\lib;..\GLSDK 0.4.4\freeglut\lib;..\freetype-2.5.1\objs\win32\vc2010;
// added to Linker Input Additional  Dependencies: glload.lib;glimg.lib;glutil.lib;glmesh.lib;freeglut.lib;glu32.lib;opengl32.lib;gdi32.lib;winmm.lib;user32.lib;freetype251.lib;
// with different versions for debug build:        glloadD.lib;glimgD.lib;glutilD.lib;glmeshD.lib;freeglutD.lib;glfwD.lib;glu32.lib;opengl32.lib;gdi32.lib;winmm.lib;user32.lib;freetype251_D.lib;

// added to C/C++ preprocessor definitions: /FREEGLUT_STATIC /_LIB //FREEGLUT_LIB_PRAGMAS=0


///* Eliminated some linker warnings

// /VERBOSE:LIB -- in linker command line shows process of searching for libraries   --not used
// /LTCG -- Link-Time Code Generation linker optimizations --not used
// /NODEFAULTLIB:MSVCRT  -- resolves: warning LNK4098: defaultlib 'MSVCRT' conflicts with use of other libs; use /NODEFAULTLIB:library

// ^(?([^\r\n])\s)*[^\s+?/]+[^\n]*$   <<-- regexp to count lines of code


//http://www.freetype.org/freetype2/docs/tutorial/step1.html

/*
glEnable/glDisable(GL_FRAMEBUFFER_SRGB)

Enables/disables the conversion from linear to sRGB. When this is enabled, 
colors written by the fragment shader to an sRGB image are assumed to be linear.
They are therefore converted into the sRGB colorspace. When this is disabled,
the colors written by the fragment shader are assumed to already be in the sRGB colorspace;
they are written exactly as given.

unsigned int defaults(unsigned int displayMode, int &width, int &height)
{
    return displayMode | GLUT_SRGB;
}
*/





using namespace glm;

void keyPressed(unsigned char key, int x, int y);


Camera camera = Camera();							//can set fzNear and fzFar - but constructor supplies default values

std::unique_ptr<Maze> maze;
std::unique_ptr<BSPTree> bspTree;									//all of data in BSP tree form

std::unique_ptr<keyStates> keys;			
std::unique_ptr<fps_counter> fps;

//maybe should be in control...
GLuint globalMatricesUBO;					
const int globalMatricesBindingIndex = 0;	

std::unique_ptr<Control> control;  //carry-all structure..



void APIENTRY DebugFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
			   const GLchar* message, GLvoid* userParam)
{
	std::string srcName;
	switch(source)
	{
	case GL_DEBUG_SOURCE_API_ARB: srcName = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: srcName = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: srcName = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: srcName = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB: srcName = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER_ARB: srcName = "Other"; break;
	}

	std::string errorType;
	switch(type)
	{
	case GL_DEBUG_TYPE_ERROR_ARB: errorType = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: errorType = "Deprecated Functionality"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: errorType = "Undefined Behavior"; break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB: errorType = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB: errorType = "Performance"; break;
	case GL_DEBUG_TYPE_OTHER_ARB: errorType = "Other"; break;
	}

	std::string typeSeverity;
	switch(severity)
	{
	case GL_DEBUG_SEVERITY_HIGH_ARB: typeSeverity = "High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB: typeSeverity = "Medium"; break;
	case GL_DEBUG_SEVERITY_LOW_ARB: typeSeverity = "Low"; break;
	}

	printf("%s from %s,\t%s priority\nMessage: %s\n",
		errorType.c_str(), srcName.c_str(), typeSeverity.c_str(), message);

	//Sleep(3000);//because error causes crash and I want to be able to read it..
}

/*
helper function to find out console window handle.
source: https://www.opengl.org/discussion_boards/showthread.php/174039-working-with-the-console-window
*/
HWND GetConsoleHwnd(void) {

	#define MY_BUFSIZE 1024 // Buffer size for console window titles.
	HWND hwndFound;         // This is what is returned to the caller.

	char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated WindowTitle.
	char pszOldWindowTitle[MY_BUFSIZE]; // Contains original WindowTitle.

	// Fetch current window title.
	GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);
 
	// Format a "unique" NewWindowTitle.
	wsprintf(pszNewWindowTitle,"%d/%d",
		GetTickCount(),
		GetCurrentProcessId());
 
	// Change current window title.
	SetConsoleTitle(pszNewWindowTitle);
 
	// Ensure window title has been updated.
	Sleep(40);
 
	// Look for NewWindowTitle.
	hwndFound=FindWindow(NULL, pszNewWindowTitle);
 
	// Restore original window title.
	SetConsoleTitle(pszOldWindowTitle);
 
	return(hwndFound);
}


/*
Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
Sets all program specific things. Text, shaders, maze, bsptree...
*/
void init(){
		
	//Create programs for perspective display of maze and orthographic display of text.
	{
		Shader Vshader(GL_VERTEX_SHADER, "../Shaders/MazeVS.vert");
		Shader Fshader(GL_FRAGMENT_SHADER, "../Shaders/MazeFS.frag");

		ShaderProgram *program = new ShaderProgram("maze");

		program->addShader(&Vshader);
		program->addShader(&Fshader);

		if(not program->linkProgram()){
			Sleep(3000);	//some time to read whats up
			exit(1);
		}

	control->addProgram(program);
	}

	{
		Shader Vshader(GL_VERTEX_SHADER, "../Shaders/FontVS.vert");
		Shader Fshader(GL_FRAGMENT_SHADER, "../Shaders/FontFS.frag");

		ShaderProgram *program = new ShaderProgram("font");

		program->addShader(&Vshader);
		program->addShader(&Fshader);

		if(not program->linkProgram()){
			Sleep(3000);	//some time to read whats up   -- not necessary? caught by DebugFunc?
			exit(1);
		}

	control->addProgram(program);
	}

	
	/* 
	//not necessary.. reshape gets called at beginning
	control->perspectiveMatrix.Perspective(45.0f, control->aspectRatio, camera.fzNear, camera.fzFar);    //perspective matrix
	control->orthographicMatrix.Orthographic(0, control->windowWidth, 0, control->windowHeight); 
	*/

	//create font
	control->font = std::unique_ptr<Font> (new Font());
	control->font->addProgram(control->getProgram("font")->id);  
	
	//create font sampler
	control->font->sampler = std::unique_ptr<Sampler>(new Sampler());

	glSamplerParameteri(control->font->sampler->id, GL_TEXTURE_MIN_LOD, 0);
	glSamplerParameteri(control->font->sampler->id, GL_TEXTURE_MAX_LOD, 0);
	
	glSamplerParameteri(control->font->sampler->id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //want to have sharp font
	glSamplerParameteri(control->font->sampler->id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glSamplerParameteri(control->font->sampler->id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(control->font->sampler->id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	control->modelToWorldMatrixUniform = glGetUniformLocation(control->getProgram("maze")->id, "modelToWorldMatrix");
	control->globalUniformBlockIndex = glGetUniformBlockIndex(control->getProgram("maze")->id, "GlobalMatrices");  //perspective, position of camera


	glUniformBlockBinding(control->getProgram("maze")->id, glGetUniformBlockIndex(control->getProgram("maze")->id, "GlobalMatrices"), globalMatricesBindingIndex);
	glUniformBlockBinding(control->getProgram("font")->id, glGetUniformBlockIndex(control->getProgram("font")->id, "GlobalMatrices"), globalMatricesBindingIndex);

		//create uniform buffer
		glGenBuffers(1, &globalMatricesUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, globalMatricesUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STREAM_DRAW);//sizeof GlobalMatrices struct in .vert
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//bind a range within a buffer object to an indexed buffer target
	//glBindBufferRange(GLenum 	target,  GLuint 	index_where, 	    GLuint buffer_from,  GLintptr offset, GLsizeiptr size);
	glBindBufferRange(GL_UNIFORM_BUFFER, globalMatricesBindingIndex, globalMatricesUBO, 0, sizeof(glm::mat4) * 2);

	control->font->UBO = globalMatricesUBO;  //put to constructor or something.. setFunction
	control->font->loadFont("../Fonts/TECHKR__.ttf", 20);

	//control->font->loadFont("../Fonts/r-technical_8.ttf", 64);  //different fonts behave differently..?
	

	keys = std::unique_ptr<keyStates>(keyStates::getInstance());
		

	control->runReport = std::unique_ptr<Log> (new Log("report.txt"));



	glEnable(GL_CULL_FACE);  
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	
	glEnable(GL_DEBUG_OUTPUT);

	glEnable(GL_DEPTH_TEST);   
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE); // true by default
	
	glEnable(GL_MULTISAMPLE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	std::string help = "Hi. This is help being helpful.\n"
		"You move around with with WSAD keys\n"
		"Holding SPACE puts you above ground level\n"
		"P toggles portal drawing\n"
		"1 switches to BSP Tree drawing method\n"
		"2 switches to BSP with queries\n"
		"3 switches to drawing static PVS\n"
		"4 switches to PVS with queries\n"
		"5 switches to drawing BSP by levels\n"
		"    + to go one level deeper in tree\n"
		"    - to go one level higher\n"
		"6 switches to drawing BSP from back to front, no depth test\n"
		"7 switches to drawing PVS from back of set, no depth test\n"
		"N skips all drawing - pause\n"
		"R restarts timer for recording camera movement\n"
		"E adds position with time to recorded camera movement\n"
		"G toggles following recorded movement\n";
	
	control->helpText = std::unique_ptr<Text> (new Text(control->font.get(), help, 10, 20, 12));

	fps = std::unique_ptr<fps_counter>(new fps_counter());



	//consider http://www.boost.org/doc/libs/1_56_0/doc/html/program_options/overview.html#idp344521056
	//or getopt?
		
	//new -> [go] [index num] | [width num depth num [type "default"|"columns"]] [draw_method num] [benchmark num]
	//alt -> [go] [i num] | [w num d num [t "default"|"columns"]]  [dm num] [b num]
	//                ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//									maze parameters
	//		 ^^^^											       ^^^^^^^^^^^^^^^^
	//	program parameters									      program parameters

	//implicit loading of saved PVS or maze with set name.. when file with that name is not found
	//new maze and/or PVS is generated.    --make forced mode?

/* ??????????
file

[maze]
generate | load  index num | width num depth num [type "default"|"columns"]
[PVS]
generate | load [draw_method num] [benchmark]



*/
	////////////////////////////////////////////when redoing parameters.. look at possibility of loading instead of generating maze
	std::string name;
		
	if(control->parameters->maze_index != 0){
		maze = std::unique_ptr<Maze> (new Maze(control->parameters->maze_index));	//this uses one of prepared designs..
	} else {
		maze = std::unique_ptr<Maze> (new Maze(control->parameters->width, control->parameters->depth, control->parameters->type));
	}

	//maze = std::unique_ptr<Maze>(new Maze(20, 20, "columns"));

	maze->generateVertexArray();
	maze->saveMaze(maze->name);
	

	{
	if(control->parameters->maze_index != 180)// dont want to overwrite it -- only because of that big maze and reproducibility.. not relevant now?
		BSPTreeCreator tree(maze->vertexArray, name);
	}	// to destruct tree creator immediately

	control->runReport->append("Generated "+name+".pvs\n\n", true);
		

	//creates/loads waypoints file for this maze
	camera.loadWaypoints("waypoints_"+name+".camera");
	
	if(control->parameters->benchmark)
		camera.convertToFrames(control->parameters->number_of_frames);

	cout << name << endl;

	float offset;
	if(maze->x % 2 == 0){
		offset = 0.0f;
	}else {
		offset = 0.5f;
	}

	


	bspTree = std::unique_ptr<BSPTree> (new BSPTree(	name, 
								vec3(-((float)maze->x / 2.0f) + offset, 0.0f, 1.5f), 
								glGetUniformLocation(control->getProgram("maze")->id, "colorUniform")));
	
	 
	//create maze texture sampler
	bspTree->samplerObject = std::unique_ptr<Sampler> (new Sampler());

	glSamplerParameteri(bspTree->samplerObject->id, GL_TEXTURE_MIN_LOD, 0);
	//glSamplerParameteri(bspTree->samplerObject->id, GL_TEXTURE_MAX_LOD, 0); //default should be 1000

	glSamplerParameteri(bspTree->samplerObject->id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  //and smooth texture..
	glSamplerParameteri(bspTree->samplerObject->id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 

	glSamplerParameteri(bspTree->samplerObject->id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(bspTree->samplerObject->id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	float largestAniso;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largestAniso);
	glSamplerParameteri(bspTree->samplerObject->id, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLint) largestAniso); //because why not..




	
	keys->setToggleable('p', true);
	keys->setToggleable('g', true);
	keys->setToggleable('n', true);
	keys->setToggleable('h', true);
	keys->setToggleable('i', true);

	
	//show help
	//keyPressed('h', 0, 0);
	//keys->release('h');

	switch(control->parameters->draw_method){
		case 1:
			bspTree->drawing_method = BSPTree::drawing_method_enum::BSP;
		break;
		case 2:
			bspTree->drawing_method = BSPTree::drawing_method_enum::BSP_QUERY;
		break;
		case 3:
			bspTree->drawing_method = BSPTree::drawing_method_enum::PVS;
		break;
		case 4:
			bspTree->drawing_method = BSPTree::drawing_method_enum::PVS_QUERY;
		break;
		case 5:
			bspTree->drawing_method = BSPTree::drawing_method_enum::NODES;
		break;
		case 6:
			bspTree->drawing_method = BSPTree::drawing_method_enum::BSP_BACK;
		break;
		case 7:
			bspTree->drawing_method = BSPTree::drawing_method_enum::PVS_BACK;
		break;

		default:
			bspTree->drawing_method = BSPTree::drawing_method_enum::PVS;
		break;
	
	
	
	}
	//set drawing with preferred method
	//bspTree->drawing_method = BSPTreeCreator::drawing_method_enum::BSP;

	//bspTree->drawing_method = BSPTree::drawing_method_enum::BSP_QUERY;
	
	//bspTree->drawing_method = BSPTree::drawing_method_enum::PVS;

	//bspTree->drawing_method = BSPTree::drawing_method_enum::PVS_QUERY;

	//bspTree->drawing_method = BSPTreeCreator::drawing_method_enum::BSP_BACK;
	//bspTree->drawing_method = BSPTree::drawing_method_enum::PVS_BACK;

	//start camera movement
	//keyPressed('g', 0, 0); 
	//keys->release('g');

	if(control->parameters->go){
		keyPressed('g', 0, 0); 
	    keys->release('g');
	}
	
	//keep cursor hidden and in the middle of the screen
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
	glutSetCursor(GLUT_CURSOR_NONE);


	//make creation of queries one of the last things before main loop
	control->timestampQuery = std::unique_ptr<bufferedQuery> (new bufferedQuery(GL_TIMESTAMP));
	control->timeElapsedQuery = std::unique_ptr<bufferedQuery> (new bufferedQuery(GL_TIME_ELAPSED));

}


//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void display(){


	if(control->pause){
		Sleep(100);
		return; // to keep callbacks running...
	}

	//timestamp query for fps computation
	glQueryCounter(control->timestampQuery->query(), GL_TIMESTAMP);


	//prepare for drawing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(keys->isPressed('g') && control->parameters->benchmark)				
		camera.loadNextPosition(fps->getFrameCount());		//now that camera position is dependent on frame number, the place is here
	
	bspTree->getNode(bspTree->RootNode, camera.position - bspTree->modelToWorldVector)->checkCollisions(camera);

	
	glutil::MatrixStack camMatrix;
	camMatrix.SetMatrix(Camera::CalcLookAtMatrix(camera.position, camera.target, glm::vec3(0.0f, 1.0f, 0.0f)));  //world to camera
	
	glBindBuffer(GL_UNIFORM_BUFFER, globalMatricesUBO);

		glBufferSubData(GL_UNIFORM_BUFFER, 0				, sizeof(glm::mat4), glm::value_ptr(control->perspectiveMatrix.Top()));

		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camMatrix.Top())); 
	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);



	control->getProgram("maze")->useProgram();

	glUniformMatrix4fv(control->modelToWorldMatrixUniform, 1, GL_FALSE, glm::value_ptr(bspTree->modelToWorldMatrix.Top()));  //model to world
	

	//measure time spent solely on drawing the tree..
	glBeginQuery(GL_TIME_ELAPSED, control->timeElapsedQuery->query());

		bspTree->draw(camera.position);

	glEndQuery(GL_TIME_ELAPSED);


	///////////////////////////////////
	/////////////done drawing now display text on screen
	control->getProgram("font")->useProgram();

		camMatrix.SetIdentity();

		glBindBuffer(GL_UNIFORM_BUFFER, globalMatricesUBO);

			glBufferSubData(GL_UNIFORM_BUFFER, 0				, sizeof(glm::mat4), glm::value_ptr(control->orthographicMatrix.Top()));

			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camMatrix.Top())); 
		
		glBindBuffer(GL_UNIFORM_BUFFER, 0);


		if(control->help)
			control->helpText->print();

		if(control->stats)
			fps->stats->print();

		if(bspTree->drawing_method == BSPTree::drawing_method_enum::NODES)
			bspTree->depthInfo->print();
				
		bspTree->drawCalls->print();

	glUseProgram(0);
		

	//get and add time spent drawing tree
	GLuint64 treeDrawTime;
	control->timeElapsedQuery->getResult(treeDrawTime);

	control->overallTreeDrawTime = control->overallTreeDrawTime + treeDrawTime / 1000000.0f;

	//get result of previous timestamp query
	control->timestampQuery->getResult(fps->current_time);

	//update frame count and fps values
	//+ dynamic info text if enabled in control.
	fps->update();

	control->frame_number = fps->getFrameCount();

	glutSwapBuffers();
	glutPostRedisplay();//want to redraw immediately
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape (int w, int h) {

	glViewport(0, 0, (GLsizei) w, (GLsizei) h); //This tells OpenGL what area of the available area we are rendering to
	
	control->windowHeight = (float)h;
	control->windowWidth = (float)w;
	control->aspectRatio = control->windowWidth / control->windowHeight;

	//recompute the projection matrices
	glutil::MatrixStack newOrthographicMatrix, newPerspectiveMatrix;

	newOrthographicMatrix.Orthographic(0, control->windowWidth, 0, control->windowHeight);
	newPerspectiveMatrix.Perspective(45.0f, control->aspectRatio, camera.fzNear, camera.fzFar);

	control->orthographicMatrix.SetMatrix(newOrthographicMatrix.Top());
	control->perspectiveMatrix.SetMatrix(newPerspectiveMatrix.Top()); 

glutPostRedisplay();
}

/*
Called whenever a key on the keyboard was pressed.
The key is given by the ''key'' parameter, which is in ASCII.
It's often a good idea to have the escape key (ASCII value 27) call glutLeaveMainLoop() to 
exit the program.

Here should be only actions which happen only upon key press e.g. toggles/state settings/else..
*/
void keyPressed(unsigned char key, int x, int y) {

	//cout << "pressed: " << uint(key) << " - " << key << endl;

	if(key == 27){ //escape quits
		glutLeaveMainLoop();
		std::cout << "Leaving main loop.\n";
	}

	keys->press(key);


	//p key sets whether to draw portals
	if(keys->isToggled('p')){
		bspTree->drawPortals = true;

	} else {
		bspTree->drawPortals = false;
	}




	//set drawing mode
	if(key == '1'){
		bspTree->drawing_method = BSPTree::drawing_method_enum::BSP;
		glDepthFunc(GL_LEQUAL);
	}

	if(key == '2'){
		bspTree->drawing_method = BSPTree::drawing_method_enum::BSP_QUERY;
		glDepthFunc(GL_LEQUAL);
	}

	if(key == '3'){
		bspTree->drawing_method = BSPTree::drawing_method_enum::PVS;
		glDepthFunc(GL_LEQUAL);
	}

	if(key == '4'){
		bspTree->drawing_method = BSPTree::drawing_method_enum::PVS_QUERY;
		glDepthFunc(GL_LEQUAL);
	}

	if(key == '5'){
		bspTree->drawing_method = BSPTree::drawing_method_enum::NODES;
		glDepthFunc(GL_LEQUAL);
	}
	
	if(key == '6'){
		bspTree->drawing_method = BSPTree::drawing_method_enum::BSP_BACK;
		glDepthFunc(GL_ALWAYS);
	}

	if(key == '7'){
		bspTree->drawing_method = BSPTree::drawing_method_enum::PVS_BACK;
		glDepthFunc(GL_ALWAYS);
	}

	if(key == '+'){
		bspTree->depth = bspTree->depth + 1;
		if(bspTree->depth > bspTree->max_depth)
			bspTree->depth = bspTree->max_depth;

	Control *control = Control::getInstance();

	bspTree->depthInfo->addTextData("Depth: "+std::to_string(bspTree->depth)+"/"+std::to_string(bspTree->max_depth), control->windowHeight, control->windowWidth - 180, 20, 15);
	bspTree->depthInfo->uploadData();
	}

	if(key == '-'){
		bspTree->depth = bspTree->depth - 1;
		if(bspTree->depth < 0)
			bspTree->depth = 0;

	Control *control = Control::getInstance();

	bspTree->depthInfo->addTextData("Depth: "+std::to_string(bspTree->depth)+"/"+std::to_string(bspTree->max_depth), control->windowHeight, control->windowWidth - 180, 20, 15);
	bspTree->depthInfo->uploadData();
	}


	//n pauses drawing
	if(keys->isToggled('n')){
		control->pause = true;

	} else {
		control->pause = false;
	}

	//h shows help text   //exclusive with statistics
	if(key == 'h'){
		control->help = true;

		if(keys->isToggled('i')){
			control->stats = false;
			keys->setToggle('i', false);
		}
		if(not keys->isToggled('h'))
			control->help = false;
	}

	//i shows statistics
	if(key == 'i'){
		control->stats = true;

		if(keys->isToggled('h')){
			control->help = false;
			keys->setToggle('h', false);
		}

		if(not keys->isToggled('i'))
			control->stats = false;

	}

	if(key == ' '){  //move vertically
		camera.height = 5.0f;
		camera.moveRight(0.0f);
	}

	if(key == 'g'){  //set starting time
		camera.movement.start_time.time = clock();
		
		if(camera.movement.times.size() == 0){
			std::cout << "No data points available\n";
			keys->swapToggle('g');
			keys->release('g');
		}
	}

	if(key == 'r'){
		camera.movement.start_time.time = clock();  //reset time starting point
	}

	
	if(key == 'e'){
		camera.recordWaypoint();
	}

	if(key == 'q'){
		vec3 pos = camera.position - bspTree->modelToWorldVector;
		int ID = bspTree->getNode(bspTree->RootNode, pos)->Id;
		cout << pos.x << " " << pos.y << " " << pos.z << " in " << ID << endl;
	}
	
}

void keyUp(unsigned char key, int x, int y){
		
	if(key == ' '){  //move vertically
		
		camera.height = 0.0f;
		camera.moveRight(0.0f);

	}

	keys->release(key);	
}



/*
here should be only periodic actions which check for pressed keys.
*/
void keyActions(int value){

glutTimerFunc(10, keyActions, 0);   //periodically call this function

	if(keys->isPressed('w'))
		camera.moveForward(1.0f);

	if(keys->isPressed('s'))
		camera.moveBack(1.0f);
	
	if(keys->isPressed('a'))
		camera.moveLeft(1.0f);

	if(keys->isPressed('d'))
		camera.moveRight(1.0f);


	
	
	if(keys->isPressed('g') && not control->parameters->benchmark){			
		float current_travel_time = time_item().seconds(camera.movement.start_time);
		camera.loadNextPosition(current_travel_time);
	}
	
//glutPostRedisplay();
}

/*
Manages camera rotation angles, keeps cursor at centre of the screen and calls camera.updateTarget();
*/
void mouse(int x, int y){

	int centerX = control->windowWidth / 2;
	int centerY = control->windowHeight / 2;

     int deltaX =  x - centerX;
     int deltaY =  y - centerY;

     if(deltaX != 0.0f || deltaY != 0.0f) {

		 camera.angleSIDE += deltaX * camera.mouse_speed; // rotation speed
		 camera.angleUP += deltaY * camera.mouse_speed;

		 camera.angleUP = clamp(camera.angleUP, -89.0f, 89.0f); //straight up/down angles are bad.. m'kay

		 //to not spin out of range
		 if(camera.angleSIDE > 360.0f){
			camera.angleSIDE -= 360.0f;
		 } else if(camera.angleSIDE < -360.0f){
			camera.angleSIDE += 360.0f;		 
		 }
		 
		 camera.updateTarget();

     glutWarpPointer(centerX, centerY);
     }

//glutPostRedisplay();
}



int main(int argc, char** argv){

	glutInit(&argc, argv);

	
	control = std::unique_ptr<Control> (Control::getInstance());

	control->parameters = std::unique_ptr<Parameters> (new Parameters(argc, argv));

	if(not control->parameters->everything_ok){
		Sleep(5000);
		return 1;
	}

	/*
	GLUT_SCREEN_WIDTH
	Width of the screen in pixels. Zero indicates the width is unknown or not available.
	GLUT_SCREEN_HEIGHT
	Height of the screen in pixels. Zero indicates the height is unknown or not available.
	*/

	//get screen resolution
	int max_screen_width = glutGet(GLUT_SCREEN_WIDTH);
	int max_screen_height = glutGet(GLUT_SCREEN_HEIGHT);
	//std::cout << "Resolution is " << max_screen_width << "x" << max_screen_height << std::endl;

	//in case the size is not available, guess..
	if(max_screen_width == 0)
		max_screen_width = 1280;

	if(max_screen_height == 0)
		max_screen_height = 1024;

	//set sizes of console and window to be big and organized.
	int border_space = 25;
	int console_width = 450;
	int console_heigth = max_screen_height - 2 * border_space;
	int console_position_x = max_screen_width - border_space - console_width;
	int console_position_y = border_space;

	control->windowWidth = max_screen_width - 2 * border_space - console_width;
	control->windowHeight = max_screen_height - 2 * border_space - 40;      //glut probably takes this as window screen size without top panel/bar/something...
	int window_position_x = border_space;
	int window_position_y = border_space;


	/*
	 GLUT_RGBA - Bit mask to select an RGBA mode window. This is the default if neither  GLUT_RGBA nor  GLUT_INDEX are specified.
	 GLUT_DOUBLE - Bit mask to select a double buffered window.
	 GLUT_ALPHA - Bit mask to select a window with an alpha component to the color buffer(s).
	 GLUT_DEPTH - Bit mask to select a window with a depth buffer.
	 GLUT_MULTISAMPLE - Bit mask to select a window with multisampling support. If multisampling is not available, a non-multisampling window will automatically be chosen
	 */

	unsigned int displayMode = GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE;

	glutInitDisplayMode(displayMode);		
	glutInitContextVersion(4, 3);    //to not crash in case of ancient HW


	glutInitContextProfile(GLUT_CORE_PROFILE); //The core profile also disables all functions from older versions from being used
	glutInitContextFlags(GLUT_DEBUG);  //

	//set window parameters in glut
	glutInitWindowSize((int)control->windowWidth, (int)control->windowHeight); 
	glutInitWindowPosition(window_position_x, window_position_y);
	
	//position console window and create display window.
	SetWindowPos(GetConsoleHwnd(), NULL, console_position_x, console_position_y, console_width, console_heigth, 0);
	int window = glutCreateWindow(argv[0]);  //creates context.. but with higest available version...

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);


	glload::LoadFunctions();

	//create debug message callback..   
	if(glDebugMessageCallback){
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(DebugFunc, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
	}

	//get version the old way..
	std::string version((char *)glGetString(GL_VERSION));
	 
	//and check if able to run this properly.. 
	if(not (version >= string("3.3.0"))) {

		printf("Your OpenGL version is %i.%i\n You must have at least OpenGL 3.3 to run this.\n",
			glload::GetMajorVersion(), glload::GetMinorVersion());
		Sleep(5000);
		glutDestroyWindow(window);
		return 0;

	} else {
		printf("Supported OpenGL version %i.%i\n",	glload::GetMajorVersion(), glload::GetMinorVersion());
	}

		




	//sets and constructs everything else..
	init();
	


	//register glut callbacks
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);

	glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses  
	glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events  
	glutTimerFunc(30, keyActions, 0);	//check keys in 30ms, gets reregistered in 10ms intervals afterwards

	//cant register member function as callback unless it's static - mouse function isnt part of camera..
	glutPassiveMotionFunc(mouse);


	/////////////////////////////////////////////////////
	////main loop and post main loop logic and reports.

	time_item start(clock(), "Starting"); //there is probably some overhead when starting main loop.. so it gets more accurate with time..

		glutMainLoop();

	time_item end(clock(), "Ending");

	///some reporting and logging after the end of main loop.
	float timeSpentDrawing = control->overallTreeDrawTime / 1000.0f;
	float duration = end.seconds(start);


	control->runReport->append(std::to_string(duration)+ " seconds elapsed since start of main loop.\n", false);
	control->runReport->append(std::to_string(timeSpentDrawing)+ " seconds spent drawing the tree structure.\n", false);
	control->runReport->append("That is "+ std::to_string(timeSpentDrawing / duration * 100.0f)+ "% of overall display time.\n", false);

	
	control->runReport->append("BSP tree nodes: "+ std::to_string(bspTree->node_count)+"\n", false);
	control->runReport->append("BSP tree leaves: "+ std::to_string(bspTree->leaves)+"\n", false);
	control->runReport->append("BSP tree maximum depth: "+ std::to_string(bspTree->max_depth)+"\n", false);

	control->runReport->append("BSP avg draw calls: "+ std::to_string((float) bspTree->draw_calls_sum_with_queries / bspTree->draw_cycles)+" = "+		//all draw calls per draw (regular + for queries)
														std::to_string((float) bspTree->draw_calls_sum / bspTree->draw_cycles)+ " + "+		//only draw calls
														 std::to_string((float) (bspTree->draw_calls_sum_with_queries - bspTree->draw_calls_sum) / bspTree->draw_cycles)+"\n\n", false);	//only draws for occlusion queries

	control->runReport->append("BSP min draw calls: "+ std::to_string((float) bspTree->min_draw_calls_with_query)+" = "+
														std::to_string(bspTree->min_draw_calls) + " + "+
														  std::to_string(bspTree->min_draw_calls_with_query - bspTree->min_draw_calls)+"\n", false);

	control->runReport->append("BSP max draw calls: "+ std::to_string((float) bspTree->max_draw_calls_with_query)+" = "+
														std::to_string(bspTree->max_draw_calls) + " + "+
														  std::to_string(bspTree->max_draw_calls_with_query - bspTree->max_draw_calls)+"\n", false);//std::to_string(bspTree->max_draw_calls) + "("+ std::to_string((float) bspTree->max_draw_calls_with_query)+")\n");

	control->runReport->append("Overall samples passed: "+ std::to_string(bspTree->samples_passed_sum)+"\n", false);

	control->runReport->append("Overdraw: "+std::to_string(bspTree->samples_passed_sum / bspTree->draw_cycles / (control->windowHeight * control->windowWidth) * 100.0f)+"%\n", false);

	std::string msg = 
		"\nAvg FPS: "+std::to_string(fps->getAvgFps())+
		"\nAvg render time: "+std::to_string(fps->getAvgRenderTime())+"ms\n"+
		"Frames rendered: "+std::to_string(fps->getFrameCount())+"\n"+
		"________________________________________________\n";

	control->runReport->append(msg, false);
	msg.clear();

	for(unsigned int i = 0; i < fps->frame_time_slots.size(); i++){
		msg.append(fps->frame_time_slots[i].slot_name + std::to_string(fps->frame_time_slots[i].value) + "\n");
	
	}

	control->runReport->append(msg, false);

	std::cout << "Exiting." << std::endl;
	

	Sleep(1000);


	return 0;
}