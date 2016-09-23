#include <frustum.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp> 

namespace hcube
{
	//plane distance
	static float plane_distance(const vec4 &plane, const vec3& point)
	{
		return plane.w + dot((vec3)plane, point);
	}
	//plane distance
	vec4 plane_normalize(const vec4 &plane)
	{
		float  size = length((vec3)plane);
		return plane / size;
	}
	//update planes
	void frustum::update_frustum(const mat4& projection)
	{
		const vec4& p_x = row(projection, 0);
		const vec4& p_y = row(projection, 1);
		const vec4& p_z = row(projection, 2);
		const vec4& p_w = row(projection, 3);

		m_planes[0] = plane_normalize(p_w + p_x);
		m_planes[1] = plane_normalize(p_w - p_x);
		m_planes[2] = plane_normalize(p_w + p_y);
		m_planes[3] = plane_normalize(p_w - p_y);
		m_planes[4] = plane_normalize(p_w + p_z);
		m_planes[5] = plane_normalize(p_w - p_z);
	}
	//distance
	float frustum::distance_from_plane(frustum::plane_type plane, const vec3& point) const
	{
		return plane_distance(m_planes[plane], point);
	}
	float frustum::distance_from_near_plane(const vec3& point) const
	{
		return plane_distance(m_planes[NEARP], point);
	}
}