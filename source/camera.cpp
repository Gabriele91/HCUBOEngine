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

glm::vec2 camera::get_near_and_far() const
{
    float m22 = -m_projection[2][2];
    float m32 = -m_projection[3][2];
    
    float far  = (2.0f*m32)/(2.0f*m22-2.0f);
    float near = ((m22-1.0f)*far)/(m22+1.0);
    
    return glm::vec2(near,far);
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

