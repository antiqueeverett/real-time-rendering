#version 420

in float height;

in vec4 fTexCoord;

in vec3 fWorldPos;
in vec3 fWorldNormal;
in vec3 fWorldCam;

in vec3 fViewPos;

in vec3 fTimeTranslate;

uniform vec4 worldSunDirection;
uniform mat4 sunRotation;
uniform sampler2D terrain0;
uniform sampler2D terrain1;
uniform sampler2D terrain2;
uniform float amplitude;
uniform float time;

uniform int terrainResolution;
uniform int textureTileNumber;

out vec3 fragmentColor;

void main()
{
	fragmentColor = vec3(0.0);
	vec3 diffuseColor = vec3(0.0);
	vec3 specularColor = vec3(0.0);
	vec3 fogColorDay = vec3(0.5444,0.62,0.69);
	vec3 fogColor = vec3(0.0);
	vec3 fogColorNight = vec3(0.0);
	vec3 N = normalize(fWorldNormal);
	vec3 V = normalize(fWorldCam - fWorldPos);
	vec3 L_Sun = normalize((sunRotation*worldSunDirection).xyz);
	vec4 moonDirection = worldSunDirection;
	moonDirection.xz = - moonDirection.xz;
	vec3 L_Moon = normalize((sunRotation*moonDirection).xyz);
	vec3 H_Sun = normalize(V + L_Sun);
	float Strength_Sun = 0.9;
	float diffuseSunLight;
	float diffuseMoonLight;
	float Strength_Moon = 0.8;	

	// Translate Tex coord based on terrain movement and map texture on pixel
	vec2 translTexCoord = fTexCoord.st + vec2(float(fTimeTranslate.x/(terrainResolution-1)*textureTileNumber), float(fTimeTranslate.z/(terrainResolution-1)*textureTileNumber));
	vec3 mudColor = texture(terrain0, translTexCoord.st).rgb;
	vec3 grassColor = texture(terrain1, translTexCoord.st).rgb;
	vec3 stoneColor = texture(terrain2, translTexCoord.st).rgb;
	
	// Texture Mapping based on height
	float clampHeight = (height / amplitude) ;	// t element [-1.0, 1.0]
	float t = clamp(clampHeight, -1.0, 1.0);
	
	if (t < 0.0) 
	{
		diffuseColor = mix(grassColor, mudColor,-t);
	}
	else
	{
		diffuseColor = mix(grassColor, stoneColor,t);
	} 

	// Fog
	float dist = length(fViewPos);
	float fogFactor = exp(-pow(dist*0.004, 3.0));
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	float blendFactor = 0.0f;
	int timeInt = int(time);

	if(timeInt >= 0 && timeInt < 5000){
		blendFactor = (float(timeInt) - 0.0)/(5000.0 - 0.0);
		fogColor = mix(fogColorNight, fogColorNight, blendFactor);
		diffuseSunLight = max(0.0, dot(N, L_Moon));
		Strength_Sun = 0.0;
		Strength_Moon = 0.5;

	}
	else if (timeInt >= 5000 && timeInt < 8000){
		blendFactor = (float(timeInt) - 5000.0)/(8000.0 - 5000.0);
		fogColor = mix(fogColorNight, fogColorDay, blendFactor);
		Strength_Sun = mix(0.0, 0.9, blendFactor);
		Strength_Moon = mix(0.5, 0.0, blendFactor);

	}
	else if (time >= 8000 && timeInt < 21000){
		blendFactor = (float(timeInt) - 8000.0)/(21000.0 - 8000.0);
		fogColor = mix(fogColorDay, fogColorDay, blendFactor);
		Strength_Sun = 0.9;
		Strength_Moon = 0.0;
	}
	else{
		blendFactor = (float(timeInt) - 21000.0)/(24000.0 - 21000.0);
		fogColor = mix(fogColorDay, fogColorNight, blendFactor);
		Strength_Sun = mix(0.9, 0.0, blendFactor);
		Strength_Moon = mix(0.0, 0.5, blendFactor);
	}

	diffuseColor = mix(fogColor, diffuseColor, fogFactor);
	
	// Light
	fragmentColor += diffuseColor * Strength_Sun * max(0.0, dot(N, L_Sun));	
	fragmentColor += diffuseColor * Strength_Moon * max(0.0, dot(N, L_Moon));

}