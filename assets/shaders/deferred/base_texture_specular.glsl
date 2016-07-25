#pragma vertex
//in
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uvcoord;
//out 
out vec3 frag_vertex;
out vec3 frag_normal;
out vec2 frag_uvcoord;
//uniform
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	//vertex
	vec4 view_vertex = view * model * vec4(vertex, 1.0);
	frag_vertex = view_vertex.xyz;
	gl_Position = projection * view_vertex;
	//normal
	mat3 normal_matrix = transpose(inverse(mat3(model)));
	frag_normal = normal_matrix * normal;
	//uvmap
	frag_uvcoord = uvcoord;
}

#pragma fragment
//in
in vec3 frag_vertex;
in vec3 frag_normal;
in vec2 frag_uvcoord;
//out
layout(location = 0) out vec4 g_vertex;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedo_spec;
//uniform
uniform vec4      color;
uniform float     mask = 0.0;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

void main()
{
	//color
	vec4 texture_color = texture(diffuse_map, frag_uvcoord);
	if (texture_color.a <= mask) discard;
	//coords
	g_vertex       = vec4(frag_vertex, gl_FragCoord.z);
	g_normal       = normalize(frag_normal) * 0.5 + 0.5;
	float specular = texture(specular_map, frag_uvcoord).r;
	g_albedo_spec  = vec4((texture_color*color).rgb, specular);
}