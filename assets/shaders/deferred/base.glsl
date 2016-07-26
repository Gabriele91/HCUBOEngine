#pragma vertex
//in
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
//out 
out vec3 frag_vertex;
out vec3 frag_normal;
//uniform
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	//vertex
	vec4 view_vertex = view * model * vec4(vertex, 1.0);
	frag_vertex = view_vertex.xyz;
	gl_Position = projection *  view_vertex;
	//normal
	mat3 normal_matrix = transpose(inverse(mat3(view * model)));
	frag_normal = normal_matrix * normal;
}

#pragma fragment
//in
in vec3 frag_vertex;
in vec3 frag_normal;
//out
layout(location = 0) out vec3 g_vertex;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedo_spec;
//uniform
uniform vec4 color;

void main()
{
	g_vertex = frag_vertex;
	g_normal = normalize(frag_normal) * 0.5 + 0.5;
	g_albedo_spec = vec4(color.rgb, 1.0);
}