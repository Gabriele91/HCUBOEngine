//
//  camera.h
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <smart_pointers.h>
#include <component.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <frustum.h>

namespace hcube
{

	class camera : public component, public smart_pointers< camera >
	{

		COMPONENT_DEC(camera)

	public:

		void set_viewport(const glm::ivec4& viewport);
		void set_perspective(float fov, float aspect, float near, float far);
		glm::vec2 get_near_and_far() const;

		const glm::ivec2  get_viewport_size() const;
		const glm::ivec4& get_viewport() const;
		const glm::mat4&  get_projection() const;
		const glm::mat4&  get_projection_inv() const;

		frustum& get_frustum();
		const frustum& get_frustum() const;
		void update_view_frustum();

		virtual component_ptr copy() const;

	protected:

		frustum	   m_frustum;
		glm::ivec4 m_viewport;
		glm::mat4  m_projection;
		glm::mat4  m_projection_inv;

	};
}