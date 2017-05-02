#define USE_VERTEX_SHADER
#define USE_FRAGMENT_SHADER
/////////////////////////////////////////////////////////
#pragma import <base_texture_normal_specular>
/////////////////////////////////////////////////////////
#pragma all

uniform vec2 uv_height_offset;
uniform vec2 uv_height_step;
uniform sampler2D height_map;

float compute_terrain_height(in vec2 pos)
{
	return texture( height_map, pos ).r - 0.5;
}

float compute_terrain_height_norm(in vec2 pos)
{
	return texture( height_map, pos ).r;
}

vec3 compute_vertex_normal(in vec2 pos,in vec3 default_normal)
{
	float me = compute_terrain_height_norm(pos);
	float n = compute_terrain_height_norm(pos + vec2(0.0, uv_height_step.y));
	float s = compute_terrain_height_norm(pos + vec2(0.0,-uv_height_step.y));
	float e = compute_terrain_height_norm(pos + vec2(-uv_height_step.x,0.0));
	float w = compute_terrain_height_norm(pos + vec2( uv_height_step.x,0.0));

	vec3 norm = default_normal;
	vec3 temp = norm; //a temporary vector that is not parallel to norm
	if(norm.x==1) temp.y+=0.5;
	else          temp.x+=0.5;

	//form a basis with norm being one of the axes:
	vec3 perp1 = normalize(cross(norm,temp));
	vec3 perp2 = normalize(cross(norm,perp1));

	//use the basis to move the normal in its own space by the offset
	vec3 normalOffset = -1.0 * ( ( (n-me) - (s-me) ) * perp1 + ( ( e - me ) - ( w - me ) ) * perp2 );
	norm += normalOffset;
	
	return normalize(norm);
}

/////////////////////////////////////////////////////////
#pragma vertex
//in number of texture ripetition
uniform vec2 uv_mapping_ripetition;
uniform vec2 offset_model;
//map height
out float height;

//get tbn matrix 
mat3 compute_tbn_from_height(in vec2 pos2D)
{	
	const vec3 tangent_normal = vec3( 1, 0, 0 );
	const vec3 normal_normal  = vec3( 0, 1, 0 );
	//compute matrix
	mat3 normal_matrix = transpose(inverse(mat3(transform.m_model)));
	//TBN
	vec3    normal   = compute_vertex_normal(pos2D, normal_normal);
	vec3 bitangent   = cross( tangent_normal, normal );
	vec3 tangent 	 = cross( bitangent, normal );
	//
	return compute_tbn(normal_matrix, normal, tangent, bitangent);
}

//define function
void vertex_shader(out vertex_output output)
{
	//compute uv
	output.m_uvmap = uv_height_offset + vertex.xy * uv_height_step;
	//compute height
	height = compute_terrain_height(output.m_uvmap);
	//compute vertex
	vec4  position = vec4(output.m_uvmap.x + offset_model.x, height, output.m_uvmap.y + offset_model.y, 1.0);
	//compute vertex in model space
	output.m_world_position = transform.m_model * position;
	//uv factor	
	output.m_uvmap *= uv_mapping_ripetition;
	//compute vertex in projection space
	output.m_position = camera.m_projection * camera.m_view * output.m_world_position;
	//compute tbn
	output.m_tbn = compute_tbn_from_height(output.m_uvmap);
}

/////////////////////////////////////////////////////////
#pragma fragment				

uniform sampler2D diffuse_map_1;
uniform sampler2D normal_map_1;
uniform sampler2D specular_map_1;

uniform sampler2D diffuse_map_2;
uniform sampler2D normal_map_2;
uniform sampler2D specular_map_2;

uniform sampler2D diffuse_map_3;
uniform sampler2D normal_map_3;
uniform sampler2D specular_map_3;

uniform vec3 levels;
//map height
in float height;

//compute color, specular and normal 
float fragment_height_shader(in float h, float f, inout vec3 normal, inout vec4 io_color, inout float specular)
{
	//calc mie
	if(h > (levels.x+f))
	{
		io_color = texture(diffuse_map_3,frag_uvcoord)*color;
		normal   = compute_normal(normal_map_3,frag_uvcoord);
		specular = texture(specular_map_3,frag_uvcoord).r;
		return levels.x;
	}
	else if(h > (levels.y+f))
	{	
		io_color = texture(diffuse_map_2,frag_uvcoord)*color;
		normal   = compute_normal(normal_map_2,frag_uvcoord);
		specular = texture(specular_map_2,frag_uvcoord).r;
		return levels.y;
	}
	else if(h > (levels.z+f))
	{
		io_color = texture(diffuse_map_1,frag_uvcoord)*color;
		normal   = compute_normal(normal_map_1,frag_uvcoord);
		specular = texture(specular_map_1,frag_uvcoord).r;		
		return levels.z;
	}
	return 0.0;
}

void fragment_shader(inout vec4 position, inout vec3 normal, inout vec4 io_color, inout float specular)
{
	float h  = height+0.5;
	const float f = 0.01;
	//calc color
	vec4 c1 = io_color; vec3 n1 = normal; float s1 = specular;
	vec4 c2 = io_color; vec3 n2 = normal; float s2 = specular;	
	
	float l1 = fragment_height_shader(h, f,n1,c1,s1);
	float l2 = fragment_height_shader(h,-f,n2,c2,s2);
	
	float l_factor = saturate(
	  (h-(l2-f))/(f*2)
	); 
	
	io_color = lerp(c1,c2,l_factor);
	normal   = lerp(n1,n2,l_factor);
	specular = lerp(s1,s2,l_factor);
}