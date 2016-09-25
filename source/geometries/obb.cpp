#include <hcube/math/vector_math.h>
#include <hcube/geometries/obb.h>
#include <glm/gtx/matrix_decompose.hpp>

namespace hcube
{
	void obb::build_from_attributes(const mat3& rotation, const vec3& position, const vec3& extension)
	{
		m_rotation = rotation;
		m_position = position;
		m_extension = extension;
	}

	void obb::build_from_sequenzial_triangles(const std::vector< vec3 >& points)
	{
		float Ai = 0.0;
		float Am = 0.0;
		vec3 mu(0.0f, 0.0f, 0.0f), mui;
		mat3 C;
		float cxx = 0.0, cxy = 0.0, cxz = 0.0, cyy = 0.0, cyz = 0.0, czz = 0.0;

		// loop over the triangles this time to find the
		// mean location
		for (int i = 0; i < (int)points.size(); i += 3)
		{
			const  vec3& p = points[i + 0];
			const  vec3& q = points[i + 1];
			const  vec3& r = points[i + 2];
			mui = (p + q + r) / 3.0f;

			//compute len( triangle  dir/norm )
			Ai = length(cross((q - p), (r - p))) / 2.0f;

			//inc
			mu += mui*Ai;
			Am += Ai;

			// these bits set the c terms to Am*E[xx], Am*E[xy], Am*E[xz]....
			cxx += (9.0*mui.x*mui.x + p.x*p.x + q.x*q.x + r.x*r.x)*(Ai / 12.0);
			cxy += (9.0*mui.x*mui.y + p.x*p.y + q.x*q.y + r.x*r.y)*(Ai / 12.0);
			cxz += (9.0*mui.x*mui.z + p.x*p.z + q.x*q.z + r.x*r.z)*(Ai / 12.0);
			cyy += (9.0*mui.y*mui.y + p.y*p.y + q.y*q.y + r.y*r.y)*(Ai / 12.0);
			cyz += (9.0*mui.y*mui.z + p.y*p.z + q.y*q.z + r.y*r.z)*(Ai / 12.0);
		}
		// divide out the Am fraction from the average position and 
		// covariance terms
		mu /= Am;
		cxx /= Am; cxy /= Am; cxz /= Am; cyy /= Am; cyz /= Am; czz /= Am;

		// now subtract off the E[x]*E[x], E[x]*E[y], ... terms
		cxx -= mu.x*mu.x; cxy -= mu.x*mu.y; cxz -= mu.x*mu.z;
		cyy -= mu.y*mu.y; cyz -= mu.y*mu.z; czz -= mu.z*mu.z;

		// now build the covariance matrix
		C[0][0] = cxx; C[0][1] = cxy; C[0][2] = cxz;
		C[1][0] = cxy; C[1][1] = cyy; C[1][2] = cyz;
		C[2][0] = cxz; C[2][1] = cyz; C[2][2] = czz;


		// set the obb parameters from the covariance matrix
		build_from_covariance_matrix(C, points);
	}

