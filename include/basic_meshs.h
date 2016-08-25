//
//  basic_meshs.h
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <mesh.h>
#include <vector_math.h>

namespace hcube
{
	namespace basic_meshs
	{
		mesh::ptr cube(const vec3& size, bool uvmap = false);
		mesh::ptr square3D(const vec2& size, bool uvmap = false);
	};
}