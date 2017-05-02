#define USE_VERTEX_SHADER
#define USE_GEOMETRY_SHADER
//enable shaders (trick)
#pragma vertex   //enable
#pragma fragment //enable
#pragma geometry //enable
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
#if 0
	float h_P = compute_terrain_height_norm(pos);
	//read neightbor heights 
	float h_L = compute_terrain_height_norm(pos + vec2(-uv_height_step.x,0.0));
	float h_R = compute_terrain_height_norm(pos + vec2( uv_height_step.x,0.0));
	
	float h_D = compute_terrain_height_norm(pos + vec2(0.0,-uv_height_step.y));
	float h_U = compute_terrain_height_norm(pos + vec2(0.0, uv_height_step.y));  
	// deduce terrain normal
	//To make it easier we offset the points such that n is "0" height
    vec3 va = vec3(0,  1,  (h_U - h_P) );
    vec3 vb = vec3(1,  0,  (h_R - h_P) );
    vec3 vc = vec3(0, -1,  (h_D - h_P) );
    vec3 vd = vec3(-1, 0,  (h_L - h_P) );
    //cross products of each vector yields the normal of each tri - return the average normal of all 4 tris
    vec3 average_n = ( cross(va, vb) + cross(vb, vc) + cross(vc, vd) + cross(vd, va) ) / -4.0;
	//normal
    return normalize( average_n );
#else 
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
#endif 
}
/////////////////////////////////////////////////////////
#pragma vertex
//in number of texture ripetition
uniform vec2 uv_mapping_ripetition;
//define function
void vertex_shader(out vertex_output output)
{
	//compute uv
	output.m_uvmap = uv_height_offset + vertex.xy * uv_height_step;
	//compute height
	float height = compute_terrain_height(output.m_uvmap);
	//compute vertex
	vec4  position = vec4(output.m_uvmap.x - 0.5, height, output.m_uvmap.y - 0.5, 1.0);
	//compute vertex in model space
	output.m_world_position = transform.m_model * position;
	//uv factor	
	output.m_uvmap *= uv_mapping_ripetition;
	//compute vertex in projection space
	output.m_position = camera.m_projection * camera.m_view * output.m_world_position;
}

#pragma geometry
//layouts
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

//in 
in vec4 vertex_position[3];
in vec2 vertex_uvcoord[3];
in mat3 vertex_tbn[3];

//out 
out vec4 frag_position;
out vec2 frag_uvcoord;
out mat3 frag_tbn;

//compute tbn
mat3 compute_terrain_tbn(in mat3 normal_matrix,in int v0,in int v1,in int v2)
{
	//tbn
	vec3  normal_0    = compute_vertex_normal(vertex_uvcoord[v0], vec3( 0, 1, 0 ));
	//tangent and bitangent
	#if 0
		//compute standard edges
		vec3 edge1     = vertex_position[v1].xyz - vertex_position[v0].xyz;
		vec3 edge2     = vertex_position[v2].xyz - vertex_position[v0].xyz;
		vec2 delta_uv1 = vertex_uvcoord[v1] - vertex_uvcoord[v0];
		vec2 delta_uv2 = vertex_uvcoord[v2] - vertex_uvcoord[v0];
		//compute div
		float div = (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);
		float r = 1.0f / div;
		vec3  tangent_0   = (edge1 * delta_uv2.y - edge2 * delta_uv1.y)*r;
		vec3  bitangent_0 = (edge2 * delta_uv1.x - edge1 * delta_uv2.x)*r;
	#else
		//fast way
		vec3 tangent_der = vec3( 1, 0, 0 );
		//vec3 tangent_0   = cross( tangent_der, normal_0 );
		//vec3 bitangent_0 = cross( tangent_0, normal_0 );
		vec3 bitangent_0   = cross( tangent_der, normal_0 );
		vec3 tangent_0 	   = cross( bitangent_0, normal_0 );
	#endif
	return compute_tbn(normal_matrix, normal_0, tangent_0, bitangent_0);
}

void main()
{
	//compute matrix
	mat3 normal_matrix = transpose(inverse(mat3(transform.m_model)));
	//compute vertexs	
	//0
	frag_position = vertex_position[0];
	frag_uvcoord  = vertex_uvcoord[0];
	frag_tbn      = compute_terrain_tbn(normal_matrix, 0, 1, 2);
	gl_Position   = gl_in[0].gl_Position;
	EmitVertex();	
	//1
	frag_position = vertex_position[1];
	frag_uvcoord  = vertex_uvcoord[1];
	frag_tbn      = compute_terrain_tbn(normal_matrix, 1, 0, 2);
	gl_Position   = gl_in[1].gl_Position;
	EmitVertex();
	//2
	frag_position = vertex_position[2];
	frag_uvcoord  = vertex_uvcoord[2];
	frag_tbn      = compute_terrain_tbn(normal_matrix, 2, 1, 0);
	gl_Position   = gl_in[2].gl_Position;
	EmitVertex();
	//end
	EndPrimitive();
}
