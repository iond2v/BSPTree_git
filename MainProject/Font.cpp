#include "Font.h"


Font::Font(void) {

	UBO = 0;

}


Font::~Font(void)
{

	
}

void Font::addProgram(GLuint prog){

program = prog;

}

inline int next_power2(int n){
	int res = 1; 

	while(res < n)
		res <<= 1; 

	return res;
}

/*
Uses freetype library to create set of textures, geometry and texture coordinates.
Geometry and texture coordinates are uploaded to GPU and appropriate attributes are enabled.
*/
bool Font::loadFont(std::string font_file, int size) {

	characters.clear(); //make sure it it empty..


	//Initializes the library..
	BOOL error = FT_Init_FreeType(&library);
	
	//Loads up the typeface
	error = FT_New_Face(library, font_file.c_str(), 0, &typeface);

	if(error)
		return false;
	
	//We want the result be of size
	//Still dont know how this precisely works.. 
	FT_Set_Char_Size(typeface, size * 72, size * 72, 300, 300); ///some problems with different fonts
	//FT_Set_Pixel_Sizes(typeface,  size, size);


	pixelSize = size;
	newLine = size + size / 2;
	
	GLuint textureWidth = 0;
	GLuint textureHeight = 0;

	
	int overallWidth = 0;
	int overallHeight = 0;

	for(uint i = 0; i < 128; i++){

		FT_Load_Glyph(typeface, FT_Get_Char_Index(typeface, i), FT_LOAD_DEFAULT);
		FT_Render_Glyph(typeface->glyph, FT_RENDER_MODE_NORMAL);

		//it sucks that I have to create all bitmaps in advance and then again
		overallHeight = glm::max((int)typeface->glyph->bitmap.rows, overallHeight);
		overallWidth = overallWidth + typeface->glyph->bitmap.width;
	}

	textureWidth = next_power2(overallWidth);
	textureHeight = next_power2(overallHeight);

	//note - there is some limitation of size of texture...


	//allocate enough space to contain the whole glyph atlas
	GLubyte* textureData = new GLubyte[textureWidth * textureHeight];

	std::fill(textureData, textureData + textureWidth * textureHeight, 0);
	uint xPosition = 0;


	//xPosition is changed at the end of this loop
	for(uint i = 0; i < 128; i++){

		FT_Load_Glyph(typeface, FT_Get_Char_Index(typeface, i), FT_LOAD_DEFAULT);
		FT_Render_Glyph(typeface->glyph, FT_RENDER_MODE_NORMAL);

		//Retrieve rendered bitmap from loaded glyph
		//it's a 8bit depth value
		FT_Bitmap* bitmap = &typeface->glyph->bitmap;

		uint bitmapWidth = bitmap->width;
		uint bitmapHeight = bitmap->rows;
					
		
		
		for(uint ch = 0; ch < bitmapHeight; ch++){
			for(uint cw = 0; cw < bitmapWidth; cw++){
			
				textureData[ch * textureWidth + cw + xPosition] = bitmap->buffer[(bitmapHeight - ch - 1) * bitmapWidth + cw];
						
			}
		}


		characters.push_back(Character());
		
	
		//bitmapWidth - glyph width
		//height - glyph height
		//advanceX - distance from origin to end of glyph space in direction of writing
		//			effectively the width of glyph space
		//advanceY - part of glyph under line
		//bearingX - distance from origin to left side of glyph
		//bearingY - distance from baseline to top of glyph
	
	
		//Record glyph metrics ----whats with that shift?? - it is division by 64 - some constant which has something to do with ppi/realsize something..
		characters[i].advanceX = typeface->glyph->advance.x >> 6;
		characters[i].bearingX = typeface->glyph->metrics.horiBearingX >> 6;
		characters[i].width = typeface->glyph->metrics.width >> 6;
	
		characters[i].advanceY = (typeface->glyph->metrics.height - typeface->glyph->metrics.horiBearingY) >> 6;
		characters[i].bearingY = typeface->glyph->metrics.horiBearingY >> 6;
		characters[i].height = typeface->glyph->metrics.height >> 6;
	
		////create square just for to char to fit
				
		/*quad format
			glm::vec2(0.0f, float(bitmapHeight - characters[i].advanceY)),  //left bottom
			glm::vec2(0.0f, float(- characters[i].advanceY)),				//left top
			glm::vec2(float(bitmapWidth), float(bitmapHeight - characters[i].advanceY)), //right bottom
			glm::vec2(float(bitmapWidth), float(- characters[i].advanceY))				//right top
		*/

		//two triangles
		//left bottom
		characters[i].vertexQuad.push_back(0.0f);
		characters[i].vertexQuad.push_back(float(bitmapHeight - characters[i].advanceY));

		//left top
		characters[i].vertexQuad.push_back(0.0f);
		characters[i].vertexQuad.push_back(float(- characters[i].advanceY));

		//right bottom
		characters[i].vertexQuad.push_back(float(bitmapWidth));
		characters[i].vertexQuad.push_back(float(bitmapHeight - characters[i].advanceY));

		
		//left top
		characters[i].vertexQuad.push_back(0.0f);
		characters[i].vertexQuad.push_back(float(- characters[i].advanceY));

		//right top
		characters[i].vertexQuad.push_back(float(bitmapWidth));
		characters[i].vertexQuad.push_back(float(- characters[i].advanceY));

		//right bottom
		characters[i].vertexQuad.push_back(float(bitmapWidth));
		characters[i].vertexQuad.push_back(float(bitmapHeight - characters[i].advanceY));


			
		/*quad form   		//that -1 is there because bitmap sizes start from 1 and there were artifacts

			glm::vec2( xPosition				/ ((float)textureWidth-1), bitmapHeight / (float)textureHeight),		//left bottom
			glm::vec2( xPosition				/ ((float)textureWidth-1), 0.0f),										//left top
			glm::vec2((xPosition + bitmapWidth-1) / ((float)textureWidth), bitmapHeight / (float)textureHeight),		//right bottom
			glm::vec2((xPosition + bitmapWidth-1) / ((float)textureWidth), 0.0f)										//right top
		*/

		//left bottom
		characters[i].textureQuad.push_back(xPosition / ((float)textureWidth-1));
		characters[i].textureQuad.push_back(bitmapHeight / (float)textureHeight);

		//left top
		characters[i].textureQuad.push_back(xPosition / ((float)textureWidth-1));
		characters[i].textureQuad.push_back(0.0f);

		//right bottom
		characters[i].textureQuad.push_back((xPosition + bitmapWidth-1) / (float)textureWidth);
		characters[i].textureQuad.push_back(bitmapHeight / (float)textureHeight);

		
		//left top
		characters[i].textureQuad.push_back(xPosition / ((float)textureWidth-1));
		characters[i].textureQuad.push_back(0.0f);

		//right top
		characters[i].textureQuad.push_back((xPosition + bitmapWidth-1) / (float)textureWidth);
		characters[i].textureQuad.push_back(0.0f);

		//right bottom
		characters[i].textureQuad.push_back((xPosition + bitmapWidth-1) / (float)textureWidth);
		characters[i].textureQuad.push_back(bitmapHeight / (float)textureHeight);

	xPosition = xPosition + bitmapWidth;
	}


	loaded = true;

	//
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, textureData);
	texture.width = textureWidth;
	texture.height = textureHeight;

	//probably dont need this array anymore
	delete[] textureData;

	//FT_Done_Face(typeface);
	//Destroy a given FreeType library object and all of its children,
	//including resources, drivers, faces, sizes, etc.
	FT_Done_FreeType(library);
	

	return true;
}




