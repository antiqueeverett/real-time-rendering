#version 420

in float height;

in vec4 fTexCoord;

in vec3 fWorldPos;
in vec3 fWorldNormal;
in vec3 fWorldCam;

in vec3 fViewPos;


uniform vec3 worldSunDirection;
uniform sampler2D terrain0;
uniform sampler2D terrain1;
uniform sampler2D terrain2;
uniform float amplitude;

out vec4 fragmentColor;

float hash(float n)
{
	return fract(sin(n) * 10000);
}

float hash(vec2 p) {
return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x))));
}

float hash1( vec2 p )
{
    p  = 50.0*fract( p*0.3183099 );
    return fract( p.x*p.y*(p.x+p.y) );
}

float hash1( float n )
{
    return fract( n*17.0*fract( n*0.3183099 ) );
}

float noise(vec2 x) {
    vec2 i = floor(x);
    vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    // Simple 2D lerp using smoothstep envelope between the values.
	// return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
	//			mix(c, d, smoothstep(0.0, 1.0, f.x)),
	//			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
    vec2 u = f*f*f*(f*(f*6.0 - 15.0) + 10.0);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

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
