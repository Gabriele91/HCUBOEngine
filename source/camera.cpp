//
//  camera.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

void camera::set_viewport(const glm::ivec4& viewport)
{
    m_viewport = viewport;
}

void camera::set_perspective(float fov,float aspect,float near,float far)
{
    m_projection     = glm::perspective(fov, aspect, near, far);
    m_projection_inv = glm::inverse(m_projection);
}

const glm::ivec2  camera::get_viewport_size() const
{
    return glm::ivec2{ m_viewport.z - m_viewport.x, m_viewport.w - m_viewport.y };
}

const glm::ivec4& camera::get_viewport() const
{
    return m_viewport;
}

const glm::mat4& camera::get_projection() const
{
    return m_projection;
}

const glm::mat4& camera::get_projection_inv() const
{
    return m_projection_inv;
}

