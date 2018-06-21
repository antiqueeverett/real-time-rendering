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
  vec3 N = normalize(pass_normal);
  vec3 L = normalize(pass_light_dir);
  fragment_color = vec4(diffuseLighting(N, L) + ambientLighting(), 1.0);
}