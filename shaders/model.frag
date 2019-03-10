#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 4) uniform vec4 color;
layout(binding = 13) uniform sampler2D shadowSampler[4];

out vec4 out_color;

in DATA
{
	vec3 normal;
	vec3 view_pos;
	vec3 frag_pos;
	vec4 lightSpacePos[4];
	float clipPositions[4];
	float clipSpace;
} frag_in;
const float shininess = 32.0f;
const vec3 glight_ambient = vec3(0.4,0.4,0.4);
const vec3 glight_specular = vec3(0.6,0.6,0.6);
const vec3 glight_diffuse = vec3(0.8,0.8,0.8);
const vec3 glight_direction = vec3(-2.f,-4.f,0);

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

float calculate_shadow_bias(vec3 normal,vec3 lightDir)
{
	return max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
}

float shadow_calculation(vec4 fragPosLightSpace,sampler2D _shadowMap,float bias)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(_shadowMap,projCoords.xy).r;   
	float currentDepth = projCoords.z; 
	//float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(_shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

	if(projCoords.z > 1.0)
		shadow = 0.0;
	return shadow;
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

	float shadow = 0;
	float bias = calculate_shadow_bias(normalize( frag_in.normal),normalize(glight_direction));
	vec4 coloradd = vec4(0);
	do{
		if (frag_in.clipSpace <= frag_in.clipPositions[0]) {
			shadow = shadow_calculation(frag_in.lightSpacePos[0],shadowSampler[0],bias);
			coloradd = vec4(0.1, 0.0, 0.0, 0.0);
			break;}
		if (frag_in.clipSpace <= frag_in.clipPositions[1]) {
			shadow = shadow_calculation(frag_in.lightSpacePos[1],shadowSampler[1],bias);
			coloradd = vec4(0.0, 0.1, 0.0, 0.0);
			break;}
		if (frag_in.clipSpace <= frag_in.clipPositions[2]) {
			shadow = shadow_calculation(frag_in.lightSpacePos[2],shadowSampler[2],bias);
			coloradd = vec4(0.0, 0.0, 0.1, 0.0);
			break;}
		if (frag_in.clipSpace <= frag_in.clipPositions[3]) {
			shadow = shadow_calculation(frag_in.lightSpacePos[3],shadowSampler[3],bias);
			break;}
	}while(false);


	out_color = vec4(colors[0] + colors[1]*(1 -shadow) + colors[2]*(1 -shadow),1.f);//* 0.1 + coloradd;
}

