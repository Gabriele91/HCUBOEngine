#pragma once
#include <vector>
#include <hcube/config.h>
#include <hcube/math/vector_math.h>

namespace hcube
{


	class HCUBE_API obb
	{
	public:
		/*		
		* Constructor of obb
		*/
		obb() {} //default

		/**
		* Constructor of obb
		* @param rotation, set rotation of obb
		* @param position, set local position of obb
		* @param extension, set extension (aka size) of obb
		*/
		obb(const mat3& rotation, const vec3& position, const vec3& extension);

		/**
		* Set values of obb
		* @param rotation, set rotation of obb
		* @param position, set local position of obb
		* @param extension, set extension (aka size) of obb
		*/
		void set(const mat3& rotation, const vec3& position, const vec3& extension);

		/**
		* Return the 8 point of obb
		*/
		std::vector< vec3 > get_bounding_box() const;

		/**
		* Return the 8 points (of obb) multiplied to "model matrix"
		*/
		std::vector< vec3 > get_bounding_box(const mat4& model) const;

		// get volume
		float volume() const;

		/*
		* Applay a matrix to obb
		*/
		void applay(const mat4& model);

		/*
		* Applay a matrix to obb and return the new obb
		*/
		obb  operator*  (const mat4& model) const;

		/*
		* Applay a matrix to obb and return this 
		*/
		obb& operator*= (const mat4& model);

		//test
		vec3 closest_point(const vec3& target) const;

		//get info
		const mat3& get_rotation_matrix() const { return m_rotation; }
		const vec3& get_position() const { return m_position; }
		const vec3& get_extension() const { return m_extension; }

	private:

		mat3 m_rotation; // rotation matrix for the transformation
		vec3 m_position; // translation component of the transformation
		vec3 m_extension;// bounding box extents

	};
}