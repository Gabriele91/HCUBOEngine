//
//  renderable.h
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <smart_pointers.h>
#include <material.h>

class renderable
{
public:

	virtual void draw(camera& cam,
					  const glm::mat4& model,
					  material_ptr material) {};
    virtual ~renderable(){};
};

using renderable_ptr  = std::shared_ptr< renderable >;
using renderable_uptr = std::unique_ptr< renderable >;
using renderable_wptr = std::weak_ptr  < renderable >;