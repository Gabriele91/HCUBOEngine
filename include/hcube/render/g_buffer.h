//
//  g_buffer.h
//  HCubo
//
//  Created by Gabriele on 19/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <hcube/config.h>
#include <hcube/math/vector_math.h>
#include <hcube/render/render.h>

namespace hcube
{
	class HCUBE_API g_buffer
	{
	public:

		enum G_BUFFER_TEXTURE_TYPE
		{
			G_BUFFER_TEXTURE_TYPE_POSITION,
			G_BUFFER_TEXTURE_TYPE_NORMAL,
			G_BUFFER_TEXTURE_TYPE_ALBEDO,
			G_BUFFER_TEXTURE_TYPE_LIGHTS_ACCUMULATOR,
			G_BUFFER_TEXTURE_TYPE_DEPTH,
			G_BUFFER_NUM_TEXTURES
		};

		g_buffer() {}

		virtual ~g_buffer() { destoy(); }

		bool init(const ivec2& window_size);

		bool init(unsigned int width, unsigned int height);

		void destoy();
		
		context_render_target* get_geometry_render_target() const;

		context_render_target* get_lights_render_target() const;
		
		context_texture* get_texture(G_BUFFER_TEXTURE_TYPE texture_type) const;

		unsigned int get_width() const;

		unsigned int get_height() const;

		ivec2 get_size() const;

	private:

		unsigned int m_width{ 0 };
		unsigned int m_height{ 0 };

		context_render_target* m_geometry_target{ nullptr };
		context_render_target* m_lights_target{ nullptr };
		context_texture*       m_textures[G_BUFFER_NUM_TEXTURES]{ nullptr };
	};
}
