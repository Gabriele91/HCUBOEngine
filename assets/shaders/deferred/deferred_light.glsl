#pragma vertex
//in
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uvcoord;
//out
out vec2 frag_uvcoord;

void main()
{
	gl_Position  = vec4(vertex, 1.0f);
	frag_uvcoord = uvcoord;
}

#pragma fragment
//in
in vec2 frag_uvcoord;
//out
out vec4 frag_color;

//uniform
uniform sampler2D g_vertex;
uniform sampler2D g_normal;
uniform sampler2D g_albedo_spec;
uniform sampler2D g_occlusion;

uniform vec3 view_pos;
uniform vec4 ambient_light;
uniform int n_lights_used = 0;


const int DIRECTION_LIGHT = 0;
const int POINT_LIGHT     = 1;
const int SPOT_LIGHT      = 2;

struct light
{
    int   m_type;
    
    vec3  m_position;
    vec3  m_direction;
    
    vec3  m_diffuse;
    vec3  m_specular;
    
    float m_constant;
    float m_linear;
    float m_quadratic;
    
    float m_inner_cut_off;
    float m_outer_cut_off;
};
const int max_lights = 32;
uniform light lights[max_lights];

struct light_res
{
    vec3 m_diffuse;
    vec3 m_specular;
};

void calc_dir_light(in    light light0,
                    vec3  view_dir,
                    vec3  normal,
                    vec3  diffuse,
                    float specular,
                    float shininess,
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
void calc_point_light(in    light light0,
                      vec3  frag_position,
                      vec3  view_dir,
                      vec3  normal,
                      vec3  diffuse,
                      float specular,
                      float shininess,
                      out light_res results)
{
    vec3 light_dir = normalize(light0.m_position - frag_position);
    // Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // Specular shading
    vec3  reflect_dir = reflect(-light_dir, normal);
    float spec        = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    // Attenuation
    float light_distance = length(light0.m_position - frag_position);
#if 0
    float attenuation    = 1.0f / (light0.m_constant + 
								   light0.m_linear * light_distance + 
								   light0.m_quadratic * (light_distance * light_distance));
#else
	float attenuation = clamp(
		min((1.0f / light0.m_constant), 1.0)
		- (light_distance / (light0.m_linear*light0.m_linear))
		- ((light_distance * light_distance) / (light0.m_quadratic*light0.m_quadratic)), 0.0, 1.0);
	attenuation *= attenuation;
#endif
    // Combine results
    results.m_diffuse  = light0.m_diffuse  * diff * diffuse * attenuation;
    results.m_specular = light0.m_specular * spec * specular * attenuation;
}

// Calculates the color when using a spot light.
void calc_spot_light(in    light light0,
                     vec3  frag_position,
                     vec3  view_dir,
                     vec3  normal,
                     vec3  diffuse,
                     float specular,
                     float shininess,
                     out light_res results)
{
    vec3 light_dir = normalize(light0.m_position - frag_position);
    // Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // Specular shading
    vec3  reflect_dir = reflect(-light_dir, normal);
    float spec        = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    // Attenuation
    float light_distance = length(light0.m_position - frag_position);
#if 0
	float attenuation = 1.0f / (light0.m_constant +
		light0.m_linear * light_distance +
		light0.m_quadratic * (light_distance * light_distance));
#else
	float attenuation = clamp(
		  min((1.0f / light0.m_constant),1.0)
		- (light_distance / (light0.m_linear*light0.m_linear))
		- ((light_distance * light_distance) / (light0.m_quadratic*light0.m_quadratic)), 0.0, 1.0);
	attenuation *= attenuation;
#endif
    // Spotlight intensity
    float theta = dot(light_dir, normalize(-light0.m_direction));
    float epsilon = light0.m_inner_cut_off - light0.m_outer_cut_off;
    float intensity = clamp((theta - light0.m_outer_cut_off) / epsilon, 0.0, 1.0);
    // Combine results
    results.m_diffuse  = light0.m_diffuse  * diff * diffuse * attenuation * intensity;
    results.m_specular = light0.m_specular * spec * specular * attenuation * intensity;
}

void main()
{
	// Retrieve data from gbuffer
	vec3  position  = texture(g_vertex,      frag_uvcoord).rgb;
	vec3  normal    = texture(g_normal,      frag_uvcoord).rgb;
	vec3  diffuse   = texture(g_albedo_spec, frag_uvcoord).rgb;
	float specular  = texture(g_albedo_spec, frag_uvcoord).a;
	float occlusion = texture(g_occlusion,   frag_uvcoord).r;

	//unpack
	normal = normalize(normal * 2.0 - 1.0);

	// Then calculate lighting as usual
	vec3 lighting = diffuse * ambient_light.rgb * occlusion;
	vec3 view_dir = normalize(vec3(0.0) - position);

    //todo: material
    float shininess = 16.0f;
    
    //for size
    int n_lights = min(n_lights_used, max_lights);

	for (int i = 0; i < n_lights; ++i)
	{
        light_res results;
        
        switch (lights[i].m_type)
        {
            case DIRECTION_LIGHT:
                calc_dir_light(lights[i],
                               view_dir,
                               normal,
                               diffuse,
                               specular,
                               shininess,
                               results);
            break;
            case POINT_LIGHT:
                calc_point_light(lights[i],
                                position,
                                view_dir,
                                normal,
                                diffuse,
                                specular,
                                shininess,
                                results);
                break;
            case SPOT_LIGHT:
                calc_spot_light(lights[i],
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
        lighting += results.m_diffuse + results.m_specular;
	}
    //output
	frag_color = vec4(lighting, 1.0);
}