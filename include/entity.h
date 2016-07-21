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
#include <light.h>
#include <renderable.h>
#include <smart_pointers.h>

class entity : public smart_pointers< entity >
{
    
public:
    
    glm::mat4      m_model;
    light_ptr      m_light;
    renderable_ptr m_renderable;
    material_ptr   m_material;
    
    entity()
    {
        m_light      = nullptr;
        m_renderable = nullptr;
        m_material   = nullptr;
    }
    
    
    entity(light_ptr      arg_light,
           renderable_ptr arg_renderable,
           material_ptr   arg_material)
    {
        m_light      = arg_light;
        m_renderable = arg_renderable;
        m_material   = arg_material;
    }
    
    entity(renderable_ptr arg_renderable,
           material_ptr   arg_material)
    {
        m_renderable = arg_renderable;
        m_material   = arg_material;
    }
    
    entity(light_ptr      arg_light,
           material_ptr   arg_material)
    {
        m_light    = arg_light;
        m_material = arg_material;
    }
    
    entity(light_ptr arg_light)
    {
        m_light = arg_light;
    }
    
    ~entity()
    {
    }
    
};
