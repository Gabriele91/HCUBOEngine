#pragma once
#include <vector>
#include <vector_math.h>

namespace hcube
{
	class obb
	{
	public:

		obb() {} //default

		obb(const std::vector< vec3 > &points)
		{
			build_from_points(points);
		}

		obb(const std::vector< vec3 > &points, const std::vector<unsigned int>& triangles)
		{
			build_from_triangles(points, triangles);
		}

		obb(const mat3& rotation, const vec3& position, const vec3& extension)
		{
			build_from_attributes(rotation, position, extension);
		}

		//set values of OBB
		void build_from_attributes(const mat3& rotation, const vec3& position, const vec3& extension);

		// builds an OBB from triangles specified as an array of
		// points with integer indices into the point array.
		void build_from_triangles(const std::vector< vec3 > &points, const std::vector<unsigned int>& triangles);

		void build_from_triangles(const unsigned char* points,
								  size_t pos_offset,
								  size_t vertex_size,
								  size_t n_points,
								  const unsigned int* triangles,
								  size_t size);

		// builds an OBB from triangles specified as an array of
		// points 3 by 3
		void build_from_sequenzial_triangles(const std::vector< vec3 >& points);

		void build_from_sequenzial_triangles(const unsigned char* points,
											 size_t pos_offset,
											 size_t vertex_size,
											 size_t n_points);

		// builds an OBB from points specified as an array.
		void build_from_points(const std::vector< vec3 >& points);

		void build_from_points(const unsigned char* points,
							   size_t pos_offset,
							   size_t vertex_size,
							   size_t n_points);

		// constructs the corner of the aligned bounding box
		// in world space
		void get_bounding_box(std::vector< vec3 >& p) const;

		void get_bounding_box(std::vector< vec3 >& p, const mat4& model) const;

		// constructs the corner of the aligned bounding box
		// in world space
		std::vector< vec3 > get_bounding_box() const;

		std::vector< vec3 > get_bounding_box(const mat4& model) const;

		// get volume
		float volume() const;

		//obb rot/trans
		void applay(const mat4& model);

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

		void build_from_covariance_matrix(const mat3& C, const std::vector< vec3 >& points);
		void build_from_covariance_matrix(const mat3& C, const std::vector< vec3 >& points, size_t size);
		void build_from_covariance_matrix
		(
			mat3 C,
			const unsigned char* points,
			size_t pos_offset,
			size_t vertex_size,
			size_t size
		);
	};
}