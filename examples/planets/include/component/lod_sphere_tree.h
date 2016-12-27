#pragma once
#include <hcube/render/render.h>
#include <hcube/components/renderable.h>
namespace hcube
{

	class load_sphere_tree : public smart_pointers< load_sphere_tree >, public renderable
	{

		//HCUBE_COMPONENT_DEC(load_sphere_tree)

	public:

		load_sphere_tree
		(
			  float range = 1.0
			, unsigned int factor = 4
			, unsigned int levels = 4
		);
		
		void build();


		//overload renderable::draw
		void draw(rendering_system& rsystem, entity::ptr view) override;

		//overload component::copy 
		virtual component_ptr copy() const;

		//serialize
#if 0
		HCUBE_EXTENDS_PROPERTIES(
			//EXTENDS
			renderable,
			//PROPERTIES
			make_property_member(&m_range, "range"),
			make_property_member(&m_start_level, "factor"),
			make_property_member(&m_end_level,"levels")
		)
#endif
	private:
		//target
		vec3 m_target{ 0,0,0 };
		//info
		float		 m_range;
		unsigned int m_factor;
		unsigned int m_levels;
		//draw info
		context_input_layout*  m_layout{ nullptr };
		context_index_buffer*  m_ibuffer{ nullptr };
		context_vertex_buffer* m_vbuffer{ nullptr };

	};

}