#pragma once
#include <hcube/math/vector_math.h>
#include <hcube/geometries/obb.h>
#include <hcube/geometries/sphere.h>
#include <hcube/geometries/frustum.h>

namespace hcube
{
	namespace intersection
	{
		enum result
		{
			OUTSIDE,
			INTERSECT,
			INSIDE
		};

		//frustum vs obb
		result check(const frustum& in_frustum, const obb& in_obb);
		//frustum vs sphere
		result check(const frustum& in_frustum, const sphere& in_sphere);
		//frustum vs point
		result check(const frustum& in_frustum, const vec3& in_point);
		//obb vs sphere
		result check(const obb& in_obb, const sphere& in_sphere);
		//obb vs point
		result check(const obb& in_obb, const vec3& in_point);
		//sphere vs sphere
		result check(const sphere& in_sphere1, const sphere& in_sphere2);
		//sphere vs point
		result check(const sphere& in_sphere, const vec3& in_point);
	}
}