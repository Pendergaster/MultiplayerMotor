#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPos;

layout(location = 1) uniform mat4 projection;
layout(location = 2) uniform mat4 view;

out DATA
{
	vec3 uv;
} vert_out;

void main() 
{
	vert_out.uv = inPos;
	gl_Position = projection * view * vec4(inPos,1);
	gl_Position = gl_Position.xyww;
}
