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

void camera::look_at(const glm::vec3& eye,const glm::vec3& center,const glm::vec3& up)
{
    glm::mat4 look_at = glm::lookAt(eye,center,up);
    glm::quat la_rotation = glm::quat(glm::mat3(look_at));
	rotation(glm::inverse(la_rotation));
	position(eye);
}

void camera::translation(const glm::vec3& vector)
{
    m_tranform.m_position += vector;
    m_tranform.m_change    = true;
}

void camera::turn(const glm::quat& rot)
{
    m_tranform.m_rotation *= rot;
    m_tranform.m_change    = true;
}

void camera::position(const glm::vec3& pos)
{
    m_tranform.m_position  = pos;
    m_tranform.m_change    = true;
}

void camera::rotation(const glm::quat& rot)
{
    m_tranform.m_rotation  = rot;
    m_tranform.m_change    = true;
}

glm::vec3 camera::get_position() const
{
    return  m_tranform.m_position;
}

glm::quat camera::get_rotation() const
{
    return m_tranform.m_rotation;
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

const glm::mat4& camera::get_view()
{
    compute_matrix();
    return m_view;
}

const glm::mat4& camera::get_view_inv()
{
    compute_matrix();
    return m_view_inv;
}

void camera::compute_matrix()
{
    if(m_tranform.m_change)
    {
		m_view_inv = glm::translate(glm::mat4(1.0f),m_tranform.m_position);
        m_view_inv*= glm::mat4_cast(m_tranform.m_rotation);
        m_view     = glm::inverse(m_view_inv);
    }
}
