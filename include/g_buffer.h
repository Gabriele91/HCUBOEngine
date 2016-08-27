//
//  g_buffer.h
//  HCubo
//
//  Created by Gabriele on 19/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <render.h>
#include <vector_math.h>

namespace hcube
{
	class g_buffer
	{
	public:

		enum G_BUFFER_TEXTURE_TYPE
		{
			G_BUFFER_TEXTURE_TYPE_POSITION,
			G_BUFFER_TEXTURE_TYPE_NORMAL,
			G_BUFFER_TEXTURE_TYPE_ALBEDO,
			G_BUFFER_NUM_TEXTURES
		};

		g_buffer() {}

		virtual ~g_buffer() { destoy(); }

		bool init(const ivec2& window_size);

		bool init(unsigned int width, unsigned int height);

		void destoy();

		void bind_for_writing();

		void bind();

		void unbind();

		void bind_for_reading();

		void set_read_buffer(G_BUFFER_TEXTURE_TYPE texture_type);

		void set_texture_buffer(G_BUFFER_TEXTURE_TYPE texture_type);

		void disable_texture(G_BUFFER_TEXTURE_TYPE texture_type);

		context_texture* get_texture(G_BUFFER_TEXTURE_TYPE texture_type) const;

		void set_read_buffer_depth();

		void set_texture_buffer_depth(size_t n_texture = 0);

		context_texture* get_texture_buffer_depth() const;

		void disable_depth_texture();

		unsigned int get_width() const;

		unsigned int get_height() const;

		ivec2 get_size() const;

	private:

		unsigned int m_width{ 0 };
		unsigned int m_height{ 0 };

		context_render_target* m_target{ nullptr };
		context_texture*       m_textures[G_BUFFER_NUM_TEXTURES]{ nullptr };
		context_texture*       m_depth_texture{ nullptr };
	};
}
