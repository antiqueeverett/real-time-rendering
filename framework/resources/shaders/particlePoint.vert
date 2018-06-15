#version 420
layout (location = 0) in vec4 particle_position;
layout (location = 1) in vec4 particle_color;

uniform mat4 camera_matrix;
uniform mat4 projection_matrix;

out vec4 pass_color;

void main() {

  gl_Position = (projection_matrix * (camera_matrix)) * vec4(particle_position.xyz, 1.0);
  pass_color = vec4(particle_color.xyz, 1.0);

}