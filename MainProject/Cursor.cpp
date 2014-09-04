#include "Cursor.h"


Cursor::Cursor(float scale) : scale(scale)
{
	control = Control::getInstance();

	texture.loadFromFile("../Textures/cursor.png", GL_RGBA, GL_RGBA);


	//two triangles
		//left bottom
		//left top
		//right bottom
		
		//left top
		//right top
		//right bottom
		
	//x, y, tx, ty
	data.push_back(0.0f);
	data.push_back(0.0f);
	data.push_back(0.0f);
	data.push_back(0.0f);

	data.push_back(0.0f);
	data.push_back(texture.height);
	data.push_back(0.0f);
	data.push_back(1.0f);
		
	data.push_back(texture.width);
	data.push_back(0.0f);
	data.push_back(1.0f);
	data.push_back(0.0f);

	//second triangle
	data.push_back(0.0f);
	data.push_back(texture.height);
	data.push_back(0.0f);
	data.push_back(1.0f);

	data.push_back(texture.width);
	data.push_back(texture.height);
	data.push_back(1.0f);
	data.push_back(1.0f);

	data.push_back(texture.width);
	data.push_back(0.0f);
	data.push_back(1.0f);
	data.push_back(0.0f);
	
	initVAO();
	uploadData();
}


Cursor::~Cursor(void)
{
	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);
}


void Cursor::initVAO(){


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW); 

	

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
void Cursor::uploadData(){


		glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
			
		//data.clear();
}

void Cursor::draw(){

	

	glDisable(GL_CULL_FACE);  //dont cull.. 
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);		//do blend
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	glProgramUniform1i(program, sampler_id, 0);	//set GLSL sampler to sample from texure unit 0 - default

	glUniform4f( colorUniform, 1.0, 1.0, 1.0, 1.0); //use white color

	glBindBuffer(GL_UNIFORM_BUFFER, UBO);

	sampler->bindToUnit(0);


		glutil::MatrixStack camMatrix;

		float translateX = (control->windowWidth / 2) - (texture.width / 2) * scale;
		float translateY = (control->windowHeight / 2) - (texture.height / 2) * scale;

		
		camMatrix.SetIdentity();
		camMatrix.Translate(translateX, translateY, 0.0f);
		camMatrix.Scale(scale);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camMatrix.Top())); 
			
		glDrawArrays(GL_TRIANGLES, 0, data.size() / 4 ); // 4 items per vertex


	sampler->unbindFromUnit(0);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	glBindVertexArray(0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);
}