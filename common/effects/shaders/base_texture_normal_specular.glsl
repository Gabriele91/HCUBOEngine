#pragma vertex
/*
hcube_input
{
	vec2 vertex     : ATT_POSITIONT
	vec2 normal     : ATT_NORMAL0
	vec2 uvcoord    : ATT_TEXCOORD0
	vec2 tangent    : ATT_TANGENT0
	vec2 bitangents : ATT_BINORMAL0
}
*/
//in
layout(location = ATT_POSITIONT) in vec3 vertex;
layout(location = ATT_NORMAL0)   in vec3 normal;
layout(location = ATT_TEXCOORD0) in vec2 uvcoord;
layout(location = ATT_TANGENT0)  in vec3 tangent;
layout(location = ATT_BINORMAL0) in vec3 bitangents;
//out 
out vec4 vertex_position;
out vec2 vertex_uvcoord;
out mat3 vertex_tbn;
//uniform
#pragma include "lib/uniform.glsl"
#pragma include "lib/utilities.glsl"


#ifndef USE_VERTEX_SHADER

void main()
{
	vertex_position = transform.m_model * vec4(vertex, 1.0);
	vertex_tbn		= compute_tbn(transpose(inverse(mat3(transform.m_model))), normal, tangent, bitangents);
	vertex_uvcoord  = uvcoord;
	gl_Position     = camera.m_projection * camera.m_view * vertex_position;
}

#else 

//output struct
struct vertex_output
{
	vec4 m_world_position;
	vec2 m_uvmap;
	mat3 m_tbn;
	vec4 m_position;
};
//define function
void vertex_shader(out vertex_output output);
//call function
void main()
{
	//execute shader
	vertex_output output;
	vertex_shader(output);
	//output shacer
	vertex_position 	 = output.m_world_position;
	vertex_tbn			 = output.m_tbn;
	vertex_uvcoord       = output.m_uvmap;
	gl_Position  		 = output.m_position;
}

#endif


#pragma fragment
//in 
in vec4 vertex_position;
in vec2 vertex_uvcoord;
in mat3 vertex_tbn;
//out
#pragma include "lib/output_fragment.glsl"
//uniform
uniform vec4      color;
uniform float     mask = 0.0;
uniform sampler2D diffuse_map;
uniform sampler2D normal_map;
uniform sampler2D specular_map;

vec3 compute_normal()
{
	#define INV_X_NORMAL_MAP
    //get normal texture
	vec3 tex_normal = texture(normal_map, vertex_uvcoord).rgb;
	//
#if defined (INV_X_NORMAL_MAP)
	tex_normal.r = 1.0f-tex_normal.r;
#endif
#if defined (INV_Y_NORMAL_MAP)
	tex_normal.g = 1.0f-tex_normal.g;
#endif
#if defined (INV_Z_NORMAL_MAP)
	tex_normal.b = 1.0f-tex_normal.b;
#endif
	//normal image
    vec3 normal = normalize( tex_normal * 2.0f - 1.0f );
    //return
    return vertex_tbn*normal;
}


#ifndef USE_FRAGMENT_SHADER

void main()
{
	//color
	vec4 texture_color = texture(diffuse_map, vertex_uvcoord);
	//mask
#ifndef DISABLE_MASK
	if (texture_color.a <= mask) discard;
#endif
	//specular
	float specular = texture(specular_map, vertex_uvcoord).r;
	//outputs
	output_fragment(vertex_position, compute_normal(), (texture_color*color), specular);
}

#else 

void fragment_shader(inout vec4 position, inout vec3 normal, inout vec4 color, inout float specular);

void main()
{		
	//color
	vec4 texture_color = texture(diffuse_map, vertex_uvcoord);
	//mask
#ifndef DISABLE_MASK
	if (texture_color.a <= mask) discard;
#endif
	//values
	vec4  v_position = vertex_position;
	vec3  v_normal   = compute_normal();
	vec4  v_color    = texture_color*color;
	float v_specular = texture(specular_map, vertex_uvcoord).r;
	//shader
	fragment_shader(v_position, v_normal, v_color, v_specular);
	//outputs
	output_fragment(v_position, v_normal, v_color, v_specular);
}

#endif 