
//structs
struct spot_light
{
    int   m_type;
    
    vec3  m_position;
    vec3  m_direction;
    
    vec3  m_diffuse;
    vec3  m_specular;
    
    float m_constant;
    float m_inside_radius;
    float m_radius;
    
    float m_inner_cut_off;
    float m_outer_cut_off;
	
	bool	  m_use_shadow;
	sampler2D m_shadow_map; 
	mat4	  m_shadow_projection;
	mat4	  m_shadow_view;
};
//return
struct spot_light_res
{
    vec3 m_diffuse;
    vec3 m_specular;
};
//attenuation
float spot_light_compute_attenuation(in spot_light light0, in vec3  frag_position)
{
	float light_distance = length(light0.m_position - frag_position);
	float attenuation 	 = pow(smoothstep(light0.m_radius, light0.m_inside_radius, light_distance), light0.m_constant);
	return attenuation;
}
//compute shadow light position
vec4 spot_light_compute_shadow_light_pos(in spot_light light0,vec4 frag_pos)
{
	return light0.m_shadow_projection * light0.m_shadow_view * frag_pos;
}
//compute shadow light 
float spot_light_compute_shadow_light(in spot_light light0,vec4 frag_pos)
{
	// compute pos 
	vec4 frag_pos_light_space = spot_light_compute_shadow_light_pos(light0,frag_pos);
    // perform perspective divide
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
	//-1, +1 -> 0, +1
	proj_coords = proj_coords * 0.5 + 0.5; 
	//clamp
	if(proj_coords.x <= 0.0f || proj_coords.x >=1.0) return 1.0;
	if(proj_coords.y <= 0.0f || proj_coords.y >=1.0) return 1.0;
	//depth of shadow map
	float closest_depth = texture(light0.m_shadow_map, proj_coords.xy).r; 
	//depth of current pos
	float current_depth = proj_coords.z;  
    // check whether current frag pos is in shadow
    float shadow = current_depth <= closest_depth  ? 1.0 : 0.0;
	//return
    return shadow;
}
//compute light
void compute_spot_light(in spot_light light0,
				        in vec4  frag_model_position,
                        in vec3  frag_position,
                        in vec3  view_dir,
                        in vec3  normal,
                        in float shininess,
                        out spot_light_res results)
{
    vec3 light_dir = normalize(light0.m_position - frag_position);
    // Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // Specular shading
    vec3  reflect_dir = reflect(-light_dir, normal);
    float spec        = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    // Attenuation
	float attenuation = spot_light_compute_attenuation(light0, frag_position);
    // Spotlight intensity
    float theta = dot(light_dir, normalize(-light0.m_direction));
    float epsilon = light0.m_inner_cut_off - light0.m_outer_cut_off;
    float intensity = clamp((theta - light0.m_outer_cut_off) / epsilon, 0.0, 1.0);
    // Combine results
    results.m_diffuse  = light0.m_diffuse  * diff * attenuation * intensity;
    results.m_specular = light0.m_specular * spec * attenuation * intensity;
	//shadow?
	if(light0.m_use_shadow)
	{
		float shadow_factor = spot_light_compute_shadow_light(light0,frag_model_position);
		results.m_diffuse  *= shadow_factor;
		results.m_specular *= shadow_factor;
	}
}
