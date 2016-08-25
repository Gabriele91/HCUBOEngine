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
		m_projection     = perspective(fov, aspect, in_near, in_far);
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
		//test entity
		if (get_entity())
		{
			//test transform
			auto t_camera = get_entity()->get_component<transform>();
			//update
			if (t_camera)
			{
				m_frustum.update_frustum(m_projection*t_camera->get_matrix());
				return;
			}
		}
		//else
		m_frustum.update_frustum(m_projection);
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