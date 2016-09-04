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
#elif defined( FORWARD_RENDERING )
//include
#pragma include "output_fragment_forward.glsl"
//
#else
//output
out vec4 frag_color;
//void output
#defined output_fragment(position,normal,albedo,specular) frag_color=vec4(0.0f);
//endif
#endif