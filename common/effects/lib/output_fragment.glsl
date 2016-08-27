//uniform
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

#ifdef DEFERRED_RENDERING

layout(location = 0) out vec4 g_position;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedo_spec;

void output_fragment(in vec4 position,
					 in vec3 normal,
					 in vec4 albedo, 
					 in float specular)
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
void output_fragment(in vec4  position,
					 in vec3  normal,
					 in vec4  albedo,
					 in float specular)
{	
	//diffuse
	vec3 diffuse = albedo.rgb;
	
	//unpack
	/* is no packet normal = normalize(normal * 2.0 - 1.0); */
	
    //todo: material
    float shininess = 16.0f;
	
	
	//pos view
	vec3 view_pos = vec3(view*position);
	
	//view dir
	vec3 view_dir = normalize(vec3(0.0) - view_pos);
	
	// Then calculate lighting as usual
	vec3 lighting =  compute_all_lights(diffuse /* * occlusion*/,
								        position,
										view_pos,
								        view_dir,
								        normal,
								        diffuse,
								        specular,
								        shininess);
    //output
	frag_color = vec4(lighting, albedo.a);
}

#endif