	void obb::build_from_sequenzial_triangles(const unsigned char* points,
		size_t pos_offset,
		size_t vertex_size,
		size_t n_points)
	{
		float Ai = 0.0;
		float Am = 0.0;
		vec3 mu(0.0f, 0.0f, 0.0f), mui;
		mat3 C;
		float cxx = 0.0, cxy = 0.0, cxz = 0.0, cyy = 0.0, cyz = 0.0, czz = 0.0;

		// loop over the triangles this time to find the
		// mean location
		for (int i = 0; i < (int)n_points; i += 3)
		{
#define att_vertex(i) (*(const vec3*)(points+vertex_size*(i)+pos_offset))
			const vec3& p = att_vertex(i + 0);
			const vec3& q = att_vertex(i + 1);
			const vec3& r = att_vertex(i + 2);
#undef att_vertex
			mui = (p + q + r) / 3.0f;

			//compute len( triangle  dir/norm )
			Ai = length(cross((q - p), (r - p))) / 2.0f;

			//inc
			mu += mui*Ai;
			Am += Ai;

			// these bits set the c terms to Am*E[xx], Am*E[xy], Am*E[xz]....
			cxx += (9.0*mui.x*mui.x + p.x*p.x + q.x*q.x + r.x*r.x)*(Ai / 12.0);
			cxy += (9.0*mui.x*mui.y + p.x*p.y + q.x*q.y + r.x*r.y)*(Ai / 12.0);
			cxz += (9.0*mui.x*mui.z + p.x*p.z + q.x*q.z + r.x*r.z)*(Ai / 12.0);
			cyy += (9.0*mui.y*mui.y + p.y*p.y + q.y*q.y + r.y*r.y)*(Ai / 12.0);
			cyz += (9.0*mui.y*mui.z + p.y*p.z + q.y*q.z + r.y*r.z)*(Ai / 12.0);
		}
		// divide out the Am fraction from the average position and 
		// covariance terms
		mu /= Am;
		cxx /= Am; cxy /= Am; cxz /= Am; cyy /= Am; cyz /= Am; czz /= Am;

		// now subtract off the E[x]*E[x], E[x]*E[y], ... terms
		cxx -= mu.x*mu.x; cxy -= mu.x*mu.y; cxz -= mu.x*mu.z;
		cyy -= mu.y*mu.y; cyz -= mu.y*mu.z; czz -= mu.z*mu.z;

		// now build the covariance matrix
		C[0][0] = cxx; C[0][1] = cxy; C[0][2] = cxz;
		C[1][0] = cxy; C[1][1] = cyy; C[1][2] = cyz;
		C[2][0] = cxz; C[2][1] = cyz; C[2][2] = czz;


		// set the obb parameters from the covariance matrix
		build_from_covariance_matrix(C, points, pos_offset, vertex_size, n_points);
	}

	void obb::build_from_triangles(const std::vector< vec3 >& points, const  std::vector<unsigned int>& triangles)
	{
		float Ai = 0.0;
		float Am = 0.0;
		vec3 mu(0.0f, 0.0f, 0.0f), mui;
		mat3 C;
		float cxx = 0.0, cxy = 0.0, cxz = 0.0, cyy = 0.0, cyz = 0.0, czz = 0.0;

		// loop over the triangles this time to find the
		// mean location
		for (int i = 0; i < (int)triangles.size(); i += 3)
		{
			const vec3& p = points[triangles[i + 0]];
			const vec3& q = points[triangles[i + 1]];
			const vec3& r = points[triangles[i + 2]];
			mui = (p + q + r) / 3.0f;

			//compute len( triangle  dir/norm )
			Ai = length(cross((q - p), (r - p))) / 2.0f;

			//inc
			mu += mui*Ai;
			Am += Ai;

			// these bits set the c terms to Am*E[xx], Am*E[xy], Am*E[xz]....
			cxx += (9.0*mui.x*mui.x + p.x*p.x + q.x*q.x + r.x*r.x)*(Ai / 12.0);
			cxy += (9.0*mui.x*mui.y + p.x*p.y + q.x*q.y + r.x*r.y)*(Ai / 12.0);
			cxz += (9.0*mui.x*mui.z + p.x*p.z + q.x*q.z + r.x*r.z)*(Ai / 12.0);
			cyy += (9.0*mui.y*mui.y + p.y*p.y + q.y*q.y + r.y*r.y)*(Ai / 12.0);
			cyz += (9.0*mui.y*mui.z + p.y*p.z + q.y*q.z + r.y*r.z)*(Ai / 12.0);
		}
		// divide out the Am fraction from the average position and 
		// covariance terms
		mu /= Am;
		cxx /= Am; cxy /= Am; cxz /= Am; cyy /= Am; cyz /= Am; czz /= Am;

		// now subtract off the E[x]*E[x], E[x]*E[y], ... terms
		cxx -= mu.x*mu.x; cxy -= mu.x*mu.y; cxz -= mu.x*mu.z;
		cyy -= mu.y*mu.y; cyz -= mu.y*mu.z; czz -= mu.z*mu.z;

		// now build the covariance matrix
		C[0][0] = cxx; C[0][1] = cxy; C[0][2] = cxz;
		C[1][0] = cxy; C[1][1] = cyy; C[1][2] = cyz;
		C[2][0] = cxz; C[2][1] = cyz; C[2][2] = czz;


		// set the obb parameters from the covariance matrix
		build_from_covariance_matrix(C, points);
	}

