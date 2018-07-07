#version 420

in float height;

in vec4 fTexCoord;

in vec3 fWorldPos;
in vec3 fWorldNormal;
in vec3 fWorldCam;

in vec3 fViewPos;

in vec3 fTimeTranslate;

uniform vec3 worldSunDirection;
uniform sampler2D terrain0;
uniform sampler2D terrain1;
uniform sampler2D terrain2;
uniform float amplitude;

uniform int terrainResolution;
uniform int textureTileNumber;

out vec3 fragmentColor;

void main()
{
	fragmentColor = vec3(0.0);
	vec3 diffuseColor = vec3(0.0);
	vec3 specularColor = vec3(0.0);
	vec3 fogColor = vec3(0.51372549, 0.56862745, 0.62745098);

	vec3 N = normalize(fWorldNormal);
	vec3 V = normalize(fWorldCam - fWorldPos);
	vec3 L_Sun = normalize(worldSunDirection);
	vec3 H_Sun = normalize(V + L_Sun);
	float Strength_Sun = 0.9;	

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
	float dist = distance(vec3(0.0), fViewPos);
	float fogFactor = exp(-pow(dist*0.007, 4.0));
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	diffuseColor = mix(fogColor, diffuseColor, fogFactor);
	
	// Light
	fragmentColor += diffuseColor * Strength_Sun * max(0.0, dot(N, L_Sun));	

}