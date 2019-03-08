#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(binding = 5) uniform samplerCube tex;

in DATA
{
	vec3 uv;
} frag_in;

out vec4 FragColor;

void main() 
{
	 FragColor = texture(tex, frag_in.uv);
}
