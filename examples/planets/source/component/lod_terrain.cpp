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
	lod_terrain::lod_terrain(ivec2 size, unsigned int levels)
	{
		m_size = size;
		m_levels = levels;
		build();
	}
	
	lod_terrain::lod_terrain(material_ptr material,ivec2 size, unsigned int levels)
	{
		set_material(material);
		m_size = size;
		m_levels = levels;
		build();
		recompute_mesh_heigth();
	}

	void lod_terrain::set_material(material_ptr material)
	{
		//set
		renderable::set_material(material);
		//update vertex
		recompute_mesh_heigth();
	}

	void lod_terrain::recompute_mesh_heigth()
	{
		//return;
		//param
		auto param = get_material()->get_default_parameter(material::MAT_DEFAULT_DIFFUSE_MAP);
		//test
		if (!param) return;
		//get material texture
		auto diff_texture = param->get_texture();
		//test
		if (!diff_texture) return;
		//get texture
		auto raw_image = render::get_texture(diff_texture->get_context_texture());
		//type
		int channels = 0;
		switch (diff_texture->get_type())
		{
			case texture_type::TT_R:    channels = 1; break;
			case texture_type::TT_RG:   channels = 2; break;
			case texture_type::TT_RGB:  channels = 3; break;
			case texture_type::TT_RGBA: channels = 4; break;
			default: break;
		}
		//hfunction
		auto get_height = [&](float normx, float normy) -> float
		{
			int y = normy * (diff_texture->get_height() - 1);
			int x = normx * (diff_texture->get_width() - 1);
			int pixel = (y * diff_texture->get_width() + x) * channels;
			return float(raw_image[pixel]) / 255.;
		};
		//size buffer
		size_t n_vertex = m_p_size.x*m_p_size.y;
		//half size
		vec3 half_size = vec3(m_size.x, 0, m_size.y) / 2.f;
		//map h 
		float map_height = 0.01;
		//get vertexs
		terrain_vertex* vertices = (terrain_vertex*)render::map_VBO(m_vbuffer, 0, sizeof(terrain_vertex)*n_vertex, MAP_WRITE_AND_READ);
		HCUBE_RENDER_PRINT_ERRORS
		#pragma omp parallel for num_threads(4)
		for (int y = 0; y < m_p_size.y; ++y)
		{
			for (int x = 0; x < m_p_size.x; ++x)
			{
				float h = get_height(float(x) / (m_p_size.x - 1), float(y) / (m_p_size.y - 1));
				vertices[y*m_p_size.x + x].m_position.y = h;
				//max
				map_height = std::max(map_height, h);
			}
		}
		//set obb
		set_bounding_box(obb(mat3(1), vec3(0, 0, 0), vec3(0.5, map_height, 0.5)));
		//unmap
		render::unmap_VBO(m_vbuffer);
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
		m_nodes[0].build({ m_p_size, ivec2(0,0), m_size,{ stride, stride } });
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
			parent->m_chils[0]->build({ m_p_size, start, end, v_stride });
		}
		{
			ivec2 start = p_info.m_start + ivec2(p_g_size.x / 2, 0);
			ivec2 end = start + l_size;
			parent->m_chils[1]->build({ m_p_size, start, end, v_stride });
		}
		{
			ivec2 start = p_info.m_start + ivec2(0, p_g_size.y / 2);
			ivec2 end = start + l_size;
			parent->m_chils[2]->build({ m_p_size, start, end, v_stride });
		}
		{
			ivec2 start = p_info.m_start + p_g_size / 2;
			ivec2 end = start + l_size;
			parent->m_chils[3]->build({ m_p_size, start, end, v_stride });
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
		//points size
		m_p_size = m_size + ivec2(1,1);
		//buffer
		std::vector< terrain_vertex > vertices(m_p_size.x*m_p_size.y);
		//half size
		vec3 half_size   = vec3(m_size.x, 0,m_size.y) / 2.f;
		//map h 
		float map_height = 0.01;
		//init all vertices
		#pragma omp parallel for num_threads(4)
		for (int y = 0; y < m_p_size.y; ++y)
		{
			for (int x = 0; x < m_p_size.x; ++x)
			{
				vertices[y*m_p_size.x + x] = terrain_vertex
				(
					(vec3(x, 0, y) - half_size) / vec3(m_p_size.x, 1, m_p_size.y),
					vec3(0, 1, 0),
					vec2(x, y) / vec2(m_size)
				);
			}
		}
		//set obb
		set_bounding_box(obb(mat3(1), vec3(0, 0, 0), vec3(0.5, map_height, 0.5)));
		//compute tangents
		//tangent_space_calculation::compute_tangent_fast<terrain_vertex>(vertices);
		//alloc vertex buffer
		size_t size_vers = sizeof(terrain_vertex) * vertices.size();
#if 0
		std::cout << "size: " << size_vers << " B" << std::endl;
		std::cout << "size: " << size_vers / 1024 << " KB" << std::endl;
		std::cout << "size: " << size_vers / 1024 / 1024 << " MB" << std::endl;
#endif
		m_vbuffer = render::create_stream_VBO((unsigned char*)vertices.data(), sizeof(terrain_vertex), vertices.size());
		//errors
		HCUBE_RENDER_PRINT_ERRORS
		//for all childs
		size_t n_nodes = std::pow(m_levels, 4);
		m_nodes.resize(n_nodes+1);
		//build tree
		build_tree();
		//errors
		HCUBE_RENDER_PRINT_ERRORS
	}


	void lod_terrain::node::build(const lod_terrain::node::build_info& info)
	{
#if 0
		std::cout << "--------------------------------------------------------------\n";
		std::cout << "g_size: " << info.m_g_size.x << ", " << info.m_g_size.y << "\n";
		std::cout << "start: "  << info.m_start.x  << ", " << info.m_start.y << "\n";
		std::cout << "end: "    << info.m_end.x << ", " << info.m_end.y << "\n";
		std::cout << "stride: " << info.m_stride.x << ", " << info.m_stride.y << "\n";
#endif
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
		//mapping tris
		size_t n_quad = (s_size.x) * (s_size.y);
		std::vector< unsigned int > idxs(n_quad * 6);
		{
			unsigned int i = 0;
			for (unsigned int y = 0; y < s_size.y; ++y)
			for (unsigned int x = 0; x < s_size.x; ++x)
			{
				const ivec2 g_coord = start + ivec2(x,y) * stride;
				// top
				const unsigned int point  = (g_coord.y * g_size.x + g_coord.x);
				const unsigned int point1 = point + (1 * stride.x);
				// bottom
				const unsigned int point2 = ((g_coord.y + stride.y) * g_size.x + g_coord.x);
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
