//
//  arc_ball.h
//  OGLHCubeView
//
//  Created by Gabriele on 01/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


namespace hcube
{

	struct arc_ball
	{

		void set_window_size(const glm::vec2& size)
		{
			m_window_size = size;
		}

		void start_drag(const glm::vec2& pos)
		{
			m_last_pos = pos;
			m_pos = pos;
		}

		const void drag(const glm::vec2& pos)
		{
			m_pos = pos;
		}

		bool is_change() const
		{
			return (m_pos != m_last_pos);
		}

		glm::mat4 compute_rotation(const glm::mat4& view, const glm::mat4& model)
		{
			if (m_pos != m_last_pos)
			{
				glm::vec3 va = get_arcball_vector(m_last_pos);
				glm::vec3 vb = get_arcball_vector(m_pos);
				float angle = std::acos(std::min(1.0f, glm::dot(va, vb)));
				glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
				glm::mat3 camera2object = glm::inverse(glm::mat3(view) * glm::mat3(model));
				glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
				m_last_pos = m_pos;
				m_last_rotation = glm::rotate(glm::mat4(1), angle, axis_in_object_coord);

			}
			return model*m_last_rotation;
		}

	private:

		//window
		glm::vec2 m_window_size;
		//mouse pos
		glm::vec2 m_pos;
		glm::vec2 m_last_pos;
		//last update
		glm::mat4 m_last_rotation;

		//compute vector
		glm::vec3 get_arcball_vector(const glm::vec2& pos)
		{
			glm::vec3 dir
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