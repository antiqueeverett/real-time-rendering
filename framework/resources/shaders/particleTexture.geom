#version 420

layout(points) in;
layout(triangle_strip, max_vertices=4) out;


uniform mat4 projection_matrix;
uniform int atlas_dim;

in VertexAttrib
{
  vec4 color;
  vec4 life;
} vertex [];

out vec2 vertex_uv;

vec2 calc_uv(vec2 uv){
  float life = vertex[0].life.z;
  float steps = 1.0 / (atlas_dim * atlas_dim);
  float grid = 1.0 / atlas_dim;
  int stage = int(life / steps);
  int col = int(mod(stage, atlas_dim));
  int row = (stage - col) / atlas_dim;

  return (uv + vec2(col, row)) * grid;
}

void main()
{	
  vec4 P = gl_in[0].gl_Position;
  float particle_size = 0.8f;

  // a: left-bottom 
  vec2 va = P.xy + vec2(-0.5, -0.5) * particle_size;
  gl_Position = projection_matrix * vec4(va, P.zw);
  vertex_uv = calc_uv(vec2(0.0, 1.0));
  EmitVertex();  
  
  // b: left-top
  vec2 vb = P.xy + vec2(-0.5, 0.5) * particle_size;
  gl_Position = projection_matrix * vec4(vb, P.zw);
  vertex_uv = calc_uv(vec2(0.0, 0.0));
  EmitVertex();  
  
  // d: right-bottom
  vec2 vd = P.xy + vec2(0.5, -0.5) * particle_size;
  gl_Position = projection_matrix * vec4(vd, P.zw);
  vertex_uv = calc_uv(vec2(1.0, 1.0));
  EmitVertex();  

  // c: right-top
  vec2 vc = P.xy + vec2(0.5, 0.5) * particle_size;
  gl_Position = projection_matrix * vec4(vc, P.zw);
  vertex_uv = calc_uv(vec2(1.0, 0.0));
  EmitVertex();  

  EndPrimitive();  
}  