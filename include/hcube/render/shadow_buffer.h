//
//  shadow_buffer.h
//  HCubo
//
//  Created by Gabriele on 19/08/25.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <hcube/math/vector_math.h>
#include <hcube/render/render.h>

namespace hcube
{
	class shadow_buffer
	{

	public:
		
		shadow_buffer() {}

		virtual ~shadow_buffer() { destoy(); }

		bool init(const ivec2& shadow_size,bool is_cube_map = false);

		void destoy();

		void bind() const;

		void unbind() const;

		void set_texture_buffer_depth(size_t n_texture = 0) const;

		void disable_depth_texture() const;

		context_texture* get_depth_texture() const;

		ivec2 get_size() const;

	protected:

		unsigned int m_width{ 0 };
		unsigned int m_height{ 0 };

		context_render_target* m_target{ nullptr };
		context_texture*       m_depth_texture{ nullptr };

	};
}