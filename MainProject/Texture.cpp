#include "Texture.h"

#include <glimg\glimg.h>




#include <iostream>


Sampler::Sampler(void){

	glGenSamplers(1, &id);

	//std::cout << "Sampler object constructed\n";
}


Sampler::~Sampler(void){
	glDeleteSamplers(1, &id);

	//std::cout << "Sampler object destructed\n";
}

void Sampler::bindToUnit(GLuint texture_unit){

	glBindSampler(texture_unit, id);

}

void Sampler::unbindFromUnit(GLuint texture_unit){

	glBindSampler(texture_unit, 0);

}


//////////////////////////////////
/////TEXTURE//////////
///////////////
///////
////
//


Texture::Texture(void) {//add dimension as parameter

	// Generate an OpenGL texture ID for this texture
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);  //this texture is forever 2D

	width = 0;
	height = 0;
	
}


Texture::~Texture(void){
	glDeleteTextures(1, &id);
}

bool Texture::loadFromFile(std::string filename, GLint data_format, GLint internal_format){
	using namespace glimg;
	ImageSet *imageSet;

	try {

		imageSet = glimg::loaders::stb::LoadFromFile(filename);
  
	} catch(glimg::loaders::stb::StbLoaderException &e) {
		std::cout << e.what() << std::endl;
		return false;
	}

	SingleImage image = imageSet->GetImage(0); //get image at mipmap level 0

	Dimensions dim = image.GetDimensions();
	ImageFormat format = image.GetFormat();

	bool expected_format = true;

	//want only 8bit 4 channel RGBA
	expected_format &= (format.Depth() == BD_PER_COMP_8);
	
	expected_format &= (format.Order() == ORDER_RGBA);

	expected_format &= (format.Components() == FMT_COLOR_RGBA);

	expected_format &= (format.Type() == DT_NORM_UNSIGNED_INTEGER);


	const GLvoid * data = image.GetImageData();


	if(expected_format){
		glTexImage2D(GL_TEXTURE_2D, 0, data_format, dim.width, dim.height, 0, internal_format, GL_UNSIGNED_BYTE, data);

		width = dim.width;
		height = dim.height;

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, id);

			glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

	} else return false;

return true;
}
