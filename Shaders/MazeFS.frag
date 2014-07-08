#version 330

flat in vec4 theColor;
in vec2 coordinates;

out vec4 outputColor;

uniform sampler2D mazeSampler;


void main()
{
	vec4 textureColor = texture(mazeSampler, coordinates);
	
	outputColor = vec4(textureColor.rgb * textureColor.a  +  ((1.0f - textureColor.a) * theColor.rgb), 1.0f);

}

