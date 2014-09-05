#pragma once

#include <glload/gl_3_3.h> //GLuint, glFunctions()
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>   //uint
#include <glutil/glutil.h>
#include "Texture.h"
#include "Shader.h"
#include "Control.h"


class Cursor
{
public:
	Cursor(float scale);
	void draw();

	GLuint colorUniform;
	GLuint UBO;
	GLuint VAO;
	GLuint VBO;

	//GLuint fontSampler;
	Texture texture;
	std::vector<GLfloat> data;

	GLuint program;
	GLuint sampler_id;
	Sampler *sampler;

	float scale;

	Control *control;

	~Cursor(void);
private:
	void initVAO();
	void uploadData();
	bool ready;		//if false skips initialization and drawing. 
};

