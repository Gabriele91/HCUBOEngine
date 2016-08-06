//costs
const int MAX_LIGHTS = 32;
const int DIRECTION_LIGHT = 0;
const int POINT_LIGHT     = 1;
const int SPOT_LIGHT      = 2;

//structs
struct light
{
    int   m_type;
    
    vec3  m_position;
    vec3  m_direction;
    
    vec3  m_diffuse;
    vec3  m_specular;
    
    float m_inv_constant;
    float m_inv_quad_linear;
    float m_inv_quad_quadratic;
    
    float m_inner_cut_off;
    float m_outer_cut_off;
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


float compute_attenuation(in light light0, in vec3  frag_position)
{
	float light_distance = length(light0.m_position - frag_position);
	float attenuation = clamp(  min(light0.m_inv_constant, 1.0)
                                - ( light_distance * light0.m_inv_quad_linear )
							    - ((light_distance * light_distance) * light0.m_inv_quad_quadratic), 0.0, 1.0);
	attenuation *= attenuation;
	return attenuation;
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
void calc_spot_light(in light light0,
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
}

//todo: gobo texture
//http://aschultz.us/blog/archives/153

// Calculates the color when using a light.
void compute_light(in light light0,
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
		calc_spot_light(light0,
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
		compute_light(lights[i],
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
