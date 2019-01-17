#version 420

#define PICK_COLOR 1
#define TEXTURE 2
#define SPRING 4
#define NORMAL 8
#define FIXED 16

layout (location = 0) in vec4 particle_position;
layout (location = 1) in vec4 particle_color;
layout (location = 3) in vec4 particle_normal;
layout (location = 4) in vec4 particle_uv;

uniform mat4 camera_matrix;
uniform mat4 projection_matrix;
uniform int color_mode;
uniform vec3 pick_color;

out vec4 pass_color;

void main() {

  gl_Position = (projection_matrix * (camera_matrix)) * vec4(particle_position.xyz, 1.0);

  if (bool(color_mode & TEXTURE)) {
    pass_color = particle_uv;
  } else if (bool(color_mode & SPRING)) {
    pass_color = vec4(particle_color.xyz, 1.0);
  } else if (bool(color_mode & NORMAL)) {
    pass_color = vec4(particle_normal.xyz,1.0);
  } else if (bool(color_mode & FIXED)) {
        if(particle_position.w == 1){
            pass_color = vec4(0.0, 0.0, 0.0, 1.0);
        } else {
            pass_color = vec4(1.0, 1.0, 1.0, 1.0);
        }
  } else {
    // PICK_COLOR is default
    pass_color = vec4(pick_color.xyz, 1.0);
  }




}
