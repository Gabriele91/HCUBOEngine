//
//  camera.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <camera.h>
#include <entity.h>
#include <transform.h>
#include <vector_math.h>

namespace hcube
{
	void camera::set_viewport(const ivec4& viewport)
	{
		m_viewport = viewport;
	}

	void camera::set_perspective(float fov, float aspect, float in_near, float in_far)
	{
		m_projection     = hcube::perspective(fov, aspect, in_near, in_far);
		m_projection_inv = inverse(m_projection);
	}

	vec2 camera::get_near_and_far() const
	{
		float m22 = -m_projection[2][2];
		float m32 = -m_projection[3][2];

		float prj_far = (2.0f*m32) / (2.0f*m22 - 2.0f);
		float prj_near = ((m22 - 1.0f)*prj_far) / (m22 + 1.0);

		return vec2(prj_near, prj_far);
	}

	const ivec2  camera::get_viewport_size() const
	{
		return ivec2{ m_viewport.z - m_viewport.x, m_viewport.w - m_viewport.y };
	}

	const ivec4& camera::get_viewport() const
	{
		return m_viewport;
	}

	const mat4& camera::get_projection() const
	{
		return m_projection;
	}

	const mat4& camera::get_projection_inv() const
	{
		return m_projection_inv;
	}

	const mat4& camera::get_view()
	{
		update_view_matrix();
		return m_view;
	}

	void camera::on_attach(entity& entity)
	{
		m_view_is_dirty = true;
	}

	bool camera::on_activate()
	{
		m_view_is_dirty = true;
		return true;
	}

	void camera::on_message(const message& message)
	{
		if (message.m_id == transform::MSG_DIRTY) m_view_is_dirty = true;
	}

	frustum& camera::get_frustum()
	{
		return m_frustum;
	}

	const frustum& camera::get_frustum() const
	{
		return m_frustum;
	}

	void camera::update_view_frustum()
	{
		m_frustum.update_frustum(m_projection*get_view());
	}

	void camera::update_view_matrix()
	{
		if (!m_view_is_dirty) return;

		entity*       e_camera = get_entity();
		transform_ptr t_camera = nullptr;

		if (!e_camera || !(t_camera = get_entity()->get_component<transform>()))
		{
			m_view = mat4(1);
			m_view_is_dirty = false;
		}

		//get matrix
		vec3 scale = t_camera->get_global_scale();
		mat4 view  = t_camera->get_matrix();
		//remove scale
		view[0][0] /= scale[0];
		view[0][1] /= scale[0];
		view[0][2] /= scale[0];

		view[1][0] /= scale[1];
		view[1][1] /= scale[1];
		view[1][2] /= scale[1];

		view[2][0] /= scale[2];
		view[2][1] /= scale[2];
		view[2][2] /= scale[2];
		//OpenGL LH, z*-1
		view[2] *= -1;
		//M^-1 = V
		m_view = inverse(view);
	}

	component_ptr camera::copy() const
	{
		auto ocamera = camera::snew();
		ocamera->m_viewport = m_viewport;
		ocamera->m_projection = m_projection;
		ocamera->m_projection_inv = m_projection_inv;
		return ocamera;
	}

}