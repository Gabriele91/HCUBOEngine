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


//distance
float frustum::distance_from_plane(frustum::plane_type plane, const glm::vec3& point) const
{
    return plane_distance(m_planes[plane],point);
}

float frustum::distance_from_near_plane(const glm::vec3& point) const
{
    return plane_distance(m_planes[NEARP], point);
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

//test sphere
frustum::testing_result frustum::test_sphere(const glm::vec3& point, float radius) const
{
	float center_distance;
	testing_result result = INSIDE;

	for (int i = 0; i != N_PLANES; i++)
	{
		//calc distance
		center_distance = plane_distance(m_planes[i], point);
		//test
			 if (center_distance < -radius) return OUTSIDE;
		else if (center_distance <= radius) result = INTERSECT;
	}

	return result;
}

//test obb
frustum::testing_result frustum::test_obb(const obb& box) const
{
#if 1
	// Project the box onto the line defined by the plane center and normal.
	// See "3D Game Engine Design" chapter 4.3.2.
	frustum::testing_result result = INSIDE;
	//refs
	const glm::mat3& rotation = glm::transpose(box.get_rotation_matrix());
	const glm::vec3& position = box.get_position();
	//axis
	auto& axis_0 = rotation[0];//glm::column(rotation, 0);
	auto& axis_1 = rotation[1];//glm::column(rotation, 1);
	auto& axis_2 = rotation[2];//glm::column(rotation, 2);

	for (int i = 0; i != N_PLANES; ++i)
	{
		// plane normal 
		const glm::vec3 norm = ((glm::vec3)m_planes[i]);
		// get extension
		const glm::vec3& ext = box.get_extension();
		// distance
		float dist = ext.x * fabs(glm::dot(axis_0, norm)) +
			ext.y * fabs(glm::dot(axis_1, norm)) +
			ext.z * fabs(glm::dot(axis_2, norm));
		//compute distance
		float distance_box = plane_distance(m_planes[i], position);
		//like sphere 
		if (distance_box < -dist) return OUTSIDE;
		else if (distance_box <= dist) result = INTERSECT;
	}
	return result;
#else
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
#endif
}

//test obb
frustum::testing_result frustum::test_obb(const obb& box, const glm::mat4& model) const
{
#if 1
	// Project the box onto the line defined by the plane center and normal.
	// See "3D Game Engine Design" chapter 4.3.2.
	frustum::testing_result result = INSIDE;
	//rotation/position in global world
	glm::mat3 rotation = glm::mat3(model)*glm::transpose( box.get_rotation_matrix() );
	glm::vec3 position = (glm::vec3)(model*glm::vec4(box.get_position(), 1.0));
	//axis
	auto& axis_0 = rotation[0];//glm::column(rotation, 0);
	auto& axis_1 = rotation[1];//glm::column(rotation, 1);
	auto& axis_2 = rotation[2];//glm::column(rotation, 2);

	for (int i = 0; i != N_PLANES; ++i)
	{
		// plane normal 
		const glm::vec3 norm = ((glm::vec3)m_planes[i]);
		// get extension
		const glm::vec3& ext = box.get_extension();
		// distance
		float dist = ext.x * fabs(glm::dot(axis_0, norm)) +
					 ext.y * fabs(glm::dot(axis_1, norm)) +
				     ext.z * fabs(glm::dot(axis_2, norm));
		//compute distance
		float distance_box = plane_distance(m_planes[i], position);
		//like sphere 
			 if (distance_box < -dist) return OUTSIDE;
	    else if (distance_box <= dist) result = INTERSECT;
	}
	return result;
#else
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
			if (plane_distance(m_planes[i], points[k]) < 0) ++out;
			else ++in;
		}
		//if all corners are out
		if (!in) return OUTSIDE;
		// if some corners are out and others are in
		else if (out) result = INTERSECT;
	}
	return result;
#endif
}