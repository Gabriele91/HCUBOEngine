//
//  text_mesh.h
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <string>
#include <hcube/config.h>
#include <hcube/core/smart_pointers.h>
#include <hcube/render/render.h>
#include <hcube/components/renderable.h>
#include <hcube/core/component_register.h>
#include <hcube/data/property.h>

namespace hcube
{
	class HCUBE_API text_mesh : public smart_pointers< text_mesh >, public renderable
	{
		//string info
		std::u32string m_text;
		size_t         m_text_max_size;
		//draw info
		context_input_layout*  m_layout{ nullptr };
		context_vertex_buffer* m_bpoints{ nullptr };
		//update
		void           update_mesh();

	public:

		text_mesh(size_t text_max_size = 255); 
		text_mesh(const std::u32string text, size_t text_max_size = 255);
		virtual ~text_mesh();

		void set_text_max_size(size_t max_size);
		void set_text(const std::string& text);
		void set_text(const std::u32string& text);

		size_t get_text_max_size() const;
		std::u32string get_text() const;
		std::string    get_text_utf8() const;
		void draw();

		virtual component_ptr copy() const;

		HCUBE_EXTENDS_PROPERTIES(
			//EXTENDS
			renderable,
			//PROPERTIES
			make_property_const_get_set(
				&text_mesh::get_text_max_size, 
				&text_mesh::set_text_max_size, 
				"text_max_size"
			),
			make_property_function<text_mesh>(
				[](text_mesh* self)-> variant_ref
				{ 
					return self->get_text_utf8(); 
				},
				[](text_mesh* self, variant_ref value) 
				{ 
					self->set_text(value.get<std::string>()); 
				},
				"text"
			)
		)

	};
}