#pragma vertex
//in
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uvcoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangents;
//out 
out vec3 frag_vertex;
out vec2 frag_uvcoord;
out mat3 tbn;
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
    mat3 normal_mat = transpose(inverse(mat3(view * model)));
    //pass T/B/N
    vec3 t_pixel = normalize(normal_mat * tangent);
    vec3 n_pixel = normalize(normal_mat * normal);
	vec3 b_pixel = normalize(normal_mat * bitangents);
    //..
    tbn = transpose(mat3(t_pixel.x, b_pixel.x, n_pixel.x,
				         t_pixel.y, b_pixel.y, n_pixel.y,
                         t_pixel.z, b_pixel.z, n_pixel.z));
	//uvmap
	frag_uvcoord = uvcoord;
}

#pragma fragment
//in
in vec3 frag_vertex;
in vec2 frag_uvcoord;
in mat3 tbn;
//out
#pragma include "output_fragment.glsl"
//uniform
uniform vec4      color;
uniform float     mask = 0.0;
uniform sampler2D diffuse_map;
uniform sampler2D normal_map;
uniform sampler2D specular_map;
//uniform matrix
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

vec3 compute_normal()
{
    //get normal texture
    vec3 text_normal = normalize( texture(normal_map, frag_uvcoord).rgb * 2.0f - 1.0f );
    //return
    return tbn*text_normal;
}

void main()
{
	//color
	vec4 texture_color = texture(diffuse_map, frag_uvcoord);
	if (texture_color.a <= mask) discard;
	//specular
	float specular = texture(specular_map, frag_uvcoord).r;
	//outputs
	output_fragment(frag_vertex, compute_normal(), (texture_color*color), specular);
}