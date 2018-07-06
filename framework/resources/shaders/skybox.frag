#version 450

in vec4 fTexCoord;

uniform samplerCube skyboxSampler;

out vec4 fragmentColor;

const float lowerLimit = -15.0f;
const float upperLimit = 30.0f;

void main()
{
 	//vec4 fogColor = vec4(0.235294,0.266667,0.333333, 1.0);
	vec4 fogColor = vec4(0.51372549,0.56862745,0.62745098, 1.0);
	vec4 skyColor = texture(skyboxSampler, vec3(fTexCoord)).rgba;
	float fogFactor = (fTexCoord.y - lowerLimit) / (upperLimit - lowerLimit); 
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	
	fragmentColor = mix(fogColor, skyColor, fogFactor); 
	
	//fragmentColor = texture(skyboxSampler, vec3(fTexCoord)).rgba;
	
}