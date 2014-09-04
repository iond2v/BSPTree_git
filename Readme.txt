About

This is basically my bachelor's degree project to pet/learning project. 
It can generate simple mazes, put it into BSP tree and compute PVS for its leaves.
Both BSP tree and its PVS are saved and can be loaded.
It can also record camera positions and then interpolate camera movement from them in the next run.
Supports several modes of drawing and writes statistics after each run.


Uses GLSDK and Freetype libraries. These are required to build.
http://glsdk.sourceforge.net/docs/html/index.html, namely components GL Load,
GL Utility, OpenGL Mathematics and FreeGLUT.
FreeType http://www.freetype.org.
Additionally Visual C++ Redistributable Packages are required too http://www.microsoft.com/en-us/download/details.aspx?id=30679
This is project for Windows and OpenGl 3.3+.

Instructions for running and controlling the application
   When running, help can be displayed by pressing 'h'.
   Run time statistics can be displayed by 'i'.

   You move around with with WSAD keys
   Holding SPACE puts you above ground level - currently it's not possible to get above maze because of collision handling.
   P toggles portal drawing
   1 switches to BSP Tree drawing method
   2 switches to BSP with queries
   3 switches to drawing static PVS
   4 switches to PVS with queries
   5 switches to drawing BSP by levels
		+ to go one level deeper in tree
		- to go one level higher
   6 switches to drawing BSP from back to front, no depth test
   7 switches to drawing PVS from back of set, no depth test
   N skips all drawing - pause
   R restarts timer for recording camera movement
   E adds position with time to recorded camera movement
   G toggles following recorded movement

Note
   When recording camera waypoints it is only possible to add waypoints to prevent
   involuntary overwriting of previous data.
   For playing back recorded movement it is neccessary to re-run the application.
   Camera position is computed from the frame number and those can't be reset.
		-this is for benchmarking purposes..


Application parameters

	new -> [go] [index num] | [width num depth num [type "default"|"columns"]] [draw_method (1-7)] [benchmark num]
	alt -> [go] [i num] | [w num d num [t "default"|"columns"]]  [dm num] [b num]
   
   Parameters can be in random order but pairs of 'argument value' must be respected. There is some 
   validity checking. All parameters have some default value so all should be more or less optional.
   draw_method is starting mode of drawing. Numbers are the same as written above.
   
   Maze indexes
   1 - 17x14
   2 - 20x15
   3 - 30x30
   4 - 60x60
   5 - 50x50
   6 - 31x31
   7 - 180x180
	
   All indexed mazes with exception of no.7 are hard coded. See file Maze.cpp.
   appname is name of the exe file.

   go - runs camera movement immediately if previously recorded.

   index num / i num - generates (or loads if present) maze with set index. 
		If set it overrides width, depth and type parameters.

   width num / w num - sets width of maze to be loaded / generated.

   depth num / d num - sets depth of maze to be loaded / generated.

   type "string" / t "string"- sets type of maze to generate / load. 
		types are default - regular maze with branching corridors.
				  columns - one great room filled with "columns".

   draw_method num / dm num - sets the starting draw method as described above in help.

   benchmark num / b num - makes camera movement dependent on frame number instead of time.

