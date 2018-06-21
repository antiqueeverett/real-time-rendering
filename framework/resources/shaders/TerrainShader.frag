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
	vec4 fogColor = vec4(0.235294,0.266667,0.333333, 1.0);
	vec3 N = normalize(fWorldNormal);
	vec3 V = normalize(fWorldCam - fWorldPos);
	vec3 L_Sun = normalize(worldSunDirection);
	vec3 H_Sun = normalize(V + L_Sun);

	
	float Strength_Sun = 0.9;	
	float shiny = 128.0;	
	
	fragmentColor = vec4(0.0);
	vec4 diffuseColor;
	vec4 specularColor;

	// Texture Mapping based on height
	float t = 2.0 * (height/amplitude) - 1.0;	// t element [-1.0, 1.0]
	t = clamp(t, -1.0, 1.0);

	vec4 translTexCoord = fTexCoord + vec4(float(fTimeTranslate.x/(511)*12), float(fTimeTranslate.z/(511)*12), 0.0 ,0.0 );
	
	if (t < 0.0) 
	{
		diffuseColor = mix(texture(terrain0, translTexCoord.st).rgba, texture(terrain1, translTexCoord.st).rgba,-t);
		specularColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else
	{
		diffuseColor = mix(texture(terrain0, translTexCoord.st).rgba, texture(terrain2, translTexCoord.st).rgba,t);
		specularColor = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(0.75, 0.75, 0.75, 1.0), t);		
	}
	
	
	// Fog
	float dist = distance(vec3(0.0), fViewPos);
	//float dist = length(fViewPos.xyz);
	float fogFactor = exp(-pow(dist*0.0035, 4.0));
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	diffuseColor = mix(fogColor, diffuseColor, fogFactor);
	diffuseColor.w = fogFactor;
	// Light
	fragmentColor += diffuseColor * Strength_Sun * max(0.0, dot(N, L_Sun));	
	fragmentColor += specularColor * Strength_Sun * pow(max(0.0, dot(N, H_Sun)), shiny);
	
	//fragmentColor = vec4(1.0);
	

}
