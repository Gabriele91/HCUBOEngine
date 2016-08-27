
struct shadow_light
{
	mat4	  m_projection;
	mat4	  m_view;
	sampler2D m_shadow_map; 
};
uniform int n_shadow_lights_used = 0;
uniform shadow_light shadow_lights[MAX_LIGHTS];

vec4 compute_shadow_light_pos(int i_light,vec4 frag_pos)
{
	return shadow_lights[i_light].m_projection * shadow_lights[i_light].m_view * frag_pos;
}

float compute_shadow_factor(int i_light,vec4 frag_pos_light_space)
{
    // perform perspective divide
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
	//-1, +1 -> 0, +1
	proj_coords = proj_coords * 0.5 + 0.5; 
	//clamp
	if(proj_coords.x <= 0.0f || proj_coords.x >=1.0) return 1.0;
	if(proj_coords.y <= 0.0f || proj_coords.y >=1.0) return 1.0;
	//depth of shadow map
	float closest_depth = texture(shadow_lights[i_light].m_shadow_map, proj_coords.xy).r; 
	//depth of current pos
	float current_depth = proj_coords.z;  
    // check whether current frag pos is in shadow
    float shadow = current_depth <= closest_depth  ? 1.0 : 0.0;
	//return
    return shadow;
}

// Calculates the shadow of all cast shadow
float compute_all_shadow_lights(vec4 frag_model_pos)
{
	float result = 1.0f;
	//for size
    int n_shadow_lights = min(n_shadow_lights_used, MAX_LIGHTS);
	//for all shadow lights
	for (int i = 0; i < n_shadow_lights; ++i)
	{
		//compue light pos
		vec4 frag_pos_light_space = compute_shadow_light_pos(i,frag_model_pos);
		//compute shadow
		result *= compute_shadow_factor(i,frag_pos_light_space);
	}
	//return 
	return result;
}
