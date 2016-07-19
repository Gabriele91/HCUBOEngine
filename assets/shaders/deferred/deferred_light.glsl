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

void main()
{
	// Retrieve data from gbuffer
	vec3  vertex   = texture(g_vertex,      frag_uvcoord).rgb;
	vec3  normal   = texture(g_normal,      frag_uvcoord).rgb;
	vec3  diffuse  = texture(g_albedo_spec, frag_uvcoord).rgb;
	float specular = texture(g_albedo_spec, frag_uvcoord).a;
	//todo
	frag_color = vec4(diffuse, 1.0);
}