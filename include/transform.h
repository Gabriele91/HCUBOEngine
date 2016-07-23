//
//  transform.h
//  HCubo
//
//  Created by Gabriele on 23/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <component.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

class transform : public component
{
    
    COMPONENT_DEC(transform)
    
public:
    
    void look_at(const glm::vec3& eye,const glm::vec3& center,const glm::vec3& up);
    void translation(const glm::vec3& vector);
    void turn(const glm::quat& rot);
    
    void position(const glm::vec3& pos);
    void rotation(const glm::quat& rot);
    void scale(const glm::vec3& pos);
    
    glm::vec3 get_position() const;
    glm::quat get_rotation() const;
    glm::vec3 get_scale() const;
    
    glm::mat4 const& get_matrix();
    glm::mat4 const& get_matrix_inv();
    
    inline operator const glm::mat4& ()
    {
        return get_matrix();
    }
    
private:
    //info
    struct local_tranform
    {
        bool      m_change;
        glm::vec3 m_position;
        glm::quat m_rotation;
        glm::vec3 m_scale;
    }
    m_tranform;
    //matrix
    glm::mat4  m_model_local;
    glm::mat4  m_model_local_inv;
    //compute
    void compute_matrix();
    
};

using transform_ptr  = std::shared_ptr< transform >;
using transform_uptr = std::unique_ptr< transform >;
using transform_wptr = std::weak_ptr< transform >;

template < class... Args >
static inline transform_ptr transform_snew(Args&&... args)
{
    return std::make_shared< transform >( args... );
}