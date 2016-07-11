//
//  entity.h
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <material.h>
#include <glm/mat4x4.hpp>
#include <renderable.h>
#include <smart_pointers.h>

class entity : public smart_pointers< entity >
{
    
public:
    
    glm::mat4      m_model;
    renderable_ptr m_renderable;
    material_ptr   m_material;
    
    static ptr create(renderable_ptr arg_renderable,
                      material_ptr   arg_material)
    {
        auto thiz = snew();
        thiz->m_renderable = arg_renderable;
        thiz->m_material = arg_material;
        return thiz;
    }
    
    entity()
    {
        m_renderable = nullptr;
        m_material   = nullptr;
    }
    
    entity(renderable_ptr arg_renderable,
           material_ptr   arg_material)
    {
        m_renderable = arg_renderable;
        m_material   = arg_material;
    }
    
    ~entity()
    {
    }
    
};