	void obb::build_from_triangles(const unsigned char* points,
		size_t pos_offset,
		size_t vertex_size,
		size_t n_points,
		const unsigned int* triangles,
		size_t size)
	{
		float Ai = 0.0;
		float Am = 0.0;
		vec3 mu(0.0f, 0.0f, 0.0f), mui;
		mat3 C;
		float cxx = 0.0, cxy = 0.0, cxz = 0.0, cyy = 0.0, cyz = 0.0, czz = 0.0;
		// access
		// loop over the triangles this time to find the
		// mean location
		for (int i = 0; i < (int)size; i += 3)
		{
#define att_vertex(i) (*(const vec3*)(points+vertex_size*(i)+pos_offset))
			const vec3& p = att_vertex(triangles[i + 0]);
			const vec3& q = att_vertex(triangles[i + 1]);
			const vec3& r = att_vertex(triangles[i + 2]);
#undef att_vertex
			mui = (p + q + r) / 3.0f;

			//compute len( triangle  dir/norm )
			Ai = length(cross((q - p), (r - p))) / 2.0f;

			//inc
			mu += mui*Ai;
			Am += Ai;

			// these bits set the c terms to Am*E[xx], Am*E[xy], Am*E[xz]....
			cxx += (9.0*mui.x*mui.x + p.x*p.x + q.x*q.x + r.x*r.x)*(Ai / 12.0);
			cxy += (9.0*mui.x*mui.y + p.x*p.y + q.x*q.y + r.x*r.y)*(Ai / 12.0);
			cxz += (9.0*mui.x*mui.z + p.x*p.z + q.x*q.z + r.x*r.z)*(Ai / 12.0);
			cyy += (9.0*mui.y*mui.y + p.y*p.y + q.y*q.y + r.y*r.y)*(Ai / 12.0);
			cyz += (9.0*mui.y*mui.z + p.y*p.z + q.y*q.z + r.y*r.z)*(Ai / 12.0);
		}
		// divide out the Am fraction from the average position and 
		// covariance terms
		mu /= Am;
		cxx /= Am; cxy /= Am; cxz /= Am; cyy /= Am; cyz /= Am; czz /= Am;

		// now subtract off the E[x]*E[x], E[x]*E[y], ... terms
		cxx -= mu.x*mu.x; cxy -= mu.x*mu.y; cxz -= mu.x*mu.z;
		cyy -= mu.y*mu.y; cyz -= mu.y*mu.z; czz -= mu.z*mu.z;

		// now build the covariance matrix
		C[0][0] = cxx; C[0][1] = cxy; C[0][2] = cxz;
		C[1][0] = cxy; C[1][1] = cyy; C[1][2] = cyz;
		C[2][0] = cxz; C[2][1] = cyz; C[2][2] = czz;


		// set the obb parameters from the covariance matrix
		build_from_covariance_matrix(C, points, pos_offset, vertex_size, n_points);
	}

	void obb::build_from_points(const std::vector< vec3 >& points)
	{
		vec3 mu(0.0, 0.0, 0.0);
		mat3 C;

		// loop over the points to find the mean point
		// location
		for (int i = 0; i < (int)points.size(); i++) mu += points[i] / float(points.size());

		// loop over the points again to build the 
		// covariance matrix.  Note that we only have
		// to build terms for the upper trianglular 
		// portion since the matrix is symmetric
		float cxx = 0.0,
			cxy = 0.0,
			cxz = 0.0,
			cyy = 0.0,
			cyz = 0.0,
			czz = 0.0;

		for (int i = 0; i < (int)points.size(); i++)
		{
			const vec3& p = points[i];
			cxx += p.x*p.x - mu.x*mu.x;
			cxy += p.x*p.y - mu.x*mu.y;
			cxz += p.x*p.z - mu.x*mu.z;
			cyy += p.y*p.y - mu.y*mu.y;
			cyz += p.y*p.z - mu.y*mu.z;
			czz += p.z*p.z - mu.z*mu.z;
		}

		// now build the covariance matrix
		C[0][0] = cxx; C[0][1] = cxy; C[0][2] = cxz;
		C[1][0] = cxy; C[1][1] = cyy; C[1][2] = cyz;
		C[2][0] = cxz; C[2][1] = cyz; C[2][2] = czz;

		// set the OBB parameters from the covariance matrix
		build_from_covariance_matrix(C, points);
	}

	void obb::build_from_points(const unsigned char* points,
		size_t pos_offset,
		size_t vertex_size,
		size_t n_points)

