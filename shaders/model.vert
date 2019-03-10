#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

layout(location = 1) uniform mat4 projection;
layout(location = 2) uniform mat4 view;
layout(location = 3) uniform mat4 model;
layout(location = 8) uniform mat4 shadowcascades[4];
layout(location = 12) uniform vec4 splits;

out DATA
{
	vec3 normal;
	vec3 view_pos;
	vec3 frag_pos;
	vec4 lightSpacePos[4];
	float clipPositions[4];
	float clipSpace;
} vert_out;

void main() 
{
	vert_out.normal = normalize(mat3(transpose(inverse(model))) * inNormal);
	mat4 inv = inverse(view);
	vert_out.view_pos = vec3(inv[3][0], inv[3][1], inv[3][2]);
	vert_out.frag_pos = vec3(model * vec4(inPos, 1.0));
	gl_Position = projection * view * model * vec4(inPos,1);

	// shadows
	vert_out.lightSpacePos[0] = shadowcascades[0] *vec4(vert_out.frag_pos,1.0); 
	vert_out.lightSpacePos[1] = shadowcascades[1] *vec4(vert_out.frag_pos,1.0);
	vert_out.lightSpacePos[2] = shadowcascades[2] *vec4(vert_out.frag_pos,1.0); 
	vert_out.lightSpacePos[3] = shadowcascades[3] *vec4(vert_out.frag_pos,1.0);

	vert_out.clipPositions[0] = -splits.x;
	vert_out.clipPositions[1] = -splits.y;
	vert_out.clipPositions[2] = -splits.z;
	vert_out.clipPositions[3] = -splits.w;
	
	vert_out.clipSpace = gl_Position.z;
}
