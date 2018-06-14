#version 420

in vec4 fTexCoord;

uniform samplerCube skyboxSampler;

out vec4 fragmentColor;

void main()
{
	fragmentColor = texture(skyboxSampler, vec3(fTexCoord)).rgba;
	
}