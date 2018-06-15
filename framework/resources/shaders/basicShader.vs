// 1.	specify the version of openGL
// 2. 	specify data and data type to accept as a parameter  

#version 120

attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;

varying vec2 texCoord0;
varying vec3 normal0;

uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(position, 1.0);
	texCoord0 = texCoord;
	normal0 = (transform * vec4(normal, 0.0)).xyz;
}


// this is the same as the identity program in a manner of speaking
//     (i.e.) takes the position and returns the position as is 