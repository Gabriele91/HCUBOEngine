//
//  picking_rotation.h
//  OGLHCubeView
//
//  Created by Gabriele on 03/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <camera.h>
#include <picking.h>

namespace hcube
{
	class picking_rotation
	{
	public:

		void set_camera(camera::ptr in_camera)
		{
			m_camera = in_camera;
		}

		void start_drag(const glm::vec2& pos)
		{
			auto point_pick = camera_picking(*m_camera.get(), pos);
			m_valid = !point_pick.m_background;
			m_rotation = glm::quat();
			m_last_to =
				m_to =
				m_start = point_pick;
		}

		void drag(const glm::vec2& pos)
		{
			if (m_valid)
			{
				//pick back
				auto point_pick = camera_picking(*m_camera.get(), pos);
				//if not backgound
				if (!point_pick.m_background) m_to = point_pick;
			}
		}

		bool is_change() const
		{
			return (m_to != m_last_to);
		}

		glm::quat get_rotation(const glm::mat4& model_start,
			const glm::mat4& model,
			double delta_time = 1.0,
			double lerp_factor = 0.025)
		{
			if (is_change())
			{
				//save last compute vector
				m_last_to = m_to;
				//pos by last rotation
				glm::dmat4 model_inv = model_start*glm::inverse(model);
				glm::dvec3 to_relative = (glm::dvec3)(model_inv * glm::vec4(m_to.x, m_to.y, m_to.z, 0.0));
				glm::dvec3 start_relative = (glm::dvec3)(model_inv * glm::vec4(m_start.x, m_start.y, m_start.z, 0.0));
				//..
				m_rotation = rotation_between_vectors(start_relative, to_relative);
				m_rotation *= (glm::dquat)(model_start);

			}
			return (glm::quat)rotate_to_wards((glm::dquat)model, m_rotation, delta_time*lerp_factor);
		}

	public:

		//utils
		glm::dquat rotation_between_vectors(const glm::dvec3& from, const glm::dvec3& to)
		{
			glm::dvec3 start = glm::normalize(from);
			glm::dvec3 dest = glm::normalize(to);

			float cos_theta = glm::dot(start, dest);
			glm::dvec3 rotation_axis;

			if (cos_theta < (-1.0f + 0.001f))
			{
				// special case when vectors in opposite directions:
				// there is no "ideal" rotation axis
				// So guess one; any will do as long as it's perpendicular to start
				rotation_axis = glm::cross(glm::dvec3(0.0f, 0.0f, 1.0f), start);
				if (glm::length2(rotation_axis) < 0.01)
				{
					// bad luck, they were parallel, try again!
					rotation_axis = glm::cross(glm::dvec3(1.0f, 0.0f, 0.0f), start);
				}

				rotation_axis = glm::normalize(rotation_axis);
				return glm::angleAxis(180.0, rotation_axis);
			}

			rotation_axis = glm::cross(start, dest);

			float s = std::sqrt((1.0f + cos_theta)*2.0f);
			float invs = 1.0f / s;


			return glm::dquat(
				s * 0.5f,
				rotation_axis.x * invs,
				rotation_axis.y * invs,
				rotation_axis.z * invs
			);

		}
		//slerp
		glm::dquat rotate_to_wards(glm::dquat q1, glm::dquat q2, float max_angle)
		{

			if (max_angle < 0.001f) return q1;

			double cos_theta = glm::dot(q1, q2);

			// q1 and q2 are already equal.
			// Force q2 just to be sure
			if (cos_theta > 0.9999f) return q2;

			// Avoid taking the long path around the sphere
			if (cos_theta < 0)
			{
				q1 = q1*-1.0;
				cos_theta *= -1.0;
			}

			double angle = std::acos(cos_theta);

			// If there is only a 2∞ difference, and we are allowed 5∞,
			// then we arrived.
			if (angle < max_angle) return q2;

			// This is just like slerp(), but with a custom t
			double t = max_angle / angle;
			angle = max_angle;

			glm::quat res = (std::sin((1.0f - t) * angle) * q1 + std::sin(t * angle) * q2) / std::sin(angle);
			res = glm::normalize(res);
			return res;

		}

		//camera
		camera::ptr m_camera;
		//mouse pos
		glm::vec3 m_start;
		glm::vec3 m_to;
		glm::vec3 m_last_to;
		//save last rotation
		glm::dquat m_rotation;
		//state
		bool      m_valid{ false };
	};
}