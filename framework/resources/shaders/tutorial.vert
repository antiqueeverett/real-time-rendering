#version 420
    layout (location = 0) in vec3 vertex_position;
    layout (location = 1) in vec2 vertex_coord;
    layout (location = 2) in vec3 vertex_normal;
    
    uniform mat4 camera_matrix;
    uniform mat4 projection_matrix;
    uniform mat4 model_matrix;
    
    
    out vec4 pass_color;
    out vec3 pass_normal;
    out vec3 pass_light_dir;
    
    void main() {
      gl_Position = (projection_matrix * camera_matrix * model_matrix) * vec4(vertex_position, 1.0);
      //gl_Position = vec4(vertex_position, 1.0);
      
      pass_color = vec4(normalize(vertex_normal), 1.0);
      pass_normal = vertex_normal;
      pass_light_dir = vec3(100.0, 200.0, -50.0) - vertex_position;
    }