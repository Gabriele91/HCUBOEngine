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


struct light
{
	vec3  m_position;
	vec4  m_diffuse;
	float m_max_radius;
	float m_min_radius;
	float m_inv_intensity;
};
const int max_lights = 32;
uniform light lights[max_lights];

void main()
{
	// Retrieve data from gbuffer
	vec3  vertex    = texture(g_vertex,      frag_uvcoord).rgb;
	vec3  normal    = texture(g_normal,      frag_uvcoord).rgb;
	vec3  diffuse   = texture(g_albedo_spec, frag_uvcoord).rgb;
	float specular  = texture(g_albedo_spec, frag_uvcoord).a;
	float occlusion = texture(g_occlusion,   frag_uvcoord).r;

	//unpack
	normal = normalize(normal * 2.0 - 1.0);

	// Then calculate lighting as usual
	vec3 lighting = diffuse * ambient_light.rgb * occlusion;
	vec3 view_dir = normalize(vec3(0.0) - vertex);

    //for size
    int n_lights = min(n_lights_used, max_lights);

	for (int i = 0; i < n_lights; ++i)
	{
		// light distance vector
		vec3 light_dist_v = lights[i].m_position - vertex ;
		// Diffuse
		vec3 light_dir = normalize(light_dist_v);
		vec3 ldiffuse  = vec3(max(dot(normal, light_dir), 0.0)) * diffuse * lights[i].m_diffuse.rgb;

		// Specular
		vec3 halfway_dir = normalize(light_dir + view_dir);
		float spec       = pow(max(dot(normal, halfway_dir), 0.0), 16.0);
		vec3 lspecular   = lights[i].m_diffuse.rgb * spec * specular;

		// Attenuation
		float ldistance    = length(light_dist_v);
        float lattenuation = pow(smoothstep(lights[i].m_max_radius, lights[i].m_min_radius, ldistance), lights[i].m_inv_intensity);

        ldiffuse  *= lattenuation;
        lspecular *= lattenuation;
        lighting  += ldiffuse+lspecular;
	}
    //output
	frag_color = vec4(lighting, 1.0);
}