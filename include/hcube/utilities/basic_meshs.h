//
//  basic_meshs.h
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <hcube/config.h>
#include <hcube/math/vector_math.h>
#include <hcube/components/mesh.h>

namespace hcube
{
	namespace basic_meshs
	{
		HCUBE_API mesh::ptr cube(const vec3& size, bool uvmap = false);
		HCUBE_API mesh::ptr square3D(const vec2& size, bool uvmap = false);
		HCUBE_API mesh::ptr sphere(const float radius, const bool use_uvmap = false);
		HCUBE_API mesh::ptr sphere(const float radius, const int rings, const int sectors, const bool use_uvmap = false);
		HCUBE_API mesh::ptr icosphere(const float radius, const const bool uvmap = false);
		HCUBE_API mesh::ptr icosphere(const float radius, int lod, const bool uvmap = false);
	}
}