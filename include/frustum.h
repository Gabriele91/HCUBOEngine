#pragma once
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <obb.h>

class frustum
{
public:

	//results
	static enum testing_result { OUTSIDE, INTERSECT, INSIDE };

	//update planes
	void update_frustum(const glm::mat4& projection);

	//test point
	testing_result test_point(const glm::vec3& point) const;

	//test sphere
	testing_result test_sphere(const glm::vec3& point,float radius) const;

	//test obb
	testing_result test_obb(const obb& box) const;

	//test obb
	testing_result test_obb(const obb& box,const glm::mat4& model) const;

private:


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

	glm::vec4 m_planes[N_PLANES];

};