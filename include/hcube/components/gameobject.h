//
//  gameobject.h
//  HCubo
//
//  Created by Gabriele on 23/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <hcube/core/entity.h>
#include <hcube/components/transform.h>
#include <hcube/components/camera.h>
#include <hcube/components/light.h>
#include <hcube/components/basic_meshs.h>

namespace hcube
{
	namespace gameobject
	{
		template < class... Args >
		static inline entity::ptr node_new(Args&&... args)
		{
			return entity::snew(transform_snew(), args...);
		}

		static inline entity::ptr light_new()
		{
			return entity::snew(transform_snew(), light_snew());
		}

		static inline entity::ptr camera_new()
		{
			return entity::snew(transform_snew(), camera::snew());
		}

		static inline entity::ptr cube_new(const vec3& size, bool uvmap = true)
		{
			return entity::snew(transform_snew(), basic_meshs::cube(size, uvmap));
		}

	}
}