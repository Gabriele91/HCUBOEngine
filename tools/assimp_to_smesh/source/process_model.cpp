#pragma once
#include <iostream>
#include <smesh.h>
#include <filesystem.h>
#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


std::string texture_by_type(aiMaterial* mat, aiTextureType type)
{
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		return str.C_Str();
	}
	return "";
}

void process_mesh(aiMesh* mesh, const aiScene* scene, node& out_mesh)
{
	out_mesh.m_vertex.resize(mesh->mNumVertices);
	// add vertex
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		if (mesh->mVertices)
			out_mesh.m_vertex[i].m_position = glm::vec3
		{
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z,
		};

		if (mesh->mNormals)
			out_mesh.m_vertex[i].m_normal = glm::vec3
		{
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z,
		};

		if (mesh->mTextureCoords[0])
			out_mesh.m_vertex[i].m_uvmap = glm::vec2
		{
			mesh->mTextureCoords[0][i].x,
			mesh->mTextureCoords[0][i].y
		};

		if (mesh->mTangents)
			out_mesh.m_vertex[i].m_tangent = glm::vec3
		{
			mesh->mTangents[i].x,
			mesh->mTangents[i].y,
			mesh->mTangents[i].z,
		};

		if (mesh->mBitangents)
			out_mesh.m_vertex[i].m_bitangent = glm::vec3
		{
			mesh->mBitangents[i].x,
			mesh->mBitangents[i].y,
			mesh->mBitangents[i].z,
		};
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) out_mesh.m_index.push_back(face.mIndices[j]);
	}


	// Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		aiString  name;
		aiColor4D color(1.f, 1.f, 1.f, 1.f);
		material->Get(AI_MATKEY_NAME, name);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		out_mesh.m_material.m_name = name.C_Str();
		out_mesh.m_material.m_color = { color.r, color.g, color.b, color.a };
		out_mesh.m_material.m_diffuse = filesystem::get_basename(texture_by_type(material, aiTextureType_DIFFUSE));
		out_mesh.m_material.m_specular = filesystem::get_basename(texture_by_type(material, aiTextureType_SPECULAR));
		out_mesh.m_material.m_normal = filesystem::get_basename(texture_by_type(material, aiTextureType_NORMALS));
	}
}

void process_node(aiNode* node, const aiScene* scene, model& out_model)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		out_model.m_nodes.resize(out_model.m_nodes.size() + 1);
		process_mesh(mesh, scene, out_model.m_nodes[out_model.m_nodes.size() - 1]);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene, out_model);
	}
}

extern bool compute_tangent_space(model& m);

void process_model(const std::string& path, model& out_model)
{
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate
	//| aiProcess_FlipUVs
	//| aiProcess_CalcTangentSpace
	);
	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// Process ASSIMP's root node recursively
	process_node(scene->mRootNode, scene, out_model);
	// Compute tangent space
	if (!compute_tangent_space(out_model))
	{
		std::cout << "ERROR::COMPUTE::TANGENT::SPACE " << "texture coordinates include NAN" << std::endl;
	}
	//save obj
#if 0
	Assimp::Exporter exporter;
	const aiExportFormatDesc* format = exporter.GetExportFormatDescription(0);
	exporter.Export(scene, "obj", "test.obj");
	std::cout << exporter.GetErrorString() << std::endl;
#endif
}