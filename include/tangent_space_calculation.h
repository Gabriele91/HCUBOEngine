#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

namespace tangent_space_calculation
{
	template < class T >
	void compute_tangent_fast(std::vector< T >& vertices)
	{
		//all to 0
		for (unsigned int i = 0; i < vertices.size(); ++i)
		{
			vertices[i].m_tangent   = { 0,0,0 };
			vertices[i].m_bitangent = { 0,0,0 };
		}
		//compute tangent per vertex
		for (unsigned int i = 0; i < vertices.size(); i += 3)
		{

			// Shortcuts for vertices
			glm::vec3 & v0 = vertices[i + 0].m_position;
			glm::vec3 & v1 = vertices[i + 1].m_position;
			glm::vec3 & v2 = vertices[i + 2].m_position;

			// Shortcuts for UVs
			glm::vec2 & uv0 = vertices[i + 0].m_uvmap;
			glm::vec2 & uv1 = vertices[i + 1].m_uvmap;
			glm::vec2 & uv2 = vertices[i + 2].m_uvmap;

			// Edges of the triangle : postion delta
			glm::vec3 edge1 = v1 - v0;
			glm::vec3 edge2 = v2 - v0;

			// UV delta
			glm::vec2 delta_uv1 = uv1 - uv0;
			glm::vec2 delta_uv2 = uv2 - uv0;

			//compute div
			float div = (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);

			//can compute tangent?
			if (div != 0.0)
			{
				//compute tanget & bitangent
				float r = 1.0f / div;
				glm::vec3 tangent = (edge1 * delta_uv2.y - edge2 * delta_uv1.y)*r;
				glm::vec3 bitangent = (edge2 * delta_uv1.x - edge1 * delta_uv2.x)*r;

				//add tangent
				vertices[i + 0].m_tangent += tangent;
				vertices[i + 1].m_tangent += tangent;
				vertices[i + 2].m_tangent += tangent;

				//add bitangent
				vertices[i + 0].m_bitangent += bitangent;
				vertices[i + 1].m_bitangent += bitangent;
				vertices[i + 2].m_bitangent += bitangent;
			}

		}

		// See "Going Further"
		for (unsigned int i = 0; i<vertices.size(); ++i)
		{
			glm::vec3& n = vertices[i].m_normal;
			glm::vec3& t = vertices[i].m_tangent;
			glm::vec3& b = vertices[i].m_bitangent;

			//normalize
			t = glm::normalize(t);
			b = glm::normalize(b);

			// Gram-Schmidt orthogonalize
			t = glm::normalize(t - n * glm::dot(n, t));

			// Calculate handedness
			if (glm::dot(glm::cross(n, t), b) < 0.0f)
			{
				t = t * -1.0f;
			}
		}
	}
	template < class T >
	void compute_tangent_fast(const std::vector<unsigned int>& indexes,  std::vector< T >& vertices)
	{
		for (unsigned int i = 0; i < indexes.size(); ++i)
		{
			vertices[indexes[i]].m_tangent = { 0,0,0 };
			vertices[indexes[i]].m_bitangent = { 0,0,0 };
		}
		//compute tangent per vertex
		for (unsigned int i = 0; i < indexes.size(); i += 3)
		{
			
			// Shortcuts for vertices
			glm::vec3 & v0 = vertices[indexes[i + 0]].m_position;
			glm::vec3 & v1 = vertices[indexes[i + 1]].m_position;
			glm::vec3 & v2 = vertices[indexes[i + 2]].m_position;

			// Shortcuts for UVs
			glm::vec2 & uv0 = vertices[indexes[i + 0]].m_uvmap;
			glm::vec2 & uv1 = vertices[indexes[i + 1]].m_uvmap;
			glm::vec2 & uv2 = vertices[indexes[i + 2]].m_uvmap;

			// Edges of the triangle : postion delta
			glm::vec3 edge1 = v1 - v0;
			glm::vec3 edge2 = v2 - v0;

			// UV delta
			glm::vec2 delta_uv1 = uv1 - uv0;
			glm::vec2 delta_uv2 = uv2 - uv0;

			//compute div
			float div = (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);

			//can compute tangent?
			if (div != 0.0)
			{
				//compute tanget & bitangent
				float r = 1.0f / div;
				glm::vec3 tangent   = (edge1 * delta_uv2.y - edge2 * delta_uv1.y)*r;
				glm::vec3 bitangent = (edge2 * delta_uv1.x - edge1 * delta_uv2.x)*r;

				//add tangent
				vertices[indexes[i + 0]].m_tangent += tangent;
				vertices[indexes[i + 1]].m_tangent += tangent;
				vertices[indexes[i + 2]].m_tangent += tangent;

				//add bitangent
				vertices[indexes[i + 0]].m_bitangent += bitangent;
				vertices[indexes[i + 1]].m_bitangent += bitangent;
				vertices[indexes[i + 2]].m_bitangent += bitangent;
			}
		}

		// See "Going Further"
		for (unsigned int i = 0; i<vertices.size(); ++i)
		{
			glm::vec3& n = vertices[i].m_normal;
			glm::vec3& t = vertices[i].m_tangent;
			glm::vec3& b = vertices[i].m_bitangent;

			//normalize
			t = glm::normalize(t);
			b = glm::normalize(b);

			// Gram-Schmidt orthogonalize
			t = glm::normalize(t - n * glm::dot(n, t));

			// Calculate handedness
			if (glm::dot(glm::cross(n, t), b) < 0.0f)
			{
				t = t * -1.0f;
			}
		}
	}
}