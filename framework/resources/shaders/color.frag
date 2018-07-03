#version 420

	in vec4 pass_color;
	in vec3 pass_normal;
	in vec3 pass_light_dir;

layout (location = 0) out vec4 fragment_color;

vec3 diffuseLighting(in vec3 N, in vec3 L){

	float diffuseTerm = max(dot(N,L),0);
	return pass_color.xyz  * diffuseTerm;

}

vec3 ambientLighting(){

	return pass_color.xyz * 0.8;
}

void main() {
  fragment_color = vec4(0.0, 0.0, 0.0, 1.0);
}