	{
		vec3 mu(0.0, 0.0, 0.0);
		mat3 C;

#define att_vertex(i) (*(const vec3*)(points+vertex_size*(i)+pos_offset))
		// loop over the points to find the mean point
		// location
		for (int i = 0; i < (int)n_points; i++)
		{
			mu += att_vertex(i) / float(n_points);
		}

		// loop over the points again to build the 
		// covariance matrix.  Note that we only have
		// to build terms for the upper trianglular 
		// portion since the matrix is symmetric
		float cxx = 0.0,
			cxy = 0.0,
			cxz = 0.0,
			cyy = 0.0,
			cyz = 0.0,
			czz = 0.0;

		for (int i = 0; i < (int)n_points; i++)
		{
			const vec3& p = att_vertex(i);
			cxx += p.x*p.x - mu.x*mu.x;
			cxy += p.x*p.y - mu.x*mu.y;
			cxz += p.x*p.z - mu.x*mu.z;
			cyy += p.y*p.y - mu.y*mu.y;
			cyz += p.y*p.z - mu.y*mu.z;
			czz += p.z*p.z - mu.z*mu.z;
		}
#undef att_vertex
		// now build the covariance matrix
		C[0][0] = cxx; C[0][1] = cxy; C[0][2] = cxz;
		C[1][0] = cxy; C[1][1] = cyy; C[1][2] = cyz;
		C[2][0] = cxz; C[2][1] = cyz; C[2][2] = czz;

		// set the OBB parameters from the covariance matrix
		build_from_covariance_matrix(C, points, pos_offset, vertex_size, n_points);
	}

	float obb::volume() const
	{
		return 8 * m_extension[0] * m_extension[1] * m_extension[2];
	}

	void obb::get_bounding_box(std::vector< vec3 >& p) const
	{
		p.resize(8);
		vec3 r(m_rotation[0][0], m_rotation[1][0], m_rotation[2][0]);
		vec3 u(m_rotation[0][1], m_rotation[1][1], m_rotation[2][1]);
		vec3 f(m_rotation[0][2], m_rotation[1][2], m_rotation[2][2]);
		p[0] = m_position - r*m_extension[0] - u*m_extension[1] - f*m_extension[2];
		p[1] = m_position + r*m_extension[0] - u*m_extension[1] - f*m_extension[2];
		p[2] = m_position + r*m_extension[0] - u*m_extension[1] + f*m_extension[2];
		p[3] = m_position - r*m_extension[0] - u*m_extension[1] + f*m_extension[2];
		p[4] = m_position - r*m_extension[0] + u*m_extension[1] - f*m_extension[2];
		p[5] = m_position + r*m_extension[0] + u*m_extension[1] - f*m_extension[2];
		p[6] = m_position + r*m_extension[0] + u*m_extension[1] + f*m_extension[2];
		p[7] = m_position - r*m_extension[0] + u*m_extension[1] + f*m_extension[2];
	}

	void obb::get_bounding_box(std::vector< vec3 >& p, const mat4& model) const
	{
		p.resize(8);
		vec3 r(m_rotation[0][0], m_rotation[1][0], m_rotation[2][0]);
		vec3 u(m_rotation[0][1], m_rotation[1][1], m_rotation[2][1]);
		vec3 f(m_rotation[0][2], m_rotation[1][2], m_rotation[2][2]);
		p[0] = m_position - r*m_extension[0] - u*m_extension[1] - f*m_extension[2];
		p[1] = m_position + r*m_extension[0] - u*m_extension[1] - f*m_extension[2];
		p[2] = m_position + r*m_extension[0] - u*m_extension[1] + f*m_extension[2];
		p[3] = m_position - r*m_extension[0] - u*m_extension[1] + f*m_extension[2];
		p[4] = m_position - r*m_extension[0] + u*m_extension[1] - f*m_extension[2];
		p[5] = m_position + r*m_extension[0] + u*m_extension[1] - f*m_extension[2];
		p[6] = m_position + r*m_extension[0] + u*m_extension[1] + f*m_extension[2];
		p[7] = m_position - r*m_extension[0] + u*m_extension[1] + f*m_extension[2];
		//mul by model matrix
		for (vec3& point : p)
		{
			point = (vec3)(model * vec4(point, 1.0));
		}
	}

	std::vector< vec3 > obb::get_bounding_box() const
	{
		std::vector< vec3 > p;
		get_bounding_box(p);
		return p;
	}

	std::vector< vec3 > obb::get_bounding_box(const mat4& model) const
	{
		std::vector< vec3 > p;
		get_bounding_box(p, model);
		return p;
	}

