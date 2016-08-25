//
//  shadow_buffer.h
//  HCubo
//
//  Created by Gabriele on 19/08/25.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <render.h>
#include <vector_math.h>

namespace hcube
{
	class shadow_buffer
	{

		bool init(const ivec2& shadow_size);

		void destoy();

		void bind();

		void unbind();

		void set_texture_buffer_depth(size_t n_texture = 0);

		void disable_depth_texture();

	protected:

		unsigned int m_width{ 0 };
		unsigned int m_height{ 0 };

		context_render_target* m_target{ nullptr };
		context_texture*       m_depth_texture{ nullptr };

	};
}