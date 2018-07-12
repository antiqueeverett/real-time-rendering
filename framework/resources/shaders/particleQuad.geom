#version 420

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

uniform mat4 projection_matrix;

in VertexAttrib
{
  vec4 color;
  vec4 life;
} vertex [];

out vec4 pass_color;

void main()
{	
  vec4 P = gl_in[0].gl_Position;
  float particle_size = 0.3;

  // a: left-bottom 
  vec2 va = P.xy + vec2(-0.5, -0.5) * particle_size;
  gl_Position = projection_matrix * vec4(va, P.zw);
  pass_color = vertex[0].color;
  EmitVertex();  
  
  // b: left-top
  vec2 vb = P.xy + vec2(-0.5, 0.5) * particle_size;
  gl_Position = projection_matrix * vec4(vb, P.zw);
  pass_color = vertex[0].color;
  EmitVertex();  
  
  // d: right-bottom
  vec2 vd = P.xy + vec2(0.5, -0.5) * particle_size;
  gl_Position = projection_matrix * vec4(vd, P.zw);
  pass_color = vertex[0].color;
  EmitVertex();  

  // c: right-top
  vec2 vc = P.xy + vec2(0.5, 0.5) * particle_size;
  gl_Position = projection_matrix * vec4(vc, P.zw);
  pass_color = vertex[0].color;
  EmitVertex();  

  EndPrimitive();  
}  