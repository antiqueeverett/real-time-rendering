#version 420

in float height;

in vec4 fTexCoord;

in vec3 fWorldPos;
in vec3 fWorldNormal;
in vec3 fWorldCam;

uniform vec3 worldSunDirection;
uniform sampler2D terrain0;
uniform sampler2D terrain1;
uniform sampler2D terrain2;
uniform float amplitude;

out vec4 fragmentColor;

vec3 fogColor = vec3(0.5,0.6,0.7);


void main()
{
	
	vec3 N = normalize(fWorldNormal);
	vec3 V = normalize(fWorldCam - fWorldPos);
	vec3 L_Sun = normalize(worldSunDirection);
	vec3 H_Sun = normalize(V + L_Sun);

	float distToPoint = normalize(distance(fWorldCam, fWorldPos));
	
	float Strength_Sun = 0.6;	
	float shiny = 128.0;	
	
	fragmentColor = vec4(0.0);
	vec4 diffuseColor;
	vec4 specularColor;


	float t = 2.0 * (height/amplitude) - 1.0;	// t element [-1.0, 1.0]
	t = clamp(t, -1.0, 1.0);

	if (t < 0.0) 
	{
		diffuseColor = mix(texture(terrain0, fTexCoord.st).rgba, texture(terrain1, fTexCoord.st).rgba,-t);
		specularColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else
	{
		diffuseColor = mix(texture(terrain0, fTexCoord.st).rgba, texture(terrain2, fTexCoord.st).rgba,t);
		specularColor = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(0.75, 0.75, 0.75, 1.0), t);		
	}
	float fogFactor = 1.0 / exp(distToPoint*0.05); 
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	vec3 fogColor = mix(fogColor, diffuseColor.rgb, fogFactor);
	diffuseColor = vec4(fogColor, 1.0);

	fragmentColor += diffuseColor * Strength_Sun * max(0.0, dot(N, L_Sun));	
	fragmentColor += specularColor * Strength_Sun * pow(max(0.0, dot(N, H_Sun)), shiny);
}
