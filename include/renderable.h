//
//  renderable.h
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <component.h>
#include <smart_pointers.h>
#include <material.h>

class renderable : public component
{
    
    COMPONENT_DEC(renderable)
    
public:

    renderable(){}
    
	virtual void draw(const glm::vec4& viewport,
                      const glm::mat4& projection,
                      const glm::mat4& view,
					  const glm::mat4& model,
					  material_ptr material)
    {
        assert(0);
    };
    virtual ~renderable(){};
};

using renderable_ptr  = std::shared_ptr< renderable >;
using renderable_uptr = std::unique_ptr< renderable >;
using renderable_wptr = std::weak_ptr  < renderable >;