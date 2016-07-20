#pragma vertex
//in
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uvcoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 binormal;
//out 
out vec3 frag_vertex;
out vec2 frag_uvcoord;
out mat3 tbn_matrix;
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
    mat3 normal_matrix = transpose(inverse(mat3(model)));
    //pass T/N
    vec3 T = normalize(normal_matrix * tangent);
    vec3 B = normalize(normal_matrix * binormal);
    vec3 N = normalize(normal_matrix * normal);
   	// tbn column-major matrix
    tbn_matrix = mat3(T.x, B.x, N.x,
                      T.y, B.y, N.y,
                      T.z, B.z, N.z);
	//uvmap
	frag_uvcoord = uvcoord;
}

#pragma fragment
//in
in vec3 frag_vertex;
in vec2 frag_uvcoord;
in mat3 tbn_matrix;
//out
layout(location = 0) out vec3 g_vertex;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedo_spec;
//uniform
uniform vec4      in_color;
uniform sampler2D texture_id;
uniform sampler2D normal_id;
//uniform matrix
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

vec3 compute_normal()
{
    //get normal texture
    vec3 text_normal = texture(normal_id, frag_uvcoord).rgb * 2.0f - 1.0f;
    //return
    return normalize( text_normal * tbn_matrix );
}

void main()
{
	//coords
	g_vertex      = frag_vertex;
    //normal map
    g_normal      = compute_normal() * 0.5 + 0.5;
	//albedo
	vec3 color    = vec3(texture(texture_id, frag_uvcoord)*in_color);
	g_albedo_spec = vec4(color, 0.0);
}