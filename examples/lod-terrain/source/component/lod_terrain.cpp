#include <iostream>
#include <component/lod_terrain.h>
#include <hcube/geometries/intersection.h>
#include <hcube/math/tangent_space_calculation.h>
#include <hcube/render/rendering_system.h>

namespace hcube
{
	//vertex data description CPU
	struct alignas(16) terrain_vertex
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
	void lod_terrain::node::set(const lod_terrain::node::build_info& info)
	{
#if 0
		std::cout << "--------------------------------------------------------------\n";
		std::cout << "start: " << info.m_start.x << ", " << info.m_start.y << "\n";
		std::cout << "end: " << info.m_end.x << ", " << info.m_end.y << "\n";
#endif
		//save
		m_info = info;
		//ref
		const vec2& _start  = info.m_start;
		const vec2& _end    = info.m_end;
		//compute box size
		vec3
		box_size(_end.x - _start.x, 1.0, _end.y - _start.y);
		box_size /= 2.0f;
		//compute box position
		vec3
		box_pos((_start.x + _end.x)*0.5,
				0.0,
				(_start.y + _end.y)*0.5);
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
		render::create_IL
		({
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
		std::vector< unsigned int >   indices(m_detail.x * m_detail.y * 6);
		std::vector< terrain_vertex > vertices(m_detail_vertexs.x*m_detail_vertexs.y);
		//init index
		int i = 0;
		//#pragma omp parallel for num_threads(4)
		for (int y = 0; y < m_detail.y; ++y)
		for (int x = 0; x < m_detail.x; ++x)
		{
			// top
			const unsigned int point = y * m_detail_vertexs.x + x;
			const unsigned int point1 = point + 1;
			// bottom
			const unsigned int point2 = (y + 1) * m_detail_vertexs.x + x;
			const unsigned int point3 = point2 + 1;
			//add tri 1
			indices[i++] = point;
			indices[i++] = point3;
			indices[i++] = point1;
			//add tri 2
			indices[i++] = point;
			indices[i++] = point2;
			indices[i++] = point3;
		}
		//compute tangents
		//tangent_space_calculation::compute_tangent_fast<terrain_vertex>(vertices,indices);
		//set obb
		set_bounding_box(obb(mat3(1), vec3(0, 0, 0), vec3(0.5, 0.5, 0.5)));
		//save sizes
		m_vb_size = vertices.size();
		m_ib_size = indices.size();
		//alloc vertex buffer
		m_vbuffer = render::create_stream_VBO((unsigned char*)vertices.data(), sizeof(terrain_vertex), vertices.size());
		//alloc index buffer
		m_ibuffer = render::create_stream_IBO(indices.data(), indices.size());
		//errors
		HCUBE_RENDER_PRINT_ERRORS
		//build tree
		build_tree();
		//errors
		HCUBE_RENDER_PRINT_ERRORS
	}

	void lod_terrain::build_tree()
	{
		//alloc all childs
		size_t n_nodes = std::pow(4, m_levels+1) + 1;
		m_nodes.resize(n_nodes);
		//compute root 
		m_nodes[0].set({ vec2(0.0,0.0), vec2(1.0,1.0), m_detail });
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
		//safe: 
		if (level == m_levels) return;
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
		//compute local size
		auto& p_info  = parent->m_info;
		vec2 p_g_size = p_info.m_end - p_info.m_start;
		vec2 l_size   = p_g_size / 2.0f;
		//coord
		{
			vec2 start = p_info.m_start;
			vec2 end = start + l_size;
			parent->m_chils[NODE_CHILD_TOP_LEFT]->set({ start, end, m_detail });
		}
		{
			vec2 start = p_info.m_start + vec2(p_g_size.x / 2.0f, 0);
			vec2 end = start + l_size;
			parent->m_chils[NODE_CHILD_TOP_RIGHT]->set({ start, end, m_detail });
		}
		{
			vec2 start = p_info.m_start + vec2(0, p_g_size.y / 2.0f);
			vec2 end = start + l_size;
			parent->m_chils[NODE_CHILD_BOTTOM_LEFT]->set({ start, end, m_detail });
		}
		{
			vec2 start = p_info.m_start + p_g_size / 2.0f;
			vec2 end = start + l_size;
			parent->m_chils[NODE_CHILD_BOTTOM_RIGHT]->set({ start, end, m_detail });
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
			int x = normx * (diff_texture->get_width() - 1);
			int y = normy * (diff_texture->get_height() - 1);
			int pixel = (y * diff_texture->get_width() + x) * channels;
			return float(raw_image[pixel]) / 255.;
		};
		//map h 
		float map_height = 0.01;
		//alloc
		m_hmap_width = diff_texture->get_width();
		m_hmap_height = diff_texture->get_height();
		m_hmap.resize(m_hmap_width * m_hmap_height);
		//for all
		//#pragma omp parallel for num_threads(4)
		for (int y = 0; y < diff_texture->get_height(); ++y)
		for (int x = 0; x < diff_texture->get_width();  ++x)
		{
			int pixel_pos	 = (y * diff_texture->get_width() + x);
			m_hmap[pixel_pos]= float(raw_image[pixel_pos*channels]) / 255.0f - 0.5f;
			//max
			map_height = std::max(map_height, std::abs(m_hmap[pixel_pos]));
		}
		//set obb
		set_bounding_box(obb(mat3(1), vec3(0, 0, 0), vec3(0.5, map_height, 0.5)));
	}

	#pragma endregion

	//terrain
	#pragma region terrain
	lod_terrain::lod_terrain(vec2 detail, unsigned int levels)
	{
		m_detail		 = detail;
		m_detail_vertexs = detail + vec2(1,1);
		m_levels		 = levels;
		build();
		set_support_culling(true);
	}
	
	lod_terrain::~lod_terrain()
	{
		if (m_layout)
		{
			render::delete_IL(m_layout);
		}
		if (m_ibuffer)
		{
			render::delete_IBO(m_ibuffer);
		}
		if (m_vbuffer)
		{
			render::delete_VBO(m_vbuffer);
		}
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
		//all no draw
		for (node& thiz_node : m_nodes) thiz_node.m_state = NODE_NOT_DRAW;
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
		//this model
		mat4 m4_model = get_entity()->get_component<transform>()->get_matrix();
		//draw
		compute_object_to_draw
		(
			&m_nodes[0], 
			std::pow(get_bounding_box().volume(), 1.0f/3.f) * (2.0f / float(m_levels)),
			t_camera->get_global_position(), 
			c_camera->get_frustum(),
			m4_model
		);
#endif 
#if 1
		HCUBE_RENDER_PRINT_ERRORS
		//draw
		for (node& thiz_node : m_nodes)
		{
			if (thiz_node.m_state == NODE_DRAW)
			{
				terrain_vertex* vecs = (terrain_vertex*)render::map_VBO(m_vbuffer, 0, m_vb_size * sizeof(terrain_vertex), MAP_WRITE);
				//uv range in terrain
				vec2  uv_area = thiz_node.m_info.m_end - thiz_node.m_info.m_start;
				vec2  uv_step = uv_area / (vec2(m_detail_vertexs) - vec2(1, 1));
				//mapping
				#pragma omp parallel for num_threads(4)
				for (int y = 0; y < m_detail_vertexs.y; ++y)
				for (int x = 0; x < m_detail_vertexs.x; ++x)
				{
					//vertex id
					int vid = y * m_detail_vertexs.x + x;
					assert(vid < m_vb_size);
					//current uv
					vec2 uv_coord(thiz_node.m_info.m_start + uv_step * vec2(x,y));
					//get height
					float height = get_height(uv_coord.x, uv_coord.y);
					//pos
					vec3 pos
					{
						  uv_coord.x - 0.5
						, height
						, uv_coord.y - 0.5
					};
					//add new vertex
					vecs[vid] = terrain_vertex
					(
						pos,
						vec3(0, 1, 0),
						uv_coord
					);
				}
				render::unmap_VBO(m_vbuffer);
				HCUBE_RENDER_PRINT_ERRORS
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////
				render::bind_VBO(m_vbuffer);
				render::bind_IBO(m_ibuffer);
				render::bind_IL(m_layout);
				//draw
				render::draw_elements(DRAW_TRIANGLES, m_ib_size);
				//unbind
				render::unbind_IL(m_layout);
				render::unbind_IBO(m_ibuffer);
				render::unbind_VBO(m_vbuffer);
				HCUBE_RENDER_PRINT_ERRORS
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}
		HCUBE_RENDER_PRINT_ERRORS
#endif
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
