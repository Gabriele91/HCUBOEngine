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
    vec4 world_vertex = model * vec4(vertex, 1.0);
	frag_vertex = world_vertex.xyz;
    gl_Position = projection * view * world_vertex;
    //normal
    mat3 normal_mat = transpose(inverse(mat3(model)));
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
layout(location = 0) out vec3 g_vertex;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedo_spec;
//uniform
uniform vec4      color;
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
	//coords
	g_vertex      = frag_vertex;
    //normal map
    g_normal      = compute_normal() * 0.5 + 0.5;
	//albedo
	vec3 color     = vec3(texture(diffuse_map, frag_uvcoord)*color);
	float specular = texture(specular_map, frag_uvcoord).r;
	g_albedo_spec  = vec4(color, specular*0.5);
}