/////////////////////////////////////////////
////////////////Text/////////////
//////////////////////


/*Everything is handled in constructor; it's only waiting for print to be called.*/
Text::Text(Font *font, const std::string message, uint position_x, uint position_y, uint size) {

	UBO = 0;
	VAO = 0;
	VBO = 0;

	this->font = font;
	uploaded = false;

	if(not font->loaded)
		return;

	msg_length = message.size();

	uint height = Control::getInstance()->windowHeight;

	addTextData(message, height, position_x, position_y, size);

	initVAO();

	uploadData();

	//after this point we dont need nothing from parameters except Font?
	//maybe keep the message.. note - msg.size() is drawn...

	//get UBO from Font...
	UBO = font->UBO;

	fontSampler = glGetUniformLocation(font->program, "fontSampler");
	colorUniform = glGetUniformLocation(font->program, "colorUniform");
}  


bool Text::ready(){

	return (uploaded && UBO != 0);
}
	
	
/*
For every character in text puts its geometry and texture mapping into data vector.
*/
void Text::addTextData(const std::string &text, uint windowHeight, uint x = 10, uint y = 10, uint size = -1) {


	if(size == -1)
		size = font->pixelSize;

	float scale = float(size) / float(font->pixelSize);


		float currentX = x, currentY = windowHeight - y - font->pixelSize * scale;  //write from top


		//for each character in text
		for(uint i = 0; i < text.size(); i++) {

			//shift to start of new line
			if(text[i] == '\n'){
				currentX = (float)x;
				currentY -= font->newLine * scale;
				continue;
			}

			//get index of current character
			uint charIndex = text[i];
			currentX += font->characters[charIndex].bearingX * scale;

			if(text[i] != ' '){

				for(unsigned int s = 0; s < 6; s++){  //2 triangles to quad
					data.push_back(font->characters[charIndex].vertexQuad[s * 2] * scale + currentX);
					data.push_back(font->characters[charIndex].vertexQuad[s * 2 + 1] * scale + currentY);

					//texture mapping coordinates stay the same
					data.push_back(font->characters[charIndex].textureQuad[s * 2]);
					data.push_back(font->characters[charIndex].textureQuad[s * 2 + 1]);
				}

			}

			if(charIndex >= 48 && charIndex <= 57){  //draw numbers with fixed width
				currentX += font->pixelSize * scale;
			} else {
				currentX += (font->characters[charIndex].advanceX - font->characters[charIndex].bearingX) * scale;
			}

		}
	
}


