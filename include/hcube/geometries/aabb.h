#pragma once
#include <vector>
#include <hcube/config.h>
#include <hcube/math/vector_math.h>

namespace hcube
{
	class HCUBE_API aabb 
	{
	public:

		aabb(){}

		aabb(const vec3& min, const vec3 max);
		
		const vec3& get_min() const;

		const vec3& get_max() const;

		vec3 get_center() const;

		vec3 get_extension() const;

		/**
		* Return the 8 point of obb
		*/
		std::vector< vec3 > get_bounding_box() const;

		/**
		* Return the 8 points (of obb) multiplied to "model matrix"
		*/
		std::vector< vec3 > get_bounding_box(const mat4& model) const;

		/*
		* Applay a matrix to aabb
		*/
		void applay(const mat4& model);

		/*
		* Applay a matrix to aabb and return the new aabb
		*/
		aabb  operator*  (const mat4& model) const;

		/*
		* Applay a matrix to obb and return this
		*/
		aabb& operator*= (const mat4& model);

		vec3 closest_point(const vec3& point) const;

	protected:

		vec3 m_min;
		vec3 m_max;

	};
}
