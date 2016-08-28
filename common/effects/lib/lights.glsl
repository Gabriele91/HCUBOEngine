//costs
const int DIRECTION_LIGHT = 0;
const int POINT_LIGHT     = 1;
const int SPOT_LIGHT      = 2;

//structs
struct light
{
    vec3  m_position;
    vec3  m_direction;
    
    vec3  m_diffuse;
    vec3  m_specular;
    
    float m_constant;
    float m_inside_radius;
    float m_radius;
    
    float m_inner_cut_off;
    float m_outer_cut_off;
	
	bool  m_shadow;
};

struct shadow_light
{
	mat4	  m_projection;
	mat4	  m_view;
	sampler2D m_shadow_map; 
};

struct light_res
{
    vec3 m_diffuse;
    vec3 m_specular;
};

//uniforms
uniform vec4 ambient_light;
uniform int n_lights_used = 0;
uniform light lights[MAX_LIGHTS];
uniform shadow_light shadow_lights[MAX_LIGHTS];


float compute_attenuation(in light light0, in vec3  frag_position)
{
	float light_distance = length(light0.m_position - frag_position);
	float attenuation 	 = pow(smoothstep(light0.m_radius, light0.m_inside_radius, light_distance), light0.m_constant);
	return attenuation;
}

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

// Calculates the color when using a directional light.
void calc_dir_light(in light light0,
					in vec3  view_dir,
					in vec3  normal,
					in vec3  diffuse,
					in float specular,
					in float shininess,
					out light_res results)
{
    vec3 light_dir = normalize(-light0.m_direction);
    // Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // Specular shading
    vec3  reflect_dir = reflect(-light_dir, normal);
    float spec        = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    // Combine results
    results.m_diffuse  = light0.m_diffuse  * diff * diffuse;
    results.m_specular = light0.m_specular * spec * specular;
}

// Calculates the color when using a point light.
void calc_point_light(in light light0,
					  in vec3  frag_position,
                      in vec3  view_dir,
                      in vec3  normal,
	                  in vec3  diffuse,
	                  in float specular,
	                  in float shininess,
                      out light_res results)
{
    vec3 light_dir = normalize(light0.m_position - frag_position);
    // Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // Specular shading
    vec3  reflect_dir = reflect(-light_dir, normal);
    float spec        = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    // Attenuation
	float attenuation = compute_attenuation(light0, frag_position);
    // Combine results
    results.m_diffuse  = light0.m_diffuse  * diff * diffuse * attenuation;
    results.m_specular = light0.m_specular * spec * specular * attenuation;
}

// Calculates the color when using a spot light.
void calc_spot_light(in int   i_light,
					 in light light0,
				     in vec4  frag_model_position,
                     in vec3  frag_position,
                     in vec3  view_dir,
                     in vec3  normal,
                     in vec3  diffuse,
                     in float specular,
                     in float shininess,
                     out light_res results)
{
    vec3 light_dir = normalize(light0.m_position - frag_position);
    // Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // Specular shading
    vec3  reflect_dir = reflect(-light_dir, normal);
    float spec        = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    // Attenuation
	float attenuation = compute_attenuation(light0, frag_position);
    // Spotlight intensity
    float theta = dot(light_dir, normalize(-light0.m_direction));
    float epsilon = light0.m_inner_cut_off - light0.m_outer_cut_off;
    float intensity = clamp((theta - light0.m_outer_cut_off) / epsilon, 0.0, 1.0);
    // Combine results
    results.m_diffuse  = light0.m_diffuse  * diff * diffuse * attenuation * intensity;
    results.m_specular = light0.m_specular * spec * specular * attenuation * intensity;
	//shadow?
	if(light0.m_shadow)
	{
#if 1
		vec4  shadow_light_pos = compute_shadow_light_pos(i_light,frag_model_position);
		float shadow_factor = compute_shadow_factor(i_light,shadow_light_pos);
		results.m_diffuse  *= shadow_factor;
		results.m_specular *= shadow_factor;
#endif
	}
}

//todo: gobo texture
//http://aschultz.us/blog/archives/153
//https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Cookies

// Calculates the color when using a light.
void compute_light(in int   i_light,
				   in light light0,
				   in vec4  model_position,
				   in vec3  position,
				   in vec3  view_dir,
				   in vec3  normal,
				   in vec3  diffuse,
				   in float specular,
				   in float shininess,
				   out light_res results)
{
	switch (light0.m_type)
	{
	case DIRECTION_LIGHT:
		calc_dir_light(light0,
					   view_dir,
					   normal,
                       diffuse,
					   specular,
					   shininess,
					   results);
	break;

	case POINT_LIGHT:
		calc_point_light(light0,
						 position,
						 view_dir,
						 normal,
						 diffuse,
						 specular,
						 shininess,
						 results);
	break;

	case SPOT_LIGHT:
		calc_spot_light(i_light,
						light0,
						model_position,
						position,
						view_dir,
						normal,
						diffuse,
						specular,
						shininess,
						results);
	break;

	default: break;
	}
}

// Calculates the color of all lights
vec3 compute_all_lights(in vec3  lighting,
				        in vec4  model_position,
						in vec3  position,
					    in vec3  view_dir,
					    in vec3  normal,
					    in vec3  diffuse,
					    in float specular,
					    in float shininess)
{
	//start to compute lighting
	lighting *= ambient_light.rgb;
	//for size
    int n_lights = min(n_lights_used, MAX_LIGHTS);
	//for all lights
	for (int i = 0; i < n_lights; ++i)
	{

		light_res results;
		compute_light(i,
					  lights[i],
					  model_position,
					  position,
					  view_dir,
					  normal,
					  diffuse,
					  specular,
					  shininess,
				      results);
        lighting += results.m_diffuse + results.m_specular;
	}
	
	return lighting;
}
