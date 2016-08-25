#pragma vertex
//in
layout(location = ATT_POSITIONT) in vec3 vertex;
layout(location = ATT_NORMAL0)   in vec3 normal;
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
#pragma include "output_fragment.glsl"
//uniform
uniform vec4 color;

void main()
{
	//outputs
	output_fragment(frag_vertex, normalize(frag_normal), color, 1.0);
}