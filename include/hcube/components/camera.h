//
//  camera.h
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <hcube/math/vector_math.h>
#include <hcube/core/component.h>
#include <hcube/core/smart_pointers.h>
#include <hcube/geometries/frustum.h>
#include <hcube/render/shader.h>

namespace hcube
{

	class camera : public component, public smart_pointers< camera >
	{

		HCUBE_COMPONENT_DEC(camera)

	public:

		void set_viewport(const ivec4& viewport);
		void set_perspective(float fov, float aspect, float near, float far);
		vec2 get_near_and_far() const;

		const ivec2  get_viewport_size() const;
		const ivec4& get_viewport() const;
		const mat4&  get_projection() const;
		const mat4&  get_projection_inv() const;
		const mat4&  get_view();

		//events
		void on_attach(entity& entity);
		bool on_activate();
		void on_message(const message& message);
		
		const frustum& get_frustum() const;
		const frustum& update_frustum();

		void update_view_matrix();

		virtual component_ptr copy() const;

	protected:

		frustum	 m_frustum;
		ivec4    m_viewport;
		mat4     m_projection;
		mat4     m_projection_inv;
		mat4     m_view;
		bool     m_view_is_dirty{ true };
	};

	struct uniform_camera
	{
		uniform* m_uniform_viewport{ nullptr };
		uniform* m_uniform_projection{ nullptr };
		uniform* m_uniform_view{ nullptr };
		uniform* m_uniform_model{ nullptr };
		uniform* m_uniform_position{ nullptr };

		void get_uniform(shader::ptr shader);

		void uniform(camera::wptr camera,const mat4& model) const;

		bool is_valid() const;

	protected:

		bool m_valid{ false };

	};

}