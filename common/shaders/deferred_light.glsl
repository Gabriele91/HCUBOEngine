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
//include lights lib
#pragma include "lib/lights.glsl"



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
	
    //todo: material
    float shininess = 16.0f;
	
	//view dir
	vec3 view_dir = normalize(vec3(0.0) - position);

	// Then calculate lighting as usual
	vec3 lighting =  compute_all_lights(diffuse * occlusion,
								        position,
								        view_dir,
								        normal,
								        diffuse,
								        specular,
								        shininess);
    //output
	frag_color = vec4(lighting, 1.0);
}