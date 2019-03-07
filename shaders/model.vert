#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

layout(location = 1) uniform mat4 projection;
layout(location = 2) uniform mat4 view;
layout(location = 3) uniform mat4 model;

out DATA
{
	vec3 normal;
	vec3 view_pos;
	vec3 frag_pos;
} vert_out;

void main() 
{
	vert_out.normal = inNormal;
	mat4 inv = inverse(view);
	vert_out.view_pos = vec3(inv[3][0], inv[3][1], inv[3][2]);
	vert_out.frag_pos = vec3(model * vec4(inPos, 1.0));
	gl_Position = projection * view * model * vec4(inPos,1);
}
