#pragma once
#include <vector_math.h>
#include <obb.h>

namespace hcube
{
	class frustum
	{
	public:

		//planes
		enum plane_type
		{
			TOP = 0,
			BOTTOM,
			LEFT,
			RIGHT,
			NEARP,
			FARP,
			N_PLANES
		};

		//results
		enum testing_result { OUTSIDE, INTERSECT, INSIDE };

		//update planes
		void update_frustum(const mat4& projection);

		//test point
		testing_result test_point(const vec3& point) const;

		//test sphere
		testing_result test_sphere(const vec3& point, float radius) const;

		//test obb
		testing_result test_obb(const obb& box) const;

		//test obb
		testing_result test_obb(const obb& box, const mat4& model) const;

		//distance
		float distance_from_plane(plane_type plane, const vec3& point) const;
		float distance_from_near_plane(const vec3& point) const;

	private:

		vec4 m_planes[N_PLANES];

	};
}