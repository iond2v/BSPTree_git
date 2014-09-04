#pragma once

#include <glload/gl_3_3.h> //GLuint, glFunctions()
#include <string>

class Sampler
{
public:
	Sampler(void);
	~Sampler(void);

	GLuint id;
	void bindToUnit(GLuint texture_unit);
	void unbindFromUnit(GLuint texture_unit);

};



class Texture
{
public:
	Texture(void);
	~Texture(void);


	GLuint id;
	GLuint width;
	GLuint height;

	bool loadFromFile(std::string filename, GLint data_format, GLint internal_format);

};

