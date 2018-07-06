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

out vec4 fragmentColor;

void main()
{

	vec4 fogColor = vec4(0.51372549, 0.56862745, 0.62745098, 1.0);
	vec3 N = normalize(fWorldNormal);
	vec3 V = normalize(fWorldCam - fWorldPos);
	vec3 L_Sun = normalize(worldSunDirection);
	vec3 H_Sun = normalize(V + L_Sun);

	float Strength_Sun = 0.9;	
	float shiny = 128.0;	
	
	fragmentColor = vec4(0.0);
	vec4 diffuseColor = vec4(0.0);
	vec4 specularColor = vec4(0.0);

	vec4 translTexCoord = fTexCoord + vec4(float(fTimeTranslate.x/(terrainResolution-1)*textureTileNumber), float(fTimeTranslate.z/(terrainResolution-1)*textureTileNumber), 0.0 ,0.0 );
	vec3 mudColor = texture(terrain0, translTexCoord.st).rgb;
	vec3 grassColor = texture(terrain1, translTexCoord.st).rgb;
	vec3 stoneColor = texture(terrain2, translTexCoord.st).rgb;
	
	// Texture Mapping based on height
	float clampHeight = (height / amplitude) ;	// t element [-1.0, 1.0]
	float t = clamp(clampHeight, -1.0, 1.0);
	
	if (t < 0.0) 
	{
		diffuseColor = vec4(mix(grassColor, mudColor,-t), 1.0);
		specularColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else
	{
		diffuseColor = vec4(mix(grassColor, stoneColor,t) , 1.0);
		specularColor = vec4(0.0, 0.0, 0.0, 1.0);		
	} 

	// Fog
	float dist = distance(vec3(0.0), fViewPos);
	float fogFactor = exp(-pow(dist*0.004, 4.0));
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	diffuseColor = mix(fogColor, diffuseColor, fogFactor);
	
	// Light
	//fragmentColor += diffuseColor * Strength_Sun * max(0.0, dot(N, L_Sun));	
	//fragmentColor += specularColor * Strength_Sun * pow(max(0.0, dot(N, H_Sun)), shiny);
	fragmentColor += diffuseColor * Strength_Sun;	
	fragmentColor += specularColor * Strength_Sun;
	

}