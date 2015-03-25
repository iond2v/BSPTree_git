#version 330

 //gl_Position: the clip-space position of the vertex.

layout(location = 0) in vec4 position;  //sets location directly. to get location use query glGetAttribLocation
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec3 normal;

flat out vec4 theColor; 
out vec2 coordinates;

uniform vec4 colorUniform;


layout(std140) uniform GlobalMatrices
{
	mat4 cameraToClipMatrix;	//perspective
	mat4 worldToCameraMatrix;  //position camera
};

uniform mat4 modelToWorldMatrix;  //move model into place

void main()
{

	vec4 temp = modelToWorldMatrix * position;
	temp = worldToCameraMatrix * temp;
	gl_Position = cameraToClipMatrix * temp;
	
	coordinates = textureCoordinates;

	theColor = colorUniform;
}
