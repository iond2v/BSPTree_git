#include "Model.h"
#include <fstream>
#include <sstream> 

Model::Model(std::string filename)
{
	this->filename = filename;

	VAO = 0;
	VBO = 0;
	IBO = 0;
	textureVBO = 0;
}

bool Model::isReady(){


return (VAO != 0 && VBO != 0 && IBO != 0 && textureVBO != 0);
}
Model::~Model(void)
{
	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &textureVBO);
	
}


/*
Loads .obj file into vertices.
*/
bool Model::loadModel(){
	using namespace std;

	cout << "\rLoading model: " << filename;

	//open file
	std::ifstream file(filename);


	if(not file.is_open())
		return false;

	string line;

	while (getline(file, line))
    {
		std::istringstream iLineStream(line);
        string mark;
		
		iLineStream >> mark;
		
		//line with vertex
		if(mark.compare("v") == 0){
		
			glm::vec3 point;

			iLineStream >> point.x;
			iLineStream >> point.y;
			iLineStream >> point.z;

			vertices.push_back(point);

		//line with face
		} else if (mark.compare("f") == 0){
		
		GLuint tmp;

			iLineStream >> tmp;
			faces.push_back(tmp - 1);

			iLineStream >> tmp;
			faces.push_back(tmp - 1);

			iLineStream >> tmp;
			faces.push_back(tmp - 1);


		
		}
		//dont care about others now
    }

	cout << "\r" << filename << " loaded. " << faces.size() / 3 << " triangles.\n";

	return true;
}


void Model::initVAO(){

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


		glBufferData(GL_ARRAY_BUFFER, (vertices.size()) * 3 * sizeof(float), vertices.data(), GL_STATIC_DRAW); //again size in Bytes!!!
	

		glEnableVertexAttribArray(0);
			
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);//asociates vertex data in currently bound VBO to VAO; not part of vao state
				

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(float), faces.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	////////generate texture mapping..
	std::vector<glm::vec2> textureCoordinates;
	
	for(unsigned int i = 0; i < vertices.size(); i++){
		textureCoordinates.push_back(glm::vec2(0.9f, 0.9f)); //somewhere transparent
	
	}


	glGenBuffers(1, &textureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);


		glBufferData(GL_ARRAY_BUFFER, (textureCoordinates.size()) * 2 * sizeof(float), textureCoordinates.data(), GL_STATIC_DRAW); //again size in Bytes!!!
	

		glEnableVertexAttribArray(1);
			
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);



	//dont need it anymore.. store size and discard
	numOfVertices = faces.size();
	vertices.clear();
	faces.clear();


}

void Model::draw(){

	if(not isReady())
		return;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	/*
	The first parameter is the primitive type to render (same as glDrawArrays).
	The second parameter is the number of indices in the index buffer to use for primitive generation. 
	The next parameter is the type of each index. The GPU must be told the size of each individual index else it will not know how to parse the buffer. 
		Possible values here are GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT. If the index range is small you want the smaller types
		that are more space efficient and if the index range is large you want the larger types. 

	The final parameter tells the GPU the offset in bytes from the start of the index buffer to the location of the first index to scan.
	*/

		glDrawElements(GL_TRIANGLES, numOfVertices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}