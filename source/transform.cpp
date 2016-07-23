//
//  transform.cpp
//  HCubo
//
//  Created by Gabriele on 23/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <transform.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

void transform::look_at(const glm::vec3& eye,const glm::vec3& center,const glm::vec3& up)
{
    glm::mat4 l_look_at  = glm::lookAt(eye,center,up);
    glm::quat l_rotation = glm::quat(glm::mat3(l_look_at));
    rotation(l_rotation);
    position(eye);
}

void transform::translation(const glm::vec3& vector)
{
    m_tranform.m_position += vector;
    m_tranform.m_change    = true;
}

void transform::turn(const glm::quat& rot)
{
    m_tranform.m_rotation  = rot * m_tranform.m_rotation;
    m_tranform.m_change    = true;
}

void transform::position(const glm::vec3& pos)
{
    m_tranform.m_position  = pos;
    m_tranform.m_change    = true;
}

void transform::rotation(const glm::quat& rot)
{
    m_tranform.m_rotation  = rot;
    m_tranform.m_change    = true;
}

void transform::scale(const glm::vec3& scale)
{
    m_tranform.m_scale   = scale;
    m_tranform.m_change  = true;
}

glm::vec3 transform::get_position() const
{
    return  m_tranform.m_position;
}

glm::quat transform::get_rotation() const
{
    return m_tranform.m_rotation;
}


glm::mat4 const& transform::get_matrix()
{
    compute_matrix();
    return m_model_local;
}
glm::mat4 const& transform::get_matrix_inv()
{
    compute_matrix();
    return m_model_local_inv;
}

void transform::compute_matrix()
{
    if(m_tranform.m_change)
    {
        //T*R*S
        m_model_local = glm::translate(glm::mat4(1.0f),m_tranform.m_position);
        m_model_local*= glm::mat4_cast(m_tranform.m_rotation);
        m_model_local = glm::scale(m_model_local, m_tranform.m_scale);
        //inverse
        m_model_local_inv = glm::inverse(m_model_local);
    }
}