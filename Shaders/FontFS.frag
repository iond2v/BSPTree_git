#version 330

in vec2 textureCoordinates; // Interpolated values from the vertex shaders

uniform sampler2D fontSampler;
uniform vec4 colorUniform;

out vec4 outputColor;


void main()
{
	vec4 textureColor = texture(fontSampler, textureCoordinates);   //texture is font in greyscale
/*
	if(textureColor.r == 0){
	outputColor = vec4(0.1, 0.6, 0.0, 0.3);
	}else{*/
	outputColor = vec4(colorUniform.r, colorUniform.g, colorUniform.b, textureColor.r);
		//}
}