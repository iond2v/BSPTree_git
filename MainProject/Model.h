#pragma once

#include <glm/glm.hpp>
#include <glload/gl_3_3.h>
#include <string>
#include <iostream>
#include <vector>

#define not !

class Model
{
public:
	Model(std::string filename);
	~Model(void);

	GLuint VAO;
	GLuint VBO;
	GLuint IBO;
	GLuint textureVBO;

	int numOfVertices;

	std::string filename;
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> faces;




	void initVAO();
	bool loadModel();
	void draw();
	bool isReady();

};

