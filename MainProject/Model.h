#pragma once

#include <glm/glm.hpp>
#include <glload/gl_3_3.h>
#include <string>
#include <iostream>
#include <vector>
#include "basicPolygon.h"
#include "Utilities.h"

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
	GLuint normalVBO;
	


	int numOfVertices;

	std::string filename;
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> faces;
	std::vector<glm::vec3> normals;




	void initVAO();
	bool loadModel();
	void draw();
	bool isReady();

};

