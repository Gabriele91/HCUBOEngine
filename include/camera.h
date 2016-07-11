//
//  camera.h
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <smart_pointers.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

class camera : public smart_pointers< camera >
{
public:
    
    void set_viewport(const glm::ivec4& viewport);
    void set_perspective(float fov,float aspect,float near,float far);
    void look_at(const glm::vec3& eye,const glm::vec3& center,const glm::vec3& up);
    
    void translation(const glm::vec3& vector);
    void turn(const glm::quat& rot);
    
    void position(const glm::vec3& pos);
    void rotation(const glm::quat& rot);
    
    glm::vec3 get_position() const;
    glm::quat get_rotation() const;
    
    const glm::ivec2  get_viewport_size() const;
    const glm::ivec4& get_viewport() const;
    const glm::mat4&  get_projection() const;
    const glm::mat4&  get_projection_inv() const;
    const glm::mat4&  get_view();
    const glm::mat4&  get_view_inv();
    
protected:
    
    struct tranform
    {
        bool      m_change;
        glm::vec3 m_position;
        glm::quat m_rotation;
    }
    m_tranform;
    
    glm::ivec4 m_viewport;
    glm::mat4  m_projection;
    glm::mat4  m_projection_inv;
    glm::mat4  m_view;
    glm::mat4  m_view_inv;
    
    void compute_matrix();
    
};