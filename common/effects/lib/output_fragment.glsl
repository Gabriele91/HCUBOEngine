//uniform
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

#if defined( DEFERRED_RENDERING )

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

#elif defined( FORWARD_RENDERING_COLOR )
//uniform
uniform vec4 ambient_light;
//output
out vec4 frag_color;
//define 
#define output_fragment(position,normal,albedo,specular) forward_color_output_fragment(albedo)
//compute
void forward_color_output_fragment(in vec4 albedo)
{	
    //output
	frag_color = albedo * ambient_light;
}

#elif defined( FORWARD_RENDERING_SPOT_LIGHT )
//light uniform
#pragma include "spot_light.glsl"
//uniform 
uniform spot_light spot_lights[1];
//output
out vec4 frag_color;
//define 
#define output_fragment(position,normal,albedo,specular)\
	forward_light_output_fragment(position,normal,specular)
//compute
void forward_light_output_fragment(in vec4  position,
								   in vec3  normal,
								   in float specular)
{	
    //todo: material
    float shininess = 16.0f;
		
	//pos view
	vec3 view_pos = vec3(view*position);
	
	//view dir
	vec3 view_dir = normalize(vec3(0.0) - view_pos);
	
	//result
	spot_light_res light_results;
	
	// Then calculate lighting as usual
	compute_spot_light(spot_lights[0],
						position,
						view_pos,
						view_dir,
						normal,
						shininess,
						light_results);
    //output
	frag_color = vec4(light_results.m_diffuse + light_results.m_specular * specular,1.0);
}

#else
//output
out vec4 frag_color;
//void output
#defined output_fragment(position,normal,albedo,specular) frag_color=vec4(0.0f);
//endif
#endif