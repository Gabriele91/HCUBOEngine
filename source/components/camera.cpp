//
//  camera.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <hcube/core/entity.h>
#include <hcube/math/vector_math.h>
#include <hcube/components/camera.h>
#include <hcube/components/transform.h>

namespace hcube
{
	HCUBE_COMPONENT_REGISTER(camera)

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
	
	const frustum& camera::get_frustum() const
	{
		return m_frustum;
	}

	const frustum& camera::update_frustum()
	{
		m_frustum.update_frustum(m_projection*get_view());
		return m_frustum;
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


	void uniform_camera::get_uniform(shader::ptr shader)
	{
		m_uniform_viewport   = shader->get_uniform("camera.m_viewport");
		m_uniform_projection = shader->get_uniform("camera.m_projection");
		m_uniform_view       = shader->get_uniform("camera.m_view");
		m_uniform_model      = shader->get_uniform("camera.m_model");
		m_uniform_position   = shader->get_uniform("camera.m_position");
		m_valid =
			   m_uniform_viewport
			|| m_uniform_projection
			|| m_uniform_view
			|| m_uniform_model
			|| m_uniform_position;
	}

	void uniform_camera::uniform(camera::wptr ref_camera, const mat4& model) const
	{
		auto c_camera = ref_camera.lock();
		if (m_uniform_viewport)     m_uniform_viewport->set_value(c_camera->get_viewport());
		if (m_uniform_projection)   m_uniform_projection->set_value(c_camera->get_projection());
		if (m_uniform_view)         m_uniform_view->set_value(c_camera->get_view());
		if (m_uniform_model)        m_uniform_model->set_value(model);
		if (m_uniform_position)     m_uniform_position->set_value(vec3(model[3]));
	}

	bool uniform_camera::is_valid() const
	{
		return m_valid;
	}

}