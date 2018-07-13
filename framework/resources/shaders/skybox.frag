#version 450

in vec4 fTexCoord;

uniform samplerCube skyboxDaySampler;
uniform samplerCube skyboxNightSampler;
uniform float time;
uniform mat4 rotateView;
out vec4 fragmentColor;

const float lowerLimit = 0.0f;
const float upperLimit = 0.25f;


void main()
{
	vec4 rotateTexCoord = rotateView * fTexCoord;
	float blendFactor;
	int timeInt = int(time);

	vec4 fogColorDay = vec4(0.5444,0.62,0.69, 1.0);
	vec4 fogColorNight = vec4(0.0);
	vec4 skyDayColor = texture(skyboxDaySampler, vec3(rotateTexCoord)).rgba;
	vec4 skyNightColor = texture(skyboxNightSampler, vec3(rotateTexCoord)).rgba;
	vec4 skyColor = vec4(0.0);
	vec4 fogColor = vec4(0.0);

	if(timeInt >= 0 && timeInt < 5000){
		blendFactor = (float(timeInt) - 0.0)/(5000.0 - 0.0);
		skyColor = mix(skyNightColor, skyNightColor, blendFactor);
		fogColor = mix(fogColorNight, fogColorNight, blendFactor);
	}
	else if (timeInt >= 5000 && timeInt < 8000){
		blendFactor = (float(timeInt) - 5000.0)/(8000.0 - 5000.0);
		skyColor = mix(skyNightColor, skyDayColor, blendFactor);
		fogColor = mix(fogColorNight, fogColorDay, blendFactor);
	}
	else if (time >= 8000 && timeInt < 21000){
		blendFactor = (float(timeInt) - 8000.0)/(21000.0 - 8000.0);
		skyColor = mix(skyDayColor, skyDayColor, blendFactor);
		fogColor = mix(fogColorDay, fogColorDay, blendFactor);
	}
	else{
		blendFactor = (float(timeInt) - 21000.0)/(24000.0 - 21000.0);
		skyColor = mix(skyDayColor, skyNightColor, blendFactor); 
		fogColor = mix(fogColorDay, fogColorNight, blendFactor);
	}

	
	float fogFactor = (rotateTexCoord.y - lowerLimit) / (upperLimit - lowerLimit); 
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	
	fragmentColor = mix(fogColor, skyColor, fogFactor); 
	
	//fragmentColor = skyColor;
	
}