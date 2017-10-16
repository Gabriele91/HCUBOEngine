#include <component/lod_sphere_tree.h>

namespace hcube
{
	load_sphere_tree::load_sphere_tree
	(
		  float range
		, unsigned int factor
		, unsigned int end_level
	)
	{
		m_range  = range;
		m_factor = factor;
		m_levels = end_level;
	}

	void load_sphere_tree::build()
	{
		//vertex data description CPU
		struct vertex
		{
			vec3 m_position;
			vec3 m_normal;
			vec2 m_uvmap;
			vec3 m_tangent;
			vec3 m_bitangent;

			vertex(const vec3& in_vertex,
				const vec3& in_normal)
			{
				m_position = in_vertex;
				m_normal = in_normal;
			}

			vertex(const vec3& in_vertex,
				const vec3& in_normal,
				const vec2& in_uvmap)
			{
				m_position = in_vertex;
				m_normal = in_normal;
				m_uvmap = in_uvmap;
			}

			vertex(const vec3& in_vertex,
				const vec3& in_normal,
				const vec2& in_uvmap,
				const vec3& in_tangent,
				const vec3& in_bitangent)
			{
				m_position = in_vertex;
				m_normal = in_normal;
				m_uvmap = in_uvmap;
				m_tangent = in_tangent;
				m_bitangent = in_bitangent;
			}
		};
		//vertex data description GPU
		m_layout = 
		render::create_IL({
			sizeof(vertex),
			{
				attribute{ ATT_POSITIONT, AST_FLOAT3, offsetof(vertex, m_position) },
				attribute{ ATT_NORMAL0, AST_FLOAT3,   offsetof(vertex, m_normal) },
				attribute{ ATT_TEXCOORD0, AST_FLOAT2, offsetof(vertex, m_uvmap) },
				attribute{ ATT_TANGENT0, AST_FLOAT3,  offsetof(vertex, m_tangent) },
				attribute{ ATT_BINORMAL0, AST_FLOAT3, offsetof(vertex, m_bitangent) }
			}
		});
	}

	//overload renderable::draw
	void load_sphere_tree::draw(rendering_system& rsystem, entity::ptr view)
	{
		//
	}

	//overload component::copy 
	component_ptr load_sphere_tree::copy() const
	{
		return load_sphere_tree::snew(m_range, m_factor, m_levels);
	}
}
