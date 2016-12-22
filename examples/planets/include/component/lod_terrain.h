#pragma once

#include <hcube/render/render.h>
#include <hcube/components/renderable.h>

namespace hcube
{
	class lod_terrain : public smart_pointers< lod_terrain >, public renderable
	{

		//HCUBE_COMPONENT_DEC(lod_terrain)

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
				ivec2 m_g_size;
				ivec2 m_start;
				ivec2 m_end;
				ivec2 m_stride;
			};
			//state
			node_state m_state;
			build_info m_info;
			//data info node
			size_t				   m_ib_size{ 0 };
			context_index_buffer*  m_ibuffer{ nullptr };
			//init index buffer
			void build(const build_info& build_info);
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

		void build_tree();
		void build_tree(node* parent, unsigned int& node, unsigned int level=0);
		void draw_tree(node* parent, unsigned int level_to_draw = 0, unsigned int level = 0);

	public:

		//size
		lod_terrain(
			ivec2 size = { 1024, 1024 },
			unsigned int levels = 3
		);
		virtual ~lod_terrain();

		//build tree
		void build();

		//overload renderable::draw
		void draw();

		//overload component::copy 
		virtual component_ptr copy() const
		{
			return nullptr;
		}


	private:
		//levels
		ivec2		 m_size;
		unsigned int m_levels;
		//draw info
		context_input_layout*  m_layout { nullptr };
		context_vertex_buffer* m_vbuffer{ nullptr };
		//root
		std::vector< node > m_nodes;

	};
}