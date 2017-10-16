//
//  text_mesh.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <vector>
#include <hcube/render/render.h>
#include <hcube/components/text_mesh.h>
#include <hcube/utilities/utf_convert.h>
#include <algorithm>

namespace hcube
{
	HCUBE_COMPONENT_REGISTER(text_mesh)

	text_mesh::text_mesh(size_t text_max_size)
	{
		//set max size
		m_text_max_size = text_max_size;
		//gen buffer
		m_bpoints = render::create_stream_VBO(nullptr, sizeof(int), text_max_size);
		//create input layout
		m_layout = render::create_IL({
			attribute{ATT_POSITIONT,AST_INT,0}
		});
	}
    
    text_mesh::text_mesh(const std::string& text, size_t text_max_size)
    : text_mesh(text_max_size)
    {
        set_text(text);
    }

	text_mesh::text_mesh(const std::u32string& text, size_t text_max_size)
	: text_mesh(text_max_size)
	{
		set_text(text);
	}


	text_mesh::~text_mesh()
	{
		render::delete_VBO(m_bpoints);
		render::delete_IL(m_layout);
	}

	void text_mesh::update_mesh()
	{
		///get point
		int* points = (int*)render::map_VBO(m_bpoints, 0, m_text_max_size * sizeof(int), MAP_WRITE);
		// stream data
		for (size_t i = 0; i != m_text_max_size && i != m_text.size(); ++i)
		{
			points[i] = (int)m_text[i];
		}
		// unmap buffer
		render::unmap_VBO(m_bpoints);
	}

	void text_mesh::set_text(const std::u32string& text)
	{
		if (m_text != text)
		{
			m_text = text;
			update_mesh();
		}
	}

	void text_mesh::set_text(const std::string& text)
	{
		set_text( to_utf32(text) );
	}

	void text_mesh::draw(rendering_system& rsystem, entity::ptr camera)
	{
		if (m_text.size())
		{
			if (context_shader* shader = render::get_bind_shader())
			{
				//view
				vec2 view_size = vec2(render::get_viewport_state().m_viewport.z - render::get_viewport_state().m_viewport.x,
							 	      render::get_viewport_state().m_viewport.w - render::get_viewport_state().m_viewport.y);
				//get params
				vec2 size, origin;
				//get
				if (material_ptr material = get_material())
				{
					if (auto* param = material->get_parameter_by_name("render_size"))
						size = param->get_vec2();
					if (auto* param = material->get_parameter_by_name("render_origin"))
						origin = param->get_vec2();
				}
				//uniform
				if (context_uniform* render_size = render::get_uniform(shader, std::string("render_size")))
					render_size->set_value(size / view_size);
				if (context_uniform* render_origin = render::get_uniform(shader, std::string("render_origin")))
					render_origin->set_value(
						((size + origin) * vec2(1.0,-1.0)) / view_size - vec2(1.0f,-1.0f)
					);
			}
			render::bind_VBO(m_bpoints);
			render::bind_IL(m_layout);
			render::draw_arrays(DRAW_POINTS, (unsigned int)std::min(m_text.size(), m_text_max_size));
			render::unbind_IL(m_layout);
			render::unbind_VBO(m_bpoints);
		}

	}

	void text_mesh::set_text_max_size(size_t text_max_size)
	{
		//set max size
		m_text_max_size = text_max_size;
		//del buffer
		render::delete_VBO(m_bpoints);
		//gen buffer
		m_bpoints = render::create_stream_VBO(nullptr, sizeof(int), text_max_size);
		//update buffer
		update_mesh();
	}
	
	component_ptr text_mesh::copy() const
	{
		return text_mesh::snew(m_text,m_text_max_size);
	}


	size_t text_mesh::get_text_max_size() const
	{
		return m_text_max_size;
	}

	std::u32string text_mesh::get_text() const
	{
		return m_text;
	}

	std::string text_mesh::get_text_utf8() const
	{
		return to_utf8(m_text);
	}
}
