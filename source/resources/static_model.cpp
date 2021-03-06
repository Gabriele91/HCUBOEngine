//
//  static_model.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <hcube/math/tangent_space_calculation.h>
#include <hcube/core/resources_manager.h>
#include <hcube/render/render.h>
#include <hcube/components/mesh.h>
#include <hcube/resources/static_model.h>
#include <hcube/utilities/gameobject.h>


namespace hcube
{

	entity::ptr static_model::instantiate()
	{
		//make root
		entity::ptr root = gameobject::node_new();
		//add childs
		for (sub_model& model : m_sub_models)
		{
			root->add_child(gameobject::node_new(model.m_mesh));
		}
		return root;
	}

	bool static_model::load(resources_manager& resources, const std::string& path)
	{
		//struct of a vertex
		struct vertex
		{
			vec3 m_position;
			vec3 m_normal;
			vec2 m_uvmap;
			vec3 m_tangent;
			vec3 m_bitangent;
		};
		//attrs list
		attribute_list attribute_list
		{
			sizeof(vertex),
			{
				attribute{ ATT_POSITIONT, AST_FLOAT3, offsetof(vertex, m_position) },
				attribute{ ATT_NORMAL0,   AST_FLOAT3, offsetof(vertex, m_normal)   },
				attribute{ ATT_TEXCOORD0, AST_FLOAT2, offsetof(vertex, m_uvmap)    },
				attribute{ ATT_TANGENT0,  AST_FLOAT3, offsetof(vertex, m_tangent)  },
				attribute{ ATT_BINORMAL0, AST_FLOAT3, offsetof(vertex, m_bitangent)}
			}
		};
		//mesh layout
		mesh::mesh_layout layout
		{
			render::create_IL(attribute_list),
			DRAW_TRIANGLES
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
		for (unsigned int i = 0; i != size_nodes; ++i)
		{
			//mat name len
			unsigned int size_mat_name = 0;
			std::fread(&size_mat_name, sizeof(unsigned int), 1, model_file);
			//mat name
			std::string mat_name((size_t)size_mat_name, '\0');
			std::fread(&mat_name[0], sizeof(char)*size_mat_name, 1, model_file);
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
			//create mesh
			m_sub_models[i].m_mesh = mesh::snew();
			m_sub_models[i].m_mesh->build(
				layout,
				ibuffer.data(),
				ibuffer.size(),
				(const mesh::byte*)vertices.data(),
				vertices.size() * sizeof(vertex),
				//compute OBB is if versione 1
				version == 1
			);
			//set material
			m_sub_models[i].m_mesh->set_material(resources.get_material(mat_name));
			//is versione 2? Load OBB
			if (version == 2)
			{
				mat3 obb_rot;
				vec3 obb_pos;
				vec3 obb_ext;
				std::fread(&obb_rot[0].x, sizeof(float), 3, model_file);
				std::fread(&obb_rot[1].x, sizeof(float), 3, model_file);
				std::fread(&obb_rot[2].x, sizeof(float), 3, model_file);
				std::fread(&obb_pos.x, sizeof(float), 3, model_file);
				std::fread(&obb_ext.x, sizeof(float), 3, model_file);
				m_sub_models[i].m_mesh->set_bounding_box(obb(obb_rot, obb_pos, obb_ext));
			}
		}
		std::fclose(model_file);

		return true;
	}

	prefab_ptr static_model::copy() const
	{
		auto ostatic_model = static_model::snew();
		//alloc
		ostatic_model->m_sub_models.resize(m_sub_models.size());
		//read and copy all
		for (unsigned int i = 0; i != m_sub_models.size(); ++i)
		{
			ostatic_model->m_sub_models[i].m_mesh = std::static_pointer_cast<mesh>(m_sub_models[i].m_mesh->copy());
		}
		return std::static_pointer_cast<prefab>(ostatic_model);
	}
}