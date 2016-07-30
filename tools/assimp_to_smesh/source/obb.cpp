#pragma once
#include <obb.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <eigenvalues_jacobi.h>


void obb::build_from_sequenzial_triangles(std::vector< glm::vec3 > &points)
{
	float Ai = 0.0;
	float Am = 0.0;
	glm::vec3 mu(0.0f, 0.0f, 0.0f), mui;
	glm::mat3 C;
	float cxx = 0.0, cxy = 0.0, cxz = 0.0, cyy = 0.0, cyz = 0.0, czz = 0.0;

	// loop over the triangles this time to find the
	// mean location
	for (int i = 0; i<(int)points.size(); i += 3)
	{
		glm::vec3 &p = points[i + 0];
		glm::vec3 &q = points[i + 1];
		glm::vec3 &r = points[i + 2];
		mui = (p + q + r) / 3.0f;

		//compute len( triangle  dir/norm )
		Ai = glm::length(glm::cross((q - p), (r - p))) / 2.0f;

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

void obb::build_from_triangles(std::vector< glm::vec3 > &points, std::vector<int> &triangles)
{
	float Ai = 0.0;
	float Am = 0.0;
	glm::vec3 mu(0.0f, 0.0f, 0.0f), mui;
	glm::mat3 C;
	float cxx = 0.0, cxy = 0.0, cxz = 0.0, cyy = 0.0, cyz = 0.0, czz = 0.0;

	// loop over the triangles this time to find the
	// mean location
	for (int i = 0; i<(int)triangles.size(); i += 3)
	{
		glm::vec3 &p = points[triangles[i + 0]];
		glm::vec3 &q = points[triangles[i + 1]];
		glm::vec3 &r = points[triangles[i + 2]];
		mui = (p + q + r) / 3.0f;

		//compute len( triangle  dir/norm )
		Ai = glm::length(glm::cross((q - p), (r - p))) / 2.0f;

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

void obb::build_from_points(std::vector< glm::vec3 >& points)
{
	glm::vec3 mu(0.0, 0.0, 0.0);
	glm::mat3 C;

	// loop over the points to find the mean point
	// location
	for (int i = 0; i<(int)points.size(); i++) mu += points[i] / float(points.size());

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

	for (int i = 0; i<(int)points.size(); i++)
	{
		glm::vec3 &p = points[i];
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

float obb::volume() const
{
	return 8 * m_extension[0] * m_extension[1] * m_extension[2];
}

std::vector< glm::vec3 > obb::get_bounding_box() const
{
	std::vector< glm::vec3 > p;
	get_bounding_box(p);
	return p;
}

void obb::get_bounding_box(std::vector< glm::vec3 >& p) const
{
	p.resize(8);
	glm::vec3 r(m_rotation[0][0], m_rotation[1][0], m_rotation[2][0]);
	glm::vec3 u(m_rotation[0][1], m_rotation[1][1], m_rotation[2][1]);
	glm::vec3 f(m_rotation[0][2], m_rotation[1][2], m_rotation[2][2]);
	p[0] = m_position - r*m_extension[0] - u*m_extension[1] - f*m_extension[2];
	p[1] = m_position + r*m_extension[0] - u*m_extension[1] - f*m_extension[2];
	p[2] = m_position + r*m_extension[0] - u*m_extension[1] + f*m_extension[2];
	p[3] = m_position - r*m_extension[0] - u*m_extension[1] + f*m_extension[2];
	p[4] = m_position - r*m_extension[0] + u*m_extension[1] - f*m_extension[2];
	p[5] = m_position + r*m_extension[0] + u*m_extension[1] - f*m_extension[2];
	p[6] = m_position + r*m_extension[0] + u*m_extension[1] + f*m_extension[2];
	p[7] = m_position - r*m_extension[0] + u*m_extension[1] + f*m_extension[2];
}

void obb::build_from_covariance_matrix(glm::mat3 C, const std::vector< glm::vec3 >& points)
{
	// extract the eigenvalues and eigenvectors from C
	glm::mat3 eigvec;
	glm::vec3 eigval = glm::eigenvalues_jacobi(C, 100, eigvec);

	// find the right, up and forward vectors from the eigenvectors
	glm::vec3  r(eigvec[0][0], eigvec[1][0], eigvec[2][0]);
	glm::vec3  u(eigvec[0][1], eigvec[1][1], eigvec[2][1]);
	glm::vec3  f(eigvec[0][2], eigvec[1][2], eigvec[2][2]);

	glm::normalize(r);
	glm::normalize(u);
	glm::normalize(f);

	// set the rotation matrix using the eigvenvectors
	m_rotation[0][0] = r.x; m_rotation[0][1] = u.x; m_rotation[0][2] = f.x;
	m_rotation[1][0] = r.y; m_rotation[1][1] = u.y; m_rotation[1][2] = f.y;
	m_rotation[2][0] = r.z; m_rotation[2][1] = u.z; m_rotation[2][2] = f.z;

	// now build the bounding box extents in the rotated frame
	glm::vec3 minim(1e10, 1e10, 1e10), maxim(-1e10, -1e10, -1e10);

	for (int i = 0; i<(int)points.size(); i++)
	{
		glm::vec3 p_prime(glm::dot(r, points[i]),
			glm::dot(u, points[i]),
			glm::dot(f, points[i]));

		minim = glm::min(minim, p_prime);
		maxim = glm::max(maxim, p_prime);
	}

	// set the center of the OBB to be the average of the 
	// minimum and maximum, and the extents be half of the
	// difference between the minimum and maximum
	glm::vec3 center = (maxim + minim) * 0.5f;
	m_position[0] = glm::dot(m_rotation[0], center);
	m_position[1] = glm::dot(m_rotation[1], center);
	m_position[2] = glm::dot(m_rotation[2], center);
	m_extension = (maxim - minim) * 0.5f;
}