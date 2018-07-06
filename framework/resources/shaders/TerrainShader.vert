#version 420

uniform mat4 model;
uniform mat3 modelInvT;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 timeTranslate;

uniform float amplitude;
uniform float frequency;

layout(location = 0) in vec4 vPos;
layout(location = 2) in vec4 vNormal;
layout(location = 3) in vec4 vTexCoord;

out float height;

out vec3 fWorldPos;
out vec3 fWorldNormal;
out vec3 fWorldCam;

out vec3 fViewPos; // vertex position in view space

out vec3 fTimeTranslate;

out vec4 fTexCoord;
//------------------------------------------------------------------------------------------------------------------
// 2D PERLIN NOISE 
// Code from: 
// Morgan McGuire @morgan3d, http://graphicscodex.com, https://www.shadertoy.com/view/4dS3Wd
// and iq, https://www.shadertoy.com/view/4ttSWf
//------------------------------------------------------------------------------------------------------------------

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
    //          mix(c, d, smoothstep(0.0, 1.0, f.x)),
    //          smoothstep(0.0, 1.0, f.y)));

    // Same code, with the clamps in smoothstep and common subexpressions
    // optimized away.
    vec2 u = f*f*f*(f*(f*6.0 - 15.0) + 10.0);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float noise2d( in vec2 x )
{
    vec2 p = floor(x);
    vec2 w = fract(x);
    vec2 u = w*w*w*(w*(w*6.0-15.0)+10.0);
    
#if 0
    p *= 0.3183099;
    float kx0 = 50.0*fract( p.x );
    float kx1 = 50.0*fract( p.x+0.3183099 );
    float ky0 = 50.0*fract( p.y );
    float ky1 = 50.0*fract( p.y+0.3183099 );

    float a = fract( kx0*ky0*(kx0+ky0) );
    float b = fract( kx1*ky0*(kx1+ky0) );
    float c = fract( kx0*ky1*(kx0+ky1) );
    float d = fract( kx1*ky1*(kx1+ky1) );
#else
    float a = hash1(p+vec2(0,0));
    float b = hash1(p+vec2(1,0));
    float c = hash1(p+vec2(0,1));
    float d = hash1(p+vec2(1,1));
#endif
    
    return -1.0+2.0*( a + (b-a)*u.x + (c-a)*u.y + (a - b - c + d)*u.x*u.y );
}

//------------------------------------------------------------------------------------------------------------------
// FRACTALS 
// Code from: 
// Ebert, David S. Texturing & modeling: a procedural approach. Morgan Kaufmann, 2003.
// and iq, https://www.shadertoy.com/view/4ttSWf
//------------------------------------------------------------------------------------------------------------------

const mat2 m2 = mat2(  0.80,  0.60,
                      -0.60,  0.80 );
                      
float fBm( vec2 point, double H, double lacunarity, double octaves ) {
double value = 0.0; 
double remainder = 0.0;
int i = 0;
for (i=0; i<octaves; i++) {
value += noise2d( point ) * pow(float(lacunarity),float(-H*i) );
point.x *= float(lacunarity);
point.y *= float(lacunarity);
}
remainder = octaves - int(octaves);
if ( remainder != 0.000000000000){
    value += remainder * noise2d( point ) * pow( float(lacunarity),float(-H*i)  );
}
return float(value);
}

float multifractal( vec2 point, double H, double lacunarity, int octaves, double offset ) {
double value = 1.0;
for (int i=0; i<octaves; i++) {
value *= (noise(point) + offset) * pow( float(lacunarity), float(-H*i) );
point.x *= float(lacunarity);
point.y *= float(lacunarity);
}
return float(value);
}
                   
float fbm_9( in vec2 x )
{
    float f = 1.9;
    float s = 0.55;
    float a = 0.0;
    float b = 0.5;
    for( int i=0; i<9; i++ )
    {
        float n = noise2d(x);
        a += b*n;
        b *= s;
        x = f*m2*x;
    }
    return a;
}

//------------------------------------------------------------------------------------------------------------------
// MAIN 
//------------------------------------------------------------------------------------------------------------------
double H = 1.4;
double lacunarity = 3.2;
double octaves = 10.0;
double offset = 0.7;

void main()
{

    vec4 position = vPos + vec4(timeTranslate, 0.0); // translate terrain when it moves to make terrain look infinite
    //position.y = 0.25 * noise(position.xz*frequency) + 0.125 * fBm((position.xz*frequency), H, lacunarity, octaves)+ 0.25* fbm_9(position.xz * frequency/2.0);
	//	position.y = multifractal(position.xz, H, lacunarity, int(octaves), 0.8);
	position.y = fBm(position.xz*frequency, 1.4, 3.2, octaves);
	//position.y = fbm_9(position.xz*frequency);
	position.y *= amplitude;
    
   if(position.y < -amplitude)
    {
        position.y = -amplitude;
    } 

	   if(position.y > amplitude)
    {
        position.y = amplitude;
    } 
	
	/*
	CALCULATE NORMALS
	*/
	
    position.x = vPos.x;    //to stay on the grid, reset x and values
    position.z = vPos.z;

    fTimeTranslate = timeTranslate;
    fViewPos = (view * model * position).xyz;                   //in view space
    height = position.y;                                        //height of vertex
    gl_Position = (projection * view * model) * position;       //ModelViewTransformation
    fWorldPos = (model * position).xyz;                         // in world space
    fWorldNormal = normalize(modelInvT * vNormal.xyz);          //surface normal                                                            
    fWorldCam = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz; //Camera position in world space
	fTexCoord = vTexCoord; 


}