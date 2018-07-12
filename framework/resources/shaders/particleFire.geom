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
out vec4 vertex_color;

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
  float opacity = (1.0 - vertex[0].life.z) * 0.9;
  vec4 P = gl_in[0].gl_Position;
  float particle_size = 1.0f;

  // a: left-bottom 
  vec2 va = P.xy + vec2(-0.5, -0.5) * particle_size;
  gl_Position = projection_matrix * vec4(va, P.zw);
  vertex_uv = calc_uv(vec2(0.0, 1.0));
  vertex_color = vec4(vertex[0].color.xyz, opacity);
  EmitVertex();  
  
  // b: left-top
  vec2 vb = P.xy + vec2(-0.5, 0.5) * particle_size;
  gl_Position = projection_matrix * vec4(vb, P.zw);
  vertex_uv = calc_uv(vec2(0.0, 0.0));
  vertex_color = vec4(vertex[0].color.xyz, opacity);
  EmitVertex();  
  
  // d: right-bottom
  vec2 vd = P.xy + vec2(0.5, -0.5) * particle_size;
  gl_Position = projection_matrix * vec4(vd, P.zw);
  vertex_uv = calc_uv(vec2(1.0, 1.0));
  vertex_color = vec4(vertex[0].color.xyz, opacity);
  EmitVertex();  

  // c: right-top
  vec2 vc = P.xy + vec2(0.5, 0.5) * particle_size;
  gl_Position = projection_matrix * vec4(vc, P.zw);
  vertex_uv = calc_uv(vec2(1.0, 0.0));
  vertex_color = vec4(vertex[0].color.xyz, opacity);
  EmitVertex();  

  EndPrimitive();  
}  