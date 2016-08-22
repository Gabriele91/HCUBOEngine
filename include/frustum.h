#pragma once
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
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
		void update_frustum(const glm::mat4& projection);

		//test point
		testing_result test_point(const glm::vec3& point) const;

		//test sphere
		testing_result test_sphere(const glm::vec3& point, float radius) const;

		//test obb
		testing_result test_obb(const obb& box) const;

		//test obb
		testing_result test_obb(const obb& box, const glm::mat4& model) const;

		//distance
		float distance_from_plane(plane_type plane, const glm::vec3& point) const;
		float distance_from_near_plane(const glm::vec3& point) const;

	private:

		glm::vec4 m_planes[N_PLANES];

	};
}