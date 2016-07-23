//
//  gameobject.h
//  HCubo
//
//  Created by Gabriele on 23/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <transform.h>
#include <camera.h>
#include <light.h>
#include <entity.h>
#include <basic_meshs.h>

namespace gameobject
{
    template < class... Args >
    static inline entity::ptr node_new(Args&&... args)
    {
        return entity::snew(transform_snew(),args...);
    }
    
    static inline entity::ptr light_new()
    {
        return entity::snew(transform_snew(),light_snew());
    }
    
    static inline entity::ptr camera_new()
    {
        return entity::snew(transform_snew(),camera::snew());
    }
    
    static inline entity::ptr cube_new(const glm::vec3& size)
    {
        return entity::snew(transform_snew(),basic_meshs::cube(size));
    }
    
}
