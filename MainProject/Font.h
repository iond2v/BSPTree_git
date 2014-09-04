#pragma once

#include <glload/gl_3_3.h> //GLuint, glFunctions()
#include <sstream>
#include <vector>
#include <memory>

#include <glm/glm.hpp>   //uint
#include <glutil/glutil.h>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>	//freetype
#include FT_FREETYPE_H

#include "Texture.h"
#include "Shader.h"

#include "Control.h"

#define not !

using namespace glm;


/*
Stores character parameters and geometry
+ mapping in texture atlas.
*/
class Character {

public:

	int advanceX, advanceY;
	int bearingX, bearingY;
	uint width, height;

	std::vector<GLfloat> vertexQuad; //sligtly misleading.. stores 2 triangles..
	std::vector<GLfloat> textureQuad; //sligtly misleading.. stores mapping for 2 triangles..

};


/*
Class for loading truetype fonts for use in Text and dynamicText.
*/
class Font {

public:
	Font(void);
	~Font(void);

	
	bool loadFont(std::string file, int size);  //creates font texture, characters and VAO + VBO
	void addProgram(GLuint prog); // is needed for getting location of sampler, colorUniform - used in Text class


	GLuint UBO;   //for toCamera transform.. currently set always identity

	FT_Library library;
	FT_Face typeface;


	std::vector<Character> characters;   //stores alphabet
	uint newLine;

	std::unique_ptr<Sampler> sampler;
	GLuint program;

	bool loaded;
	uint pixelSize;   //is a number used as size parameter when loading font.

	Texture texture;   //character atlas.. contains all glyph textures

};




/*
Class Text is meant as class for static text to be displayed with Font.
Font must be loaded or nothing will happen.
The message is kept but not used after creation of Text
*/
class Text { // issue when resized it stays fixed distance from bottom.. - reload function... rebuilds data + upload.., remember x, y, size

public:
	Text(Font *font, const std::string message, uint position_x, uint position_y, uint size);
	~Text();

	int msg_length;
	Font *font;
	GLuint colorUniform;

	bool ready();							//check if data/text is prepared for display - that is it's uploaded and UBO was changed from 0 at any point.
	void print();							//draws with VAO

protected:
	Text(Font *font);
	void addTextData(const std::string &text, uint windowHeight, 
		uint x, uint y, uint size);			//adds text geometry to data

	void initVAO();							//creates VAO + VBO and binds attribute arrays
	void uploadData();						//uploads text geometry from data to VBO + clears data vector

	bool uploaded;							//used in print to prevent unnecessary bother
	std::vector<GLfloat> data;				//temporary store for drawing data. Deleted after upload.

	GLuint VAO;
	GLuint VBO;
	GLuint UBO;								//for toCamera transform.. currently set always identity
	GLuint fontSampler;
};



/*
Extends class Text with property modified.
Creation of new text data and its upload to GPU is potentially
lot of work so the modified property helps to undergo this process
only if necessary.
*/
class dynamicText : public Text {  //idea - make use of SubData and some upper limit for message length - prevent constant reallocation?

public:
	dynamicText(Font *font);


	bool modified;

	/*
	want to call this directly but without making it public in base class Text which is static text and has no need to use it at all except once in constructor.
	This is to be manually called to construct text data from one or more parts on one or more locations on screen.
	*/
	void addTextData(const std::string &text, uint windowHeight, uint x, uint y, uint size){ 
	
		msg_length = text.size();

		Text::addTextData(text, windowHeight,  x,  y,  size);			//adds text geometry to data

		modified = true;
	}

	/*
	want to call this directly but without making it public in base class Text which is static text and has no need to use it at all except once in constructor.
	This is to be called before printing.
	*/
	void uploadData() {

		Text::uploadData();						//uploads text geometry from data to VBO + clears data vector
		modified = false;
	}



};