void Text::initVAO(){


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_DYNAMIC_DRAW); 

	

		//layout (location = 0) in vec2 inPosition; - triangles
		//layout (location = 1) in vec2 inCoordinates; - texture
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*2 * 2, 0);  //x,y, tx,ty
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*2 * 2, (void*)(sizeof(GLfloat)*2));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


}

/*
Uploads data vector to GPU and clears it.
*/
void Text::uploadData(){


		glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_DYNAMIC_DRAW);
			//glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

			//glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(GLfloat), data.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	
		uploaded = true;

		data.clear();
		
		
}

void Text::print(){

	if(not uploaded)
		return;

	glDisable(GL_CULL_FACE);  //dont cull.. 
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);		//do blend
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, font->texture.id);


	glProgramUniform1i(font->program, fontSampler, 0);	//set GLSL sampler to sample from texure unit 0 - default

	glUniform4f( colorUniform, 1.0, 1.0, 1.0, 1.0); //use white color

	glBindBuffer(GL_UNIFORM_BUFFER, UBO);

	font->sampler->bindToUnit(0);


		glutil::MatrixStack camMatrix;

		//camMatrix.Scale(scale);
		camMatrix.SetIdentity();
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camMatrix.Top())); 
			
	
		glDrawArrays(GL_TRIANGLES, 0, msg_length * 6); //6 vertices per character


	font->sampler->unbindFromUnit(0);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	glBindVertexArray(0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);
}


/*Constructor to be used in dynamicText class.*/
Text::Text(Font *font) {

	UBO = 0;
	VAO = 0;
	VBO = 0;

	this->font = font;
	uploaded = false;

	if(font != nullptr && not font->loaded)
		return;

	uint height = Control::getInstance()->windowHeight;
	
	initVAO();

	//get UBO from Font...
	UBO = font->UBO;
		
	fontSampler = glGetUniformLocation(font->program, "fontSampler");
	colorUniform = glGetUniformLocation(font->program, "colorUniform");
}  

Text::~Text(){

if(VAO != 0)
	glDeleteBuffers(1, &VAO);

if(VBO != 0)
	glDeleteBuffers(1, &VBO);


}

////////////////////////////////////////////
/////////////dynamicText/////////
///////////////////////////

/*Uses minimal constructor of Text. *///should use own constructor and empty of Text??
dynamicText::dynamicText(Font *font) : Text(font){
	

}