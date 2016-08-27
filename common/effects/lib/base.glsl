#pragma vertex
//in
layout(location = ATT_POSITIONT) in vec3 vertex;
layout(location = ATT_NORMAL0)   in vec3 normal;
//out 
out vec4 frag_position;
out vec3 frag_normal;
//uniform
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	//vertex
	frag_position     = model * vec4(vertex, 1.0);
	gl_Position 	  = projection * view * frag_position;
	//normal
	mat3 normal_matrix = transpose(inverse(mat3(view * model)));
	frag_normal = normal_matrix * normal;
}

#pragma fragment
//in
in vec4 frag_position;
in vec3 frag_normal;
//out
#pragma include "output_fragment.glsl"
//uniform
uniform vec4 color;

void main()
{
	//outputs
	output_fragment(frag_position, normalize(frag_normal), color, 1.0);
}