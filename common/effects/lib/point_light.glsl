
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
//compute light
void compute_point_light(in point_light light0,
                         in vec3  frag_position,
                         in vec3  view_dir,
                         in vec3  normal,
                         in float shininess,
                         out point_light_res results)
{
    vec3 light_dir = normalize(light0.m_position - frag_position);
    // Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // Specular shading
    vec3  reflect_dir = reflect(-light_dir, normal);
    float spec        = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    // Attenuation
	float attenuation = point_light_compute_attenuation(light0, frag_position);
    // Combine results
    results.m_diffuse  = light0.m_diffuse  * diff * attenuation;
    results.m_specular = light0.m_specular * spec * attenuation;
}
