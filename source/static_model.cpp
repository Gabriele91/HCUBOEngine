//
//  static_model.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <mesh.h>
#include <resources_manager.h>
#include <static_model.h>
#include <tangent_space_calculation.h>


void static_model::draw(const glm::vec4& viewport,
                        const glm::mat4& projection_matrix,
                        const glm::mat4& view_matrix,
                        const glm::mat4& model_matrix,
                        material_ptr material)
{
	if (material)
	{
        for (sub_model& sub_model : m_sub_models)
        {
            sub_model.m_mesh->draw(viewport, projection_matrix, view_matrix, model_matrix, material);
        }
	}
	else
	{
        for (sub_model& model : m_sub_models)
        {
            model.m_mesh->draw(viewport, projection_matrix, view_matrix, model_matrix, model.m_material);
        }
	}
}

bool static_model::load(resources_manager& resources, const std::string& path)
{
	//struct of a vertex
	struct vertex
	{
		glm::vec3 m_position;
		glm::vec3 m_normal;
		glm::vec2 m_uvmap;
		glm::vec3 m_tangent;
		glm::vec3 m_bitangent;
	};
	//gpu layout
	mesh::mesh_layout layout
	{
		mesh::input_layout
		{
			mesh::input{ 0, sizeof(vertex), 3, offsetof(vertex, m_position) },
			mesh::input{ 1, sizeof(vertex), 3, offsetof(vertex, m_normal) },
			mesh::input{ 2, sizeof(vertex), 2, offsetof(vertex, m_uvmap) },
			mesh::input{ 3, sizeof(vertex), 3, offsetof(vertex, m_tangent) },
			mesh::input{ 4, sizeof(vertex), 3, offsetof(vertex, m_bitangent) }
		},
		GL_TRIANGLES,
		GL_STATIC_DRAW
	};
	//open file
	FILE* model_file = fopen(path.c_str(), "rb");
	//test open
	if (!model_file) return false;
	//version
	unsigned int version = 0;
	std::fread(&version, sizeof(unsigned int), 1, model_file);
	//n saved geometry
	unsigned int size_nodes = 0;
	std::fread(&size_nodes, sizeof(unsigned int), 1, model_file);
	//alloc
	m_sub_models.resize(size_nodes);
	//read all
	for (unsigned int i=0; i != size_nodes; ++i)
	{
		//mat name len
		unsigned int size_mat_name = 0;
		std::fread(&size_mat_name, sizeof(unsigned int), 1, model_file);
		//mat name
		std::string mat_name((size_t)size_mat_name, '\0');
		std::fread(&mat_name[0], sizeof(char)*size_mat_name, 1, model_file);
		//get material
		m_sub_models[i].m_material = resources.get_material(mat_name);
		//get index
		unsigned int size_index = 0;
		std::fread(&size_index, sizeof(unsigned int), 1, model_file);
		std::vector<unsigned int> ibuffer((size_t)size_index);
		std::fread(ibuffer.data(), sizeof(unsigned int), size_index, model_file);
		//get vertex
		unsigned int size_vertex = 0;
		std::fread(&size_vertex, sizeof(unsigned int), 1, model_file);
		std::vector<vertex> vertices((size_t)size_vertex);

		for (unsigned int v = 0; v != size_vertex; ++v)
		{
			std::fread(&vertices[v].m_position.x, sizeof(float), 3, model_file);
			std::fread(&vertices[v].m_normal.x, sizeof(float), 3, model_file);
			std::fread(&vertices[v].m_uvmap.x, sizeof(float), 2, model_file);
			std::fread(&vertices[v].m_tangent.x, sizeof(float), 3, model_file);
			std::fread(&vertices[v].m_bitangent.x, sizeof(float), 3, model_file);
		}

		//copy to buffer
		std::vector< mesh::byte > vbuffer(vertices.size() * sizeof(vertex));
		//copy buffer
		std::memcpy(&vbuffer[0], vertices.data(), vertices.size() * sizeof(vertex));
		//create mesh
		m_sub_models[i].m_mesh = mesh::snew();
		m_sub_models[i].m_mesh->build(layout, ibuffer, vbuffer);
	}
	std::fclose(model_file);

	return true;
}

component_ptr static_model::copy() const
{
	auto ostatic_model = static_model::snew();
	//alloc
	ostatic_model->m_sub_models.resize(m_sub_models.size());
	//read and copy all
	for (unsigned int i = 0; i != m_sub_models.size(); ++i)
	{
		ostatic_model->m_sub_models[i].m_material = std::static_pointer_cast<material>(m_sub_models[i].m_material->copy());
		ostatic_model->m_sub_models[i].m_mesh     = std::static_pointer_cast<mesh>(m_sub_models[i].m_mesh->copy());
	}
	return ostatic_model;
}