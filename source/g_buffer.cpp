//
//  g_buffer.cpp
//  HCubo
//
//  Created by Gabriele on 19/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <iostream>
#include <string>
#include <g_buffer.h>

namespace hcube
{
	//texture buffer type
	struct target_texture_type
	{
		texture_format       m_format = TF_RGBA32F;
		texture_type         m_type = TT_RGBA;
		texture_type_format  m_type_format = TTF_FLOAT;

		target_texture_type() {}

		target_texture_type(texture_format      type_0,
			texture_type        type_1,
			texture_type_format type_2)
		{
			m_format = type_0;
			m_type = type_1;
			m_type_format = type_2;
		}
	};

	bool g_buffer::init(const ivec2& window_size)
	{
		return init(window_size.x, window_size.y);
	}

	bool g_buffer::init(unsigned int width, unsigned int height)
	{
		//save size
		m_width = width;
		m_height = height;

		//types
		target_texture_type types[G_BUFFER_NUM_TEXTURES];

		//specify type
		types[G_BUFFER_TEXTURE_TYPE_POSITION] = target_texture_type(TF_RGBA16F, TT_RGBA, TTF_FLOAT);
		types[G_BUFFER_TEXTURE_TYPE_NORMAL] = target_texture_type(TF_RGB8, TT_RGB, TTF_FLOAT);
		types[G_BUFFER_TEXTURE_TYPE_ALBEDO] = target_texture_type(TF_RGBA8, TT_RGBA, TTF_UNSIGNED_BYTE);


		//create texture
		for (unsigned int i = 0; i != G_BUFFER_NUM_TEXTURES; i++)
		{
			m_textures[i] =
			render::create_texture
			(
				{ 
					types[i].m_format,
					width,
					height,
					nullptr,
					types[i].m_type,
					types[i].m_type_format 
				},
				{
					TMIN_NEAREST,
					TMAG_NEAREST,
					TEDGE_CLAMP,
					TEDGE_CLAMP,
					false 
				}
			);
		}
		//depth
		m_depth_texture =
		render::create_texture
		(
			{
				TF_DEPTH_COMPONENT32,
				width,
				height,
				nullptr,
				TT_DEPTH,
				TTF_FLOAT 
			},
			{
				TMIN_NEAREST,
				TMAG_NEAREST,
				TEDGE_CLAMP,
				TEDGE_CLAMP,
				false
			}
		);

		//rander target
		m_target =
		render::create_render_target
		({
				target_field{ m_textures[G_BUFFER_TEXTURE_TYPE_POSITION], RT_COLOR },
				target_field{ m_textures[G_BUFFER_TEXTURE_TYPE_NORMAL],   RT_COLOR },
				target_field{ m_textures[G_BUFFER_TEXTURE_TYPE_ALBEDO],   RT_COLOR },
				target_field{ m_depth_texture,                            RT_DEPTH },
		});

		//success?
		return m_target != nullptr;
	}

	void g_buffer::destoy()
	{
		if (m_target) render::delete_render_target(m_target);

		for (unsigned int i = 0; i != G_BUFFER_NUM_TEXTURES; i++)
		{
			if (m_textures[i]) render::delete_texture(m_textures[i]);
		}
		if (m_depth_texture) render::delete_texture(m_depth_texture);
		//to null
		m_width = 0;
		m_height = 0;

	}

	void g_buffer::bind_for_writing()
	{
		render::enable_render_target(m_target);
		assert(0);
	}

	void g_buffer::bind_for_reading()
	{
		render::enable_render_target(m_target);
		assert(0);
	}

	void g_buffer::bind()
	{
		render::enable_render_target(m_target);
	}

	void g_buffer::unbind()
	{
		render::disable_render_target(m_target);
	}


	void g_buffer::set_read_buffer(G_BUFFER_TEXTURE_TYPE texture_type)
	{
		//glReadBuffer(GL_COLOR_ATTACHMENT0 + texture_type);
		assert(0);
	}

	void g_buffer::set_texture_buffer(G_BUFFER_TEXTURE_TYPE texture_type)
	{
		switch (texture_type)
		{
		case G_BUFFER_TEXTURE_TYPE_POSITION:
			render::bind_texture(m_textures[G_BUFFER_TEXTURE_TYPE_POSITION], 0);
			break;

		case G_BUFFER_TEXTURE_TYPE_NORMAL:
			render::bind_texture(m_textures[G_BUFFER_TEXTURE_TYPE_NORMAL], 1);
			break;

		case G_BUFFER_TEXTURE_TYPE_ALBEDO:
			render::bind_texture(m_textures[G_BUFFER_TEXTURE_TYPE_ALBEDO], 2);
			break;

		case g_buffer::G_BUFFER_NUM_TEXTURES:
		default:
			assert(0);
			break;
		}

	}

	void g_buffer::disable_texture(G_BUFFER_TEXTURE_TYPE texture_type)
	{

		switch (texture_type)
		{
		case G_BUFFER_TEXTURE_TYPE_POSITION:
			render::unbind_texture(m_textures[G_BUFFER_TEXTURE_TYPE_POSITION]);
			break;

		case G_BUFFER_TEXTURE_TYPE_NORMAL:
			render::unbind_texture(m_textures[G_BUFFER_TEXTURE_TYPE_NORMAL]);
			break;

		case G_BUFFER_TEXTURE_TYPE_ALBEDO:
			render::unbind_texture(m_textures[G_BUFFER_TEXTURE_TYPE_ALBEDO]);
			break;

		case g_buffer::G_BUFFER_NUM_TEXTURES:
		default:
			assert(0);
			break;
		}

	}

	context_texture* g_buffer::get_texture(G_BUFFER_TEXTURE_TYPE texture_type) const
	{
		switch (texture_type)
		{
		case G_BUFFER_TEXTURE_TYPE_POSITION: return  m_textures[G_BUFFER_TEXTURE_TYPE_POSITION];
		case G_BUFFER_TEXTURE_TYPE_NORMAL: return  m_textures[G_BUFFER_TEXTURE_TYPE_NORMAL];
		case G_BUFFER_TEXTURE_TYPE_ALBEDO: return m_textures[G_BUFFER_TEXTURE_TYPE_ALBEDO];
		default: return nullptr;
		}
	}

	void g_buffer::set_read_buffer_depth()
	{
		//glReadBuffer(GL_DEPTH_ATTACHMENT);
		assert(0);
	}

	void g_buffer::set_texture_buffer_depth(size_t n_texture)
	{
		render::bind_texture(m_depth_texture, (int)n_texture);
	}

	void g_buffer::disable_depth_texture()
	{
		render::unbind_texture(m_depth_texture);
	}

	context_texture* g_buffer::get_texture_buffer_depth() const
	{
		return m_depth_texture;
	}

	unsigned int g_buffer::get_width() const
	{
		return m_width;
	}

	unsigned int g_buffer::get_height() const
	{
		return m_height;
	}

	ivec2 g_buffer::get_size() const
	{
		return ivec2{ m_width,m_height };
	}

}