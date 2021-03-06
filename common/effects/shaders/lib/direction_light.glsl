
//structs
struct direction_light
{
    vec3  m_direction;
    vec3  m_diffuse;
    vec3  m_specular;
};

//return
struct direction_light_res
{
    vec3 m_diffuse;
    vec3 m_specular;
};

//compute light
void compute_direction_light(in direction_light light0,
                             in vec3  view_dir,
                             in vec3  normal,
                             in float shininess,
                             out direction_light_res results)
{
	// Light dir
	vec3 light_dir = normalize(light0.m_direction);
	// Diffuse shading
	float diff = max(dot(normal, light_dir), 0.0);
	// Specular shading
	vec3  halfway_dir = normalize(light_dir + view_dir);
	float spec = pow(max(dot(normal, halfway_dir), 0.0), shininess);
    // Combine results
    results.m_diffuse  = light0.m_diffuse  * diff;
    results.m_specular = light0.m_specular * spec;
}