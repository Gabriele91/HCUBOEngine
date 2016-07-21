//
//  light.h
//  HCubo
//
//  Created by Gabriele on 21/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <smart_pointers.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


class light
{
public:
    glm::vec4 m_diffuse;
    float     m_const{ 1.0 };
    float     m_linear;
    float     m_quadratic;
};

using light_ptr  = std::shared_ptr< light >;
using light_uptr = std::unique_ptr< light >;
using light_wptr = std::weak_ptr< light >;

template < class... Args >
static inline light_ptr light_snew(Args&&... args)
{
    return std::make_shared< light >( args... );
}