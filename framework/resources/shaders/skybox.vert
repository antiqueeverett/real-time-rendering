#version 450

uniform mat4 view;
uniform mat4 projection;

layout(location = 0) in vec4 vPos;

out vec4 fTexCoord;
void main()
{
	fTexCoord = 1.0f / vPos * 512.0f;
	gl_Position = (projection * view) * vPos;

}