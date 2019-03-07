#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 4) uniform vec4 color;

out vec4 out_color;

in DATA
{
	vec3 normal;
	vec3 view_pos;
	vec3 frag_pos;
} frag_in;
const float shininess = 32.0f;
const vec3 glight_ambient = vec3(0.4,0.4,0.4);
const vec3 glight_specular = vec3(0.6,0.6,0.6);
const vec3 glight_diffuse = vec3(0.8,0.8,0.8);
const vec3 glight_direction = vec3(0.2,-0.6,0);

void calculate_global_light(
		inout vec3 colors[3],
		vec3 diffuse,
		vec3 specular,
		vec3 normal, 
		vec3 viewDir)
{
	vec3 lightDir = normalize(-glight_direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	//vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	// combine results
	colors[0] = glight_ambient  * diffuse;
	colors[1] = glight_diffuse  * diff * diffuse;
	colors[2] = glight_specular * spec * specular;
}

void main() 
{
	//TODO(pate) light calculations
	vec3 colors[3];
	vec3 viewPos = frag_in.view_pos;
	vec3 viewDir = normalize(viewPos - frag_in.frag_pos);
	vec3 norm = normalize(frag_in.normal);
	calculate_global_light(colors,
							color.xyz,
							vec3(0.7,0.5,0.5),
							norm,
							viewDir);
	out_color = vec4(colors[0] + colors[1] + colors[2],1.f);
}

