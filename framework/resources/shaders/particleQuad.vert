#version 420
layout (location = 0) in vec4 particle_position;
layout (location = 1) in vec4 particle_color;
layout (location = 2) in vec4 particle_life;

uniform mat4 camera_matrix;

out VertexAttrib
{
  vec4 color;
  vec4 life;
} vertex;

//out vec4 pass_color;


void main() {
  gl_Position = (camera_matrix) * vec4(particle_position.xyz, 1.0);
  vertex.color = vec4(particle_color.xyz, 0.0);
  vertex.life = particle_life;

}