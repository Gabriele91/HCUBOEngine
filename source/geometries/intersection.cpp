#include <hcube/geometries/intersection.h>

namespace hcube
{
	namespace intersection
	{
		//plane distance
		static float plane_distance(const vec4 &plane, const vec3& point)
		{
			return plane.w + dot((vec3)plane, point);
		}
		//frustum vs obb
		result check(const frustum& in_frustum, const obb& in_obb)
		{
			result result = INSIDE;
			//refs
			const mat3& rotation = transpose(in_obb.get_rotation_matrix());
			const vec3& position = in_obb.get_position();
			//axis
			auto& axis_0 = rotation[0];//column(rotation, 0);
			auto& axis_1 = rotation[1];//column(rotation, 1);
			auto& axis_2 = rotation[2];//column(rotation, 2);

			for (int i = 0; i != frustum::N_PLANES; ++i)
			{
				// plane normal 
				const vec3 norm = ((vec3)in_frustum.plane((frustum::plane_type)i));
				// get extension
				const vec3& ext = in_obb.get_extension();
				// distance
				float dist = ext.x * fabs(dot(axis_0, norm)) +
					ext.y * fabs(dot(axis_1, norm)) +
					ext.z * fabs(dot(axis_2, norm));
				//compute distances
				float distance_box = plane_distance(in_frustum.plane((frustum::plane_type)i), position);
				//like sphere 
				if (distance_box < -dist) return OUTSIDE;
				else if (distance_box <= dist) result = INTERSECT;
			}
			return result;
		}
		//frustum vs sphere
		result check(const frustum& in_frustum, const sphere& in_sphere)
		{
			float center_distance;
			result result = INSIDE;

			for (int i = 0; i != frustum::N_PLANES; i++)
			{
				//calc distance
				center_distance = plane_distance(in_frustum.plane((frustum::plane_type)i), in_sphere.get_position());
				//test
				if (center_distance < -in_sphere.get_radius()) return OUTSIDE;
				else if (center_distance <= in_sphere.get_radius()) result = INTERSECT;
			}

			return result;
		}
		//frustum vs point
		result check(const frustum& in_frustum, const vec3& in_point)
		{
			for (int i = 0; i != frustum::N_PLANES; i++)
			{
				if (plane_distance(in_frustum.plane((frustum::plane_type)i), in_point) < 0) return OUTSIDE;
			}
			return INSIDE;
		}
		//obb vs sphere
		result check(const obb& in_obb, const sphere& in_sphere)
		{
			// Find the point on this AABB closest to the sphere center.
			const vec3 pt = in_obb.closest_point(in_sphere.get_position());

			//squere distance
			const vec3  diff = pt - in_sphere.get_position();
			const float sq_dist = dot(diff, diff);
			//float squere radius
			const float s_radius = in_sphere.get_radius() * in_sphere.get_radius();
			//test sq distance
			if (sq_dist <  s_radius) return INSIDE;
			if (sq_dist == s_radius) return INTERSECT;
			return OUTSIDE;
		}
		//obb vs point
		result check(const obb& in_obb, const vec3& in_point)
		{
			//todo
			assert(0);
			return OUTSIDE;
		}
		//sphere vs sphere
		result check(const sphere& in_sphere1, const sphere& in_sphere2)
		{
			//float squere radius
			const float s_radius_1 = in_sphere1.get_radius() * in_sphere1.get_radius();
			const float s_radius_2 = in_sphere2.get_radius() * in_sphere2.get_radius();
			//radius
			const float sum_radius = s_radius_1 + s_radius_2;
			//squere distance
			const vec3  diff    = in_sphere1.get_position() - in_sphere2.get_position();
			const float sq_dist = dot(diff, diff);
			//test sq distance
			if (sq_dist <= sum_radius)
			{
				if ((std::sqrt(s_radius_1) - std::sqrt(sq_dist)) > std::sqrt(s_radius_2))
				{
					return INSIDE;
				}
				return INTERSECT;
			}
			return OUTSIDE;
		}
		//sphere vs point
		result check(const sphere& in_sphere, const vec3& in_point)
		{
			//float squere radius
			const float s_radius = in_sphere.get_radius() * in_sphere.get_radius();
			//squere distance
			const vec3  diff = in_point - in_sphere.get_position();
			const float sq_dist = dot(diff, diff);
			//test sq distance
			if (sq_dist <  s_radius) return INSIDE;
			if (sq_dist == s_radius) return INTERSECT;
			return OUTSIDE;
		}
	}
}