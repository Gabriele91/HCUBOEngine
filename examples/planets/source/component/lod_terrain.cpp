#include <iostream>
#include <component/lod_terrain.h>
#include <hcube/math/tangent_space_calculation.h>

namespace hcube
{
	//vertex data description CPU
	struct terrain_vertex
	{
		vec3 m_position;
		vec3 m_normal;
		vec2 m_uvmap;
		vec3 m_tangent;
		vec3 m_bitangent;
		
		terrain_vertex(){}

		terrain_vertex(const vec3& in_vertex, 
			           const vec3& in_normal)
		{
			m_position = in_vertex;
			m_normal = in_normal;
		}

		terrain_vertex(const vec3& in_vertex,
			           const vec3& in_normal,
			           const vec2& in_uvmap)
		{
			m_position = in_vertex;
			m_normal = in_normal;
			m_uvmap = in_uvmap;
		}

		terrain_vertex(const vec3& in_vertex,
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
	//size
	lod_terrain::lod_terrain(ivec2 size,unsigned int levels)
	{
		m_size   = size;
		m_levels = levels;
		build();
	}
	lod_terrain::~lod_terrain()
	{
		if (m_layout) render::delete_IL(m_layout);
		if (m_vbuffer) render::delete_VBO(m_vbuffer);
	}


	void lod_terrain::build_tree()
	{
		//compute root 
		int  stride = std::pow(4, m_levels);
		m_nodes[0].build({ m_size, ivec2(0,0), m_size,{ stride, stride } });
		//count
		unsigned int n = 1;
		//build childs
		build_tree(&m_nodes[0], n, 1);
	}

	void lod_terrain::build_tree
	(
		node* parent,
		unsigned int& i_node,
		unsigned int  level 
	)
	{
		//safe: if (level == m_levels) return;
		//add childs
		parent->m_chils[0] = &m_nodes[i_node+0]; parent->m_chils[0]->m_parent = parent;
		parent->m_chils[1] = &m_nodes[i_node+1]; parent->m_chils[1]->m_parent = parent;
		parent->m_chils[2] = &m_nodes[i_node+2]; parent->m_chils[2]->m_parent = parent;
		parent->m_chils[3] = &m_nodes[i_node+3]; parent->m_chils[3]->m_parent = parent;
		i_node += 4;
		//level
		int  stride = std::pow(4, m_levels - level);
		//compute local size
		auto& p_info   = parent->m_info;
		ivec2 p_g_size = p_info.m_end - p_info.m_start;
		ivec2 l_size   = p_g_size / 2;
		ivec2 v_stride = { stride, stride };
		//coord
		{
			ivec2 start = p_info.m_start;
			ivec2 end = start + l_size;
			parent->m_chils[0]->build({ m_size, start,	end, v_stride });
		}
		{
			ivec2 start = p_info.m_start + ivec2(p_g_size.x / 2, 0);
			ivec2 end = start + l_size;
			parent->m_chils[1]->build({ m_size, start,	end, v_stride });
		}
		{
			ivec2 start = p_info.m_start + ivec2(0, p_g_size.y / 2);
			ivec2 end = start + l_size;
			parent->m_chils[2]->build({ m_size, start,	end, v_stride });
		}
		{
			ivec2 start = p_info.m_start + p_g_size / 2;
			ivec2 end = start + l_size;
			parent->m_chils[3]->build({ m_size, start,	end, v_stride });
		}
		//next levels
		++level;
		//call for all childs
		if (level < m_levels)
		{
			build_tree(parent->m_chils[0], i_node, level);
			build_tree(parent->m_chils[1], i_node, level);
			build_tree(parent->m_chils[2], i_node, level);
			build_tree(parent->m_chils[3], i_node, level);
		}
	}

	void lod_terrain::draw_tree(node* parent, unsigned int level_to_draw, unsigned int level)
	{
		if (level_to_draw == level)
		{
			//set index
			render::bind_IBO(parent->m_ibuffer);
			//draw
			render::draw_elements(DRAW_TRIANGLES, parent->m_ib_size);
			//unbind
			render::unbind_IBO(parent->m_ibuffer);
		}
		else
		{
			draw_tree(parent->m_chils[0], level_to_draw, level + 1);
			draw_tree(parent->m_chils[1], level_to_draw, level + 1);
			draw_tree(parent->m_chils[2], level_to_draw, level + 1);
			draw_tree(parent->m_chils[3], level_to_draw, level + 1);
		}

	}

	//build tree
	void lod_terrain::build()
	{
#if 0
		//get material texture
		auto texture = 
			get_material()
			->get_default_parameter(material::MAT_DEFAULT_DIFFUSE_MAP)
			->get_texture();
#endif
		//vertex data description GPU
		m_layout =
			render::create_IL({
			sizeof(terrain_vertex),
			{
				attribute{ ATT_POSITIONT, AST_FLOAT3, offsetof(terrain_vertex, m_position) },
				attribute{ ATT_NORMAL0, AST_FLOAT3,   offsetof(terrain_vertex, m_normal) },
				attribute{ ATT_TEXCOORD0, AST_FLOAT2, offsetof(terrain_vertex, m_uvmap) },
				attribute{ ATT_TANGENT0, AST_FLOAT3,  offsetof(terrain_vertex, m_tangent) },
				attribute{ ATT_BINORMAL0, AST_FLOAT3, offsetof(terrain_vertex, m_bitangent) }
			}
		});
		//buffer
		std::vector< terrain_vertex > vertices(m_size.x*m_size.y);
		//half size
		float map_height = std::sqrt(m_size.x * m_size.y) / 2.f;
		vec3 half_size   = vec3(m_size.x, 0,m_size.y)     / 2.f;
		//init all vertices
		for (unsigned int y = 0; y != m_size.y; ++y)
		for (unsigned int x = 0; x != m_size.x; ++x)
		{
#if 1
			float h = std::cos(float(x) / m_size.x * constants::pi<float>()*16.0f)*2.0f +
					  std::cos(float(y) / m_size.y * constants::pi<float>()*16.0f)*2.0f;
#else
			float h = 0;
#endif 
			vertices[y*m_size.x + x] = terrain_vertex
			(
				vec3(x, h, y) - half_size,
				vec3( 0,1,0 ),
				vec2( x , y ) / vec2(m_size)
			);
		}
		//set obb
		set_bounding_box(obb(mat3(1), vec3(0, 0, 0), vec3(half_size.x, map_height, half_size.y)));
		//compute tangents
		//tangent_space_calculation::compute_tangent_fast<terrain_vertex>(vertices);
		//alloc vertex buffer
		m_vbuffer = render::create_stream_VBO((unsigned char*)vertices.data(), sizeof(terrain_vertex), vertices.size());
		//for all childs
		size_t n_nodes = std::pow(m_levels, 4);
		m_nodes.resize(n_nodes+1);
		//build tree
		build_tree();
	}


	void lod_terrain::node::build(const lod_terrain::node::build_info& info)
	{
		std::cout << "--------------------------------------------------------------\n";
		std::cout << "g_size: " << info.m_g_size.x << ", " << info.m_g_size.y << "\n";
		std::cout << "start: "  << info.m_start.x  << ", " << info.m_start.y << "\n";
		std::cout << "end: "    << info.m_end.x << ", " << info.m_end.y << "\n";
		std::cout << "stride: " << info.m_stride.x << ", " << info.m_stride.y << "\n";
		//save
		m_info = info;
		//ref
		const ivec2& g_size = info.m_g_size;
		const ivec2& start  = info.m_start;
		const ivec2& end    = info.m_end;
		const ivec2& stride = info.m_stride;
		//size matrix
		ivec2 l_size = ivec2(info.m_end - info.m_start);
		ivec2 s_size = l_size / info.m_stride;
        //jump last
        //if(g_size.x == info.m_end.x)
            s_size.x += -1;
        //if(g_size.y == info.m_end.y)
            s_size.y += -1;
		//mapping tris
		size_t n_quad = (s_size.x) * (s_size.y);
		std::vector< unsigned int > idxs(n_quad * 6);
		{
			unsigned int i = 0;
			for (unsigned int y = 0; y < s_size.y; ++y)
			for (unsigned int x = 0; x < s_size.x; ++x)
			{
				const ivec2 g_coord = start + ivec2(y, x) * stride;
				// top
				const unsigned int point  = (g_coord.y * g_size.x + g_coord.x);
				const unsigned int point1 = point + (1 * stride.x);
				// bottom
				const unsigned int point2 = ((g_coord.y + 1 * stride.y) * g_size.x + g_coord.x);
				const unsigned int point3 = point2 + (1 * stride.x);
				//tri 1
				idxs[i * 6 + 0] = point;
				idxs[i * 6 + 1] = point3;
				idxs[i * 6 + 2] = point1;
				//tri 2
				idxs[i * 6 + 3] = point;
				idxs[i * 6 + 4] = point2;
				idxs[i * 6 + 5] = point3;
				//next
				++i;
			}
		}
		//alloc gpu memory
		m_ib_size = idxs.size();
		m_ibuffer = render::create_stream_IBO(idxs.data(), idxs.size());
	}

	//overload renderable::draw
	void lod_terrain::draw()
	{
		//bind buffer
		render::bind_VBO(m_vbuffer);
		render::bind_IL(m_layout);
		//set index
		draw_tree(&m_nodes[0], m_level_to_draw, 0);
	}

}
