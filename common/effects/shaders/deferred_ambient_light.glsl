#pragma vertex
//in
layout(location = ATT_POSITIONT) in vec3 vertex;
//out
out vec4 frag_position;

void main()
{
	frag_position = vec4(vertex, 1.0f);
	gl_Position = frag_position;
}

#pragma fragment
//in
in vec4 frag_position;
//out
out vec4 frag_color;
//uniform g_buffer
uniform sampler2D g_albedo_spec;
uniform sampler2D g_occlusion;
//uniform spot lights
uniform vec4 ambient_light;
//include screen utilities
#pragma include "lib/post_processing_utilities.glsl"
//get uv
vec2 get_uv_screen()
{
	return projection_space_to_screen_space(frag_position);
}

void main()
{
    // Retrieve data from gbuffer
    vec3    diffuse = texture(g_albedo_spec, get_uv_screen()).rgb;
	float occlusion = texture(g_occlusion,   get_uv_screen()).r;
    //output
    frag_color = vec4(diffuse * (ambient_light.rgb * occlusion),1.0);
}