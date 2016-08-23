#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <obb.h>

struct vertex
{
	glm::vec3 m_position;
	glm::vec3 m_normal;
	glm::vec2 m_uvmap;
	glm::vec3 m_tangent;
	glm::vec3 m_bitangent;
};

struct material
{
	std::string m_name;
	glm::vec4   m_color;
	std::string m_diffuse;
	std::string m_normal;
	std::string m_specular;
	void save(const std::string& path);
};

struct node
{
	material m_material;
	std::vector< unsigned int > m_index;
	std::vector< vertex > m_vertex;
	obb	m_obb;
};

struct model
{
	std::vector< node > m_nodes;
	void compute_obbs();
	void save(const std::string& path);
	void save(const std::string& path,const std::string& mat_prefix);
};