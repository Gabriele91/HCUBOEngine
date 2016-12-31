#pragma once

#include <hcube/render/render.h>
#include <hcube/components/renderable.h>

namespace hcube
{
	class lod_terrain : public smart_pointers< lod_terrain >, public renderable
	{

		//HCUBE_COMPONENT_DEC(lod_terrain)
		enum node_nears
		{
			NODE_TOP,
			NODE_RIGHT,
			NODE_BOTTOM,
			NODE_LEFT
		};
		enum node_child
		{
			NODE_CHILD_TOP_LEFT,
			NODE_CHILD_TOP_RIGHT,
			NODE_CHILD_BOTTOM_LEFT,
			NODE_CHILD_BOTTOM_RIGHT,
			NODE_CHILD_MAX
		};

		enum node_state
		{
			NODE_NOT_DRAW,
			NODE_DRAW,
			NODE_DRAW_CHILD
		};

		struct node : public smart_pointers< node >
		{
			struct build_info
			{
				vec2 m_start;
				vec2 m_end;
				vec2 m_detail;
			};
			//state
			node_state m_state{ NODE_NOT_DRAW };
			build_info m_info;
			obb		   m_box;
			//info
			void set(const build_info& build_info);
			//parent 
			node*  m_parent{ nullptr };
			//childs
			node* m_chils[4]
			{
				  nullptr
				, nullptr
				, nullptr
				, nullptr
			};
			//neighbors
			node* m_neighbors[4]
			{
				  nullptr
				, nullptr
				, nullptr
				, nullptr
			};
		};

		//////////////////////////////////////////////////////////////////////////////////////
		void build_tree();
		void build_tree
		(
			node* parent,
			unsigned int& node,
			unsigned int level=0
		);
		//////////////////////////////////////////////////////////////////////////////////////
		void compute_object_to_draw
		(
			node* parent,
			const float camera_factor,
			const vec3& camera_position,
			const frustum& frustum,
			const mat4& model_view,
			unsigned int level = 0
		);
		//////////////////////////////////////////////////////////////////////////////////////
		void compute_object_to_draw_debug
		(
			node* parent, 
			unsigned int level_to_draw = 0, 
			unsigned int level = 0
		);
        
	public:

		//size
		lod_terrain
		(
			vec2 detail = { 16, 16 },
			unsigned int levels = 3
		);
		virtual ~lod_terrain();

		//build tree
		void build();

		//overload renderable::draw
		void draw(rendering_system& rsystem,entity::ptr view) override;

		//overload component::copy 
		virtual component_ptr copy() const
		{
			return nullptr;
		}

        void set_draw_level(unsigned int level=0)
        {
            m_level_to_draw = level;
        }


		virtual void set_material(material_ptr material);


	private:
		//levels
		ivec2		 m_detail;
		ivec2		 m_detail_vertexs;
        unsigned int m_levels;
        unsigned int m_level_to_draw{0};
		//draw info
		unsigned int		   m_ib_size{ 0 };
		unsigned int		   m_vb_size{ 0 };
		context_input_layout*  m_layout { nullptr };
		context_vertex_buffer* m_vbuffer{ nullptr };
		context_index_buffer*  m_ibuffer{ nullptr };
		//pool nodes
		std::vector< node >  m_nodes;
		//hmap
		unsigned int m_hmap_width { 0 };
		unsigned int m_hmap_height{ 0 };
		std::vector< float > m_hmap; 
		//get height from map
		inline float get_height(float normx, float normy) const
		{
			if (!m_hmap.size()) return 0.0;
			int x = normx * (m_hmap_width - 1);
			int y = normy * (m_hmap_height - 1);
			int pixel = (y * m_hmap_width + x);
			return float(m_hmap[pixel]) / 255.;
		}
		//update mesh
		void recompute_mesh_heigth();

	};
}
