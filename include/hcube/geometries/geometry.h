#pragma once
#include <hcube/geometries/obb.h>
#include <hcube/geometries/sphere.h>

namespace hcube
{
	namespace geometry
	{

		HCUBE_API obb obb_from_sequenzial_triangles(const std::vector< vec3 >& points);

		HCUBE_API obb obb_from_sequenzial_triangles(const unsigned char* points, size_t pos_offset, size_t vertex_size, size_t n_points);

		HCUBE_API obb obb_from_triangles(const std::vector< vec3 > &points, const std::vector<unsigned int>& triangles);

		HCUBE_API obb obb_from_triangles(const unsigned char* points, size_t pos_offset, size_t vertex_size, size_t n_points, const unsigned int* triangles, size_t size);

		HCUBE_API obb obb_from_points(const std::vector< vec3 >& points);

		HCUBE_API obb obb_from_points(const unsigned char* points, size_t pos_offset, size_t vertex_size, size_t n_points);

		HCUBE_API sphere sphere_from_points(const std::vector< vec3 >& points);

		HCUBE_API sphere sphere_from_points(const unsigned char* points, size_t pos_offset, size_t vertex_size, size_t n_points);

	}
}