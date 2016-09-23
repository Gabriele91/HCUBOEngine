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

		//distance
		float distance_from_plane(plane_type plane, const vec3& point) const;
		float distance_from_near_plane(const vec3& point) const;

		//get plane
		const vec4& plane(const plane_type& type) const
		{
			return m_planes[type];
		}

	private:

		vec4 m_planes[N_PLANES];

	};
}