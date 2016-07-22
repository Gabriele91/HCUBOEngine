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
	vec4 world_vertex = model * vec4(vertex, 1.0);
	frag_vertex = world_vertex.xyz;
    gl_Position = projection * view * world_vertex;
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
layout(location = 0) out vec3 g_vertex;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedo_spec;
//uniform
uniform vec4      color;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

vec4 gamma_correction_tex(sampler2D tex,vec2 coords)
{
    const vec3 gamma = vec3(2.2);
    vec4 texturetex = texture(tex, coords);
    vec3 diffuse = pow(texturetex.rgb, gamma);
    return vec4(diffuse,texturetex.a);
}

void main()
{
	//coords
	g_vertex       = frag_vertex;
	g_normal       = normalize(frag_normal) * 0.5 + 0.5;
	//albedo
	vec3 color     = vec3(gamma_correction_tex(diffuse_map, frag_uvcoord)*color);
	float specular = texture(specular_map, frag_uvcoord).r;
	g_albedo_spec  = vec4(color, specular);
}