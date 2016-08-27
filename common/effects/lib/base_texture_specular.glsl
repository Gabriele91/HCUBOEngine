#pragma vertex
//in
layout(location = ATT_POSITIONT) in vec3 vertex;
layout(location = ATT_NORMAL0)   in vec3 normal;
layout(location = ATT_TEXCOORD0) in vec2 uvcoord;
//out 
out vec4 frag_position;
out vec3 frag_normal;
out vec2 frag_uvcoord;
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
	//uvmap
	frag_uvcoord = uvcoord;
}

#pragma fragment
//in
in vec4 frag_position;
in vec3 frag_normal;
in vec2 frag_uvcoord;
//out
#pragma include "output_fragment.glsl"
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
	//specular
	float specular = texture(specular_map, frag_uvcoord).r;
	//outputs
	output_fragment(frag_position, normalize(frag_normal), (texture_color*color), specular);
}