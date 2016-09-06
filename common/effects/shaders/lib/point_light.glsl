#define SHOFT_SHADOW
//structs
struct point_light
{
	vec3  m_position;
    vec3  m_direction;
    
    vec3  m_diffuse;
    vec3  m_specular;
    
    float m_constant;
    float m_inside_radius;
    float m_radius;

	bool	    m_use_shadow;
	samplerCube m_shadow_map;
};
//return
struct point_light_res
{
    vec3 m_diffuse;
    vec3 m_specular;
};

//attenuation
float point_light_compute_attenuation(in point_light light0, in vec3  frag_position)
{
	float light_distance = length(light0.m_position - frag_position);
	float attenuation 	 = pow(smoothstep(light0.m_radius, light0.m_inside_radius, light_distance), light0.m_constant);
	return attenuation;
}

float point_light_pcf_shadow(in point_light light0, in vec3 frag_to_light, in float bias)
{
	vec3 sample_offset_directions[20] = vec3[]
	(
		vec3(1, 1, 1),  vec3(1, -1, 1),  vec3(-1, -1, 1), vec3(-1, 1, 1 ),
		vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1),vec3(-1, 1, -1),
		vec3(1, 1, 0),  vec3(1, -1, 0),  vec3(-1, -1, 0), vec3(-1, 1, 0 ),
		vec3(1, 0, 1),  vec3(-1, 0, 1),  vec3(1, 0, -1 ), vec3(-1, 0, -1),
		vec3(0, 1, 1),  vec3(0, -1, 1),  vec3(0, -1, -1), vec3(0, 1, -1 )
	);
	float shadow  = 0.0;
	int   samples = 20;
	float current_depth = length(frag_to_light);
	float disk_radius   = 0.1;

	for (int i = 0; i < samples; ++i)
	{
		float closest_depth = texture(light0.m_shadow_map, frag_to_light + sample_offset_directions[i] * disk_radius).r;
		//0-1 to 0 to dist
		closest_depth *= light0.m_radius;
		//test
		if (current_depth - bias <= closest_depth) shadow += 1.0;
	}

	shadow /= float(samples);
	//return
	return shadow;
}

//compute shadow
float point_light_compute_shadow_light(in point_light light0, in vec4  frag_position, float bias)
{
	// Get vector between fragment position and light position
	vec3 frag_to_light = frag_position.xyz - light0.m_position;
	// Use the light to fragment vector to sample from the depth map    
#ifdef SHOFT_SHADOW
	float shadow = point_light_pcf_shadow(light0, frag_to_light, bias);
#else 
	float closest_depth = texture(light0.m_shadow_map, frag_to_light).r;
	// It is currently in linear range between [0,1]. Re-transform back to original value
	closest_depth *= light0.m_radius;
	// Now get current linear depth as the length between the fragment and light position
	float current_depth = length(frag_to_light);
	//shadow?
	float shadow = current_depth - bias <= closest_depth ? 1.0 : 0.0;
#endif
	return shadow;
}

//compute light
void compute_point_light(in point_light light0,
                         in vec4  frag_position,
                         in vec3  view_dir,
                         in vec3  normal,
                         in float shininess,
                         out point_light_res results)
{

	// Attenuation
	float attenuation = point_light_compute_attenuation(light0, frag_position.xyz);
	// Exit case
	if (attenuation <= 0.0)
	{
		results.m_diffuse = vec3(0.0);
		results.m_specular = vec3(0.0);
		return;
	}
	// Light dir
	vec3 light_dir = normalize(light0.m_position - frag_position.xyz);
	// Diffuse shading
	float diff = max(dot(normal, light_dir), 0.0);
	// Specular shading
	vec3  halfway_dir = normalize(light_dir + view_dir);
	float spec = pow(max(dot(normal, halfway_dir), 0.0), shininess);
    // Combine results
    results.m_diffuse  = light0.m_diffuse  * diff * attenuation;
    results.m_specular = light0.m_specular * spec * attenuation;
	//shadow?
	if (light0.m_use_shadow)
	{
		//compute bias
		float bias = 0.15;
		//compute shadow
		float shadow_factor = point_light_compute_shadow_light(light0, frag_position, bias);
		results.m_diffuse  *= shadow_factor;
		results.m_specular *= shadow_factor;
	}
}
