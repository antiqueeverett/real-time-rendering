#version 420

uniform sampler2D sampler;
in vec2 vertex_uv;
in vec4 vertex_color;

layout (location = 0) out vec4 fragment_color;

void main() {
	vec4 looked_up_color = texture(sampler, vertex_uv);
	if (0.0 == looked_up_color.a) {
		discard;
	}

	//fragment_color = vec4(looked_up_color.rgb, looked_up_color.a * vertex_color.a);
	fragment_color = vertex_color;
}