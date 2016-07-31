#include <frustum.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp> 
//plane distance
glm::vec4 plane_normalize(const glm::vec4 &plane)
{
	float  size = glm::length((glm::vec3)plane);
	return plane / size;
}

//plane distance
float plane_distance(const glm::vec4 &plane, const glm::vec3& point)
{
	return plane.w + glm::dot((glm::vec3)plane, point);
}

//update planes
void frustum::update_frustum(const glm::mat4& projection)
{
	const glm::vec4& p_x = glm::row(projection,0);
	const glm::vec4& p_y = glm::row(projection,1);
	const glm::vec4& p_z = glm::row(projection,2);
	const glm::vec4& p_w = glm::row(projection,3);

	m_planes[0] = plane_normalize(p_w + p_x);
	m_planes[1] = plane_normalize(p_w - p_x);
	m_planes[2] = plane_normalize(p_w + p_y);
	m_planes[3] = plane_normalize(p_w - p_y);
	m_planes[4] = plane_normalize(p_w + p_z);
	m_planes[5] = plane_normalize(p_w - p_z);
}



//test point
frustum::testing_result frustum::test_point(const glm::vec3& point) const
{
	for (int i = 0; i != N_PLANES; i++) 
	{
		if (plane_distance(m_planes[i], point) < 0) return OUTSIDE;
	}

	return INSIDE;
}

//test obb
frustum::testing_result frustum::test_obb(const obb& box) const
{
	/* todo: use normals */
	frustum::testing_result result = INSIDE;
	// get points
	std::vector< glm::vec3 > points;
	box.get_bounding_box(points);
	// for each plane do ...
	for (int i = 0; i != N_PLANES; i++)
	{
		// reset counters for corners in and out
		int out = 0; 
		int in = 0;
		//test
		for (int k = 0; k != points.size() && (in == 0 || out == 0); ++k)
		{
			// is the corner outside or inside
			if (plane_distance(m_planes[i], points[k]) < 0) out++;
			else in++;
		}
		//if all corners are out
		if (!in) return (OUTSIDE);
		// if some corners are out and others are in
		else if (out) result = INTERSECT;
	}
	return result;
}

//test obb
frustum::testing_result frustum::test_obb(const obb& box, const glm::mat4& model) const
{
	/* todo: use normals */
	frustum::testing_result result = INSIDE;
	// get points
	std::vector< glm::vec3 > points;
	box.get_bounding_box(points, model);
	// for each plane do ...
	for (int i = 0; i != N_PLANES; i++)
	{
		// reset counters for corners in and out
		int out = 0;
		int in = 0;
		//test
		for (int k = 0; k != points.size() && (in == 0 || out == 0); ++k)
		{
			// is the corner outside or inside
			if (plane_distance(m_planes[i], points[k]) < 0) out++;
			else in++;
		}
		//if all corners are out
		if (!in) return (OUTSIDE);
		// if some corners are out and others are in
		else if (out) result = INTERSECT;
	}
	return result;
}