	//obb rot/trans
	void obb::applay(const mat4& model)
	{
		mat4
		transform = translate(mat4(1), m_position);
		transform*= mat4(m_rotation);
		//new box
		transform = model * transform;
#if 0
		//decompone
		vec3 scale;
		quat rotation;
		vec3 translation;
		vec3 skew;
		vec4 perspective;
		glm::decompose(transform, scale, rotation, translation, skew, perspective);
		//to obb
		m_position  = translation;
		m_rotation  = mat3_cast( rotation );
		m_extension = scale * m_extension;
#else
		//to obb
		m_position   = vec3(transform[3]);
		//rotation
		auto r_scale = mat3(transform);
		auto scale   = vec3(length(r_scale[0]),
							length(r_scale[1]),
							length(r_scale[2]));
		r_scale[0] /= scale[0];
		r_scale[1] /= scale[1];
		r_scale[2] /= scale[2];
		m_rotation = traspose(inverse(r_scale));
		//scale
		m_extension = scale * m_extension;
#endif
	}

	// The implementation of this function is from Christer Ericson's Real-Time Collision Detection, p.133.
	vec3 obb::closest_point(const vec3& target) const
	{
		// Best: 33.412 nsecs / 89.952 ticks, Avg: 33.804 nsecs, Worst: 34.180 nsecs
		vec3 d = target - m_position;
		// Start at the center point of the OBB.
		vec3 closest_point = m_position; 
		//axis
		const mat3& axis = transpose(m_rotation);
		// Project the target onto the OBB axes and walk towards that point.
		for (int i = 0; i < 3; ++i)
		{
			closest_point += clamp(dot(d, axis[i]), -m_extension[i], m_extension[i]) * axis[i];
		}

		return closest_point;
	}

	void obb::build_from_covariance_matrix(const mat3& C, const std::vector< vec3 >& points)
	{
		build_from_covariance_matrix(C, (const unsigned char*)points.data(), 0, sizeof(vec3), points.size());
	}

	void obb::build_from_covariance_matrix(const mat3& C, const std::vector< vec3 >& points, size_t size)
	{
		build_from_covariance_matrix(C, (const unsigned char*)points.data(), 0, sizeof(vec3), size);
	}

	void obb::build_from_covariance_matrix(
		mat3 C,
		const unsigned char* points,
		size_t pos_offset,
		size_t vertex_size,
		size_t size
	)
	{
		// extract the eigenvalues and eigenvectors from C
		mat3 eigvec;
		vec3 eigval = eigenvalues_jacobi(C, 100, eigvec);

		// find the right, up and forward vectors from the eigenvectors
		vec3  r(eigvec[0][0], eigvec[1][0], eigvec[2][0]);
		vec3  u(eigvec[0][1], eigvec[1][1], eigvec[2][1]);
		vec3  f(eigvec[0][2], eigvec[1][2], eigvec[2][2]);

		normalize(r);
		normalize(u);
		normalize(f);

		// set the rotation matrix using the eigvenvectors
		m_rotation[0][0] = r.x; m_rotation[0][1] = u.x; m_rotation[0][2] = f.x;
		m_rotation[1][0] = r.y; m_rotation[1][1] = u.y; m_rotation[1][2] = f.y;
		m_rotation[2][0] = r.z; m_rotation[2][1] = u.z; m_rotation[2][2] = f.z;

		// now build the bounding box extents in the rotated frame
		vec3 minim(1e10, 1e10, 1e10), maxim(-1e10, -1e10, -1e10);

		for (int i = 0; i < (int)size; i++)
		{
#define att_vertex(i) (*(const vec3*)(points+vertex_size*(i)+pos_offset))
			vec3 p_prime(dot(r, att_vertex(i)),
				dot(u, att_vertex(i)),
				dot(f, att_vertex(i)));
#undef att_vertex

			minim = min(minim, p_prime);
			maxim = max(maxim, p_prime);
		}

		// set the center of the OBB to be the average of the 
		// minimum and maximum, and the extents be half of the
		// difference between the minimum and maximum
		vec3 center = (maxim + minim) * 0.5f;
		m_position[0] = dot(m_rotation[0], center);
		m_position[1] = dot(m_rotation[1], center);
		m_position[2] = dot(m_rotation[2], center);
		m_extension = (maxim - minim) * 0.5f;
	}
}