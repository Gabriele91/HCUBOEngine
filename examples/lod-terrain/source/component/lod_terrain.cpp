#include <iostream>
#include <component/lod_terrain.h>
#include <hcube/geometries/intersection.h>
#include <hcube/math/tangent_space_calculation.h>
#include <hcube/render/rendering_system.h>

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

	//node methos
	#pragma region node
	void lod_terrain::node::build(const lod_terrain::node::build_info& info)
	{
#if 0
		std::cout << "--------------------------------------------------------------\n";
		std::cout << "g_size: " << info.m_g_size.x << ", " << info.m_g_size.y << "\n";
		std::cout << "start: " << info.m_start.x << ", " << info.m_start.y << "\n";
		std::cout << "end: " << info.m_end.x << ", " << info.m_end.y << "\n";
		std::cout << "stride: " << info.m_stride.x << ", " << info.m_stride.y << "\n";
#endif
		//save
		m_info = info;
		//ref
		const ivec2& g_size = info.m_g_size;
		const ivec2& start = info.m_start;
		const ivec2& end = info.m_end;
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
					const ivec2 g_coord = start + ivec2(x, y) * stride;
					// top
					const unsigned int point = (g_coord.y * g_size.x + g_coord.x);
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
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//compute size
		vec3
			box_size(float(l_size.x) / g_size.x,
				2.0,
				float(l_size.y) / g_size.y);
		box_size /= 2.0f;
		//compute position
		vec3
			box_pos(float(start.x + end.x)*0.5 / g_size.x,
				0.0,
				float(start.y + end.y)*0.5 / g_size.y);
		box_pos -= vec3(0.5, 0.0, 0.5);
		//set OBB
		m_box.build_from_attributes
		(
			mat3(1), //rotation
			box_pos, //position
			box_size //extesion
		);
		//////////////////////////////////////////////////////////////////////////////////////////////////
	}
	#pragma endregion

	//build nodes
	#pragma region node_build

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
		m_p_size = m_size + ivec2(1, 1);
		//buffer
		std::vector< terrain_vertex > vertices(m_p_size.x*m_p_size.y);
		//half size
		vec3 half_size = vec3(m_size.x, 0, m_size.y) / 2.f;
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
		m_nodes.resize(n_nodes + 1);
		//build tree
		build_tree();
		//errors
		HCUBE_RENDER_PRINT_ERRORS
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
		for (size_t i = 0; i != NODE_CHILD_MAX; ++i)
		{
			parent->m_chils[i] = &m_nodes[i_node + i]; parent->m_chils[i]->m_parent = parent;
		}
		//index of allocs
		i_node += NODE_CHILD_MAX;
		//links
		parent->m_chils[NODE_CHILD_TOP_LEFT]->m_neighbors[NODE_RIGHT]  = parent->m_chils[NODE_CHILD_TOP_RIGHT];
		parent->m_chils[NODE_CHILD_TOP_LEFT]->m_neighbors[NODE_BOTTOM] = parent->m_chils[NODE_CHILD_BOTTOM_LEFT];
		//
		parent->m_chils[NODE_CHILD_TOP_RIGHT]->m_neighbors[NODE_LEFT]   = parent->m_chils[NODE_CHILD_TOP_LEFT];
		parent->m_chils[NODE_CHILD_TOP_RIGHT]->m_neighbors[NODE_BOTTOM] = parent->m_chils[NODE_CHILD_BOTTOM_RIGHT];
		//
		parent->m_chils[NODE_CHILD_BOTTOM_LEFT]->m_neighbors[NODE_RIGHT] = parent->m_chils[NODE_CHILD_BOTTOM_RIGHT];
		parent->m_chils[NODE_CHILD_BOTTOM_LEFT]->m_neighbors[NODE_TOP]   = parent->m_chils[NODE_CHILD_TOP_LEFT];
		//
		parent->m_chils[NODE_CHILD_BOTTOM_RIGHT]->m_neighbors[NODE_LEFT] = parent->m_chils[NODE_CHILD_BOTTOM_LEFT];
		parent->m_chils[NODE_CHILD_BOTTOM_RIGHT]->m_neighbors[NODE_TOP]  = parent->m_chils[NODE_CHILD_TOP_RIGHT];
		//level
		int  stride = std::pow(4, m_levels - level);
		//compute local size
		auto& p_info = parent->m_info;
		ivec2 p_g_size = p_info.m_end - p_info.m_start;
		ivec2 l_size = p_g_size / 2;
		ivec2 v_stride = { stride, stride };
		//coord
		{
			ivec2 start = p_info.m_start;
			ivec2 end = start + l_size;
			parent->m_chils[NODE_CHILD_TOP_LEFT]->build({ m_p_size, start, end, v_stride });
		}
		{
			ivec2 start = p_info.m_start + ivec2(p_g_size.x / 2, 0);
			ivec2 end = start + l_size;
			parent->m_chils[NODE_CHILD_TOP_RIGHT]->build({ m_p_size, start, end, v_stride });
		}
		{
			ivec2 start = p_info.m_start + ivec2(0, p_g_size.y / 2);
			ivec2 end = start + l_size;
			parent->m_chils[NODE_CHILD_BOTTOM_LEFT]->build({ m_p_size, start, end, v_stride });
		}
		{
			ivec2 start = p_info.m_start + p_g_size / 2;
			ivec2 end = start + l_size;
			parent->m_chils[NODE_CHILD_BOTTOM_RIGHT]->build({ m_p_size, start, end, v_stride });
		}
		//next levels
		++level;
		//call for all childs
		if (level < m_levels)
			for (size_t i = 0; i != NODE_CHILD_MAX; ++i)
				build_tree(parent->m_chils[i], i_node, level);
	}

	void lod_terrain::recompute_mesh_heigth()
	{
		if (!get_material()) return;
		//param
		auto param = get_material()->get_parameter_by_name("height_map");
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

	#pragma endregion

	//terrain
	#pragma region terrain
	lod_terrain::lod_terrain(ivec2 size, unsigned int levels)
	{
		m_size = size;
		m_levels = levels;
		build();
		set_support_culling(true);
	}
	
	lod_terrain::lod_terrain(material_ptr material,ivec2 size, unsigned int levels)
	{
		set_material(material);
		m_size = size;
		m_levels = levels;
		build();
		recompute_mesh_heigth();
		set_support_culling(true);
	}
		
	lod_terrain::~lod_terrain()
	{
		if (m_layout) render::delete_IL(m_layout);
		if (m_vbuffer) render::delete_VBO(m_vbuffer);
	}

	void lod_terrain::set_material(material_ptr material)
	{
		//set
		renderable::set_material(material);
		//update vertex
		recompute_mesh_heigth();
	}
	#pragma endregion
	
	//draw
	#pragma region draw
	//overload renderable::draw
	void lod_terrain::draw(rendering_system& rsystem, entity::ptr view) 
	{
		//bind buffer
		render::bind_VBO(m_vbuffer);
		render::bind_IL(m_layout);
		//set index
#if 0
		compute_object_to_draw(&m_nodes[0], m_level_to_draw, 0);
#else
		//get camera
		entity::ptr e_camera = rsystem.get_current_draw_camera();
		//get components
		auto c_camera = e_camera->get_component<camera>();
		auto t_camera = e_camera->get_component<transform>();
		auto t_this   = get_entity()->get_component<transform>();
		//get matrix
		//mat4 m4_view = c_camera->get_view();
		//this model
		mat4 m4_model = get_entity()->get_component<transform>()->get_matrix();
		//draw
		compute_object_to_draw(
			&m_nodes[0], 
			std::pow(get_bounding_box().volume(), 1.0f/3.f)* 1.25f,
			t_camera->get_global_position(), 
			c_camera->get_frustum(),
			m4_model
		);
#endif 
		for (auto& thiz_node : m_nodes)
		{
			if (thiz_node.m_state == NODE_DRAW)
			{
				//set index
				render::bind_IBO(thiz_node.m_ibuffer);
				//draw
				render::draw_elements(DRAW_TRIANGLES, thiz_node.m_ib_size);
				//unbind
				render::unbind_IBO(thiz_node.m_ibuffer);
			}
		}
	}

	void lod_terrain::compute_object_to_draw
	(
		node* current,
		const float camera_factor,
		const vec3& camera_position,
		const frustum& frustum,
		const mat4& model,
		unsigned int level
	)
	{
		//not draw
		current->m_state = NODE_NOT_DRAW;
		//copy box
		obb this_box = current->m_box;
		//compute box
		this_box.applay(model);
		//event, intersection
		auto intersection_res = intersection::check(frustum, this_box);
		//is inside?
		if (intersection_res != intersection::OUTSIDE)
		{
			//distance
			float distance_factor   = (camera_factor / (level + 1));
			float distance_from_cam = distance(camera_position, this_box.closest_point(camera_position));
			//levels
			if (m_levels == (level+1) || distance_factor < distance_from_cam)
			{
				//draw this
				current->m_state = NODE_DRAW;
			}
			else
			{
				//draw childs
				current->m_state = NODE_DRAW_CHILD;
				//draw
				compute_object_to_draw(current->m_chils[0], camera_factor, camera_position, frustum, model, level + 1);
				compute_object_to_draw(current->m_chils[1], camera_factor, camera_position, frustum, model, level + 1);
				compute_object_to_draw(current->m_chils[2], camera_factor, camera_position, frustum, model, level + 1);
				compute_object_to_draw(current->m_chils[3], camera_factor, camera_position, frustum, model, level + 1);
			}
		}
	}
	#pragma endregion

	#pragma region debug
	void lod_terrain::compute_object_to_draw_debug(node* current, unsigned int level_to_draw, unsigned int level)
	{
		//not draw
		current->m_state = NODE_NOT_DRAW;
		//draw
		if (level_to_draw == level)
		{
			//draw this
			current->m_state = NODE_DRAW;
		}
		else
		{
			compute_object_to_draw_debug(current->m_chils[0], level_to_draw, level + 1);
			compute_object_to_draw_debug(current->m_chils[1], level_to_draw, level + 1);
			compute_object_to_draw_debug(current->m_chils[2], level_to_draw, level + 1);
			compute_object_to_draw_debug(current->m_chils[3], level_to_draw, level + 1);
		}

	}
	#pragma endregion
}
