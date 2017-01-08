#define USE_VERTEX_SHADER
#pragma import <base_texture_normal_specular>
/////////////////////////////////////////////////////////
#pragma vertex

uniform vec2 uv_height_offset;
uniform vec2 uv_height_step;
uniform sampler2D height_map;

//define function
void vertex_shader(out vertex_output output)
{
	//compute uv
	output.m_uvmap = uv_height_offset + vertex.xy * uv_height_step;
	//compute height
	float height = texture( height_map, output.m_uvmap ).r - 0.5;
	//compute vertex
	vec4  position = vec4(output.m_uvmap.x - 0.5, height, output.m_uvmap.y - 0.5, 1.0);
	//compute vertex in model space
	output.m_world_position = transform.m_model * position;
	//compute vertex in projection space
	output.m_position = camera.m_projection * camera.m_view * output.m_world_position;
}   