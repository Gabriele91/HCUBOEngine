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
		types[G_BUFFER_TEXTURE_TYPE_POSITION]           = target_texture_type(TF_RGBA16F,            TT_RGBA, TTF_FLOAT);
		types[G_BUFFER_TEXTURE_TYPE_NORMAL]		   	    = target_texture_type(TF_RGB8,               TT_RGB,  TTF_FLOAT);
		types[G_BUFFER_TEXTURE_TYPE_ALBEDO]				= target_texture_type(TF_RGBA8,              TT_RGBA, TTF_UNSIGNED_BYTE);
		types[G_BUFFER_TEXTURE_TYPE_LIGHTS_ACCUMULATOR] = target_texture_type(TF_RGB8,               TT_RGB,  TTF_UNSIGNED_BYTE);
		types[G_BUFFER_TEXTURE_TYPE_DEPTH]              = target_texture_type(TF_DEPTH_COMPONENT24,  TT_DEPTH,TTF_FLOAT);
		

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
		//rander target
		m_geometry_target =
		render::create_render_target
		({
				target_field{ m_textures[G_BUFFER_TEXTURE_TYPE_POSITION], RT_COLOR },
				target_field{ m_textures[G_BUFFER_TEXTURE_TYPE_NORMAL],   RT_COLOR },
				target_field{ m_textures[G_BUFFER_TEXTURE_TYPE_ALBEDO],   RT_COLOR },
				target_field{ m_textures[G_BUFFER_TEXTURE_TYPE_DEPTH],    RT_DEPTH },
		});
		m_lights_target =
		render::create_render_target
		({
			target_field{ m_textures[G_BUFFER_TEXTURE_TYPE_LIGHTS_ACCUMULATOR], RT_COLOR },
			target_field{ m_textures[G_BUFFER_TEXTURE_TYPE_DEPTH],              RT_DEPTH },
		});

		//success?
		return m_geometry_target != nullptr && m_lights_target != nullptr;
	}

	void g_buffer::destoy()
	{
		if (m_geometry_target) render::delete_render_target(m_geometry_target);
		if (m_lights_target)   render::delete_render_target(m_lights_target);

		for (unsigned int i = 0; i != G_BUFFER_NUM_TEXTURES; i++)
		{
			if (m_textures[i]) render::delete_texture(m_textures[i]);
		}
		//to null
		m_width = 0;
		m_height = 0;

	}

	context_render_target* g_buffer::get_geometry_render_target() const
	{
		return m_geometry_target;
	}

	context_render_target* g_buffer::get_lights_render_target() const
	{
		return m_lights_target;
	}

	context_texture* g_buffer::get_texture(G_BUFFER_TEXTURE_TYPE texture_type) const
	{
		return m_textures[texture_type];
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