//
//  arc_ball.h
//  OGLHCubeView
//
//  Created by Gabriele on 01/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector_math.h>


namespace hcube
{

	struct arc_ball
	{

		void set_window_size(const vec2& size)
		{
			m_window_size = size;
		}

		void start_drag(const vec2& pos)
		{
			m_last_pos = pos;
			m_pos = pos;
		}

		const void drag(const vec2& pos)
		{
			m_pos = pos;
		}

		bool is_change() const
		{
			return (m_pos != m_last_pos);
		}

		mat4 compute_rotation(const mat4& view, const mat4& model)
		{
			if (m_pos != m_last_pos)
			{
				vec3 va = get_arcball_vector(m_last_pos);
				vec3 vb = get_arcball_vector(m_pos);
				float angle = std::acos(std::min(1.0f, dot(va, vb)));
				vec3 axis_in_camera_coord = cross(va, vb);
				mat3 camera2object = inverse(mat3(view) * mat3(model));
				vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
				m_last_pos = m_pos;
				m_last_rotation = rotate(mat4(1), angle, axis_in_object_coord);

			}
			return model*m_last_rotation;
		}

	private:

		//window
		vec2 m_window_size;
		//mouse pos
		vec2 m_pos;
		vec2 m_last_pos;
		//last update
		mat4 m_last_rotation;

		//compute vector
		vec3 get_arcball_vector(const vec2& pos)
		{
			vec3 dir
			{
				(pos.x / m_window_size.x)*2.0f - 1.0f,
				(pos.y / m_window_size.y)*2.0f - 1.0f,
				0
			};
			//rev y
			dir.y *= -1;
			//len
			float length_pow2 = dir.x * dir.x + dir.y * dir.y;

			if (length_pow2 <= 1.0f)
			{
				// pythagore
				dir.z = std::sqrt(1.0f - length_pow2);
			}
			else
			{
				// nearest point
				float temp_norm = 1.0f / std::sqrt(length_pow2);
				dir.x *= temp_norm;
				dir.y *= temp_norm;
				dir.z = 0.0f;
			}
			//return
			return dir;
		}

	};
}