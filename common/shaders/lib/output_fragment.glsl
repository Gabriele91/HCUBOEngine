#ifdef DEFERRED_RENDERING

layout(location = 0) out vec3 g_position;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedo_spec;

void output_fragment(vec3 position,vec3 normal,vec4 albedo, float specular)
{
	g_position	   = position;
    g_normal       = normal * 0.5 + 0.5;
	g_albedo_spec  = vec4(albedo.rgb,specular);
}

#else
//light uniform
#pragma include "lights.glsl"
//output
out vec4 frag_color;
//compute
void output_fragment(vec3 position,vec3 normal,vec4 albedo, float specular)
{	
	//diffuse
	vec3 diffuse = albedo.rgb;
	
    //todo: material
    float shininess = 16.0f;
	
	//view dir
	vec3 view_dir = normalize(vec3(0.0) - position);

	// Then calculate lighting as usual
	vec3 lighting =  compute_all_lights(diffuse /* * occlusion*/,
								        position,
								        view_dir,
								        normal,
								        diffuse,
								        specular,
								        shininess);
    //output
	frag_color = vec4(lighting, albedo.a);
}

#endif