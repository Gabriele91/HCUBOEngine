#include <smesh.h>
#include <iostream>
#include <filesystem.h>


void material::save(const std::string& path)
{
	//open file
	FILE* mat_file = fopen(path.c_str(), "wb");
	//test open
	if (!mat_file) return;
	//write file
	char buffer[2048] = { 0 };

	std::snprintf(buffer,1024,
R"material(
	cullface true
	{
		mode back
	}

	blend false

	shader "base_texture_normal_specular"
	{
		textures
		{
			%s
			%s
			%s
		}

		uniforms
		{
			color vec4(%f,%f,%f,%f)
		}
	}
)material",
	(m_diffuse.size() ? "diffuse_map \"" + m_diffuse + "\"" : std::string()).c_str(),
	(m_specular.size() ? "specular_map \"" + m_specular + "\"" : std::string()).c_str(),
	(m_normal.size() ? "normal_map \"" + m_normal + "\"" : std::string()).c_str(),
	m_color.x,
	m_color.y,
	m_color.z);
	//write
	std::fwrite(buffer, std::strlen(buffer), 1, mat_file);
	//close file
	std::fclose(mat_file);
}

void model::save(const std::string& path)
{
	//open file
	FILE* model_file = fopen(path.c_str(),"wb");
	//test open
	if (!model_file) return;
	//version
	unsigned int version = 1;
	std::fwrite(&version, sizeof(unsigned int), 1, model_file);
	//n save geometry
	unsigned int size_nodes = m_nodes.size();
	std::fwrite(&size_nodes,sizeof(unsigned int),1,model_file);
	//write
	for (auto& node : m_nodes)
	{
		//mat name
		unsigned int size_mat_name = node.m_material.m_name.size();
		std::fwrite(&size_mat_name, sizeof(unsigned int), 1, model_file);
		std::fwrite(node.m_material.m_name.data(), sizeof(char)*size_mat_name, 1, model_file);
		//index
		unsigned int size_index= node.m_index.size();
		std::fwrite(&size_index, sizeof(unsigned int), 1, model_file);
		std::fwrite(node.m_index.data(), sizeof(unsigned int)*size_index, 1, model_file);
		//vertex
		unsigned int size_vertex = node.m_vertex.size();
		std::fwrite(&size_vertex, sizeof(unsigned int), 1, model_file);

		for (vertex& v : node.m_vertex)
		{
			std::fwrite(&v.m_position.x, sizeof(float), 3, model_file);
			std::fwrite(&v.m_normal.x,   sizeof(float), 3, model_file);
			std::fwrite(&v.m_uvmap.x,    sizeof(float), 2, model_file);
			std::fwrite(&v.m_tangent.x,  sizeof(float), 3, model_file);
			std::fwrite(&v.m_bitangent.x,sizeof(float), 3, model_file);
		}
	}
	//close file
	std::fclose(model_file);
	//dir of model file
	std::string directory = filesystem::get_directory(path);
	//save materials
	for (auto& node : m_nodes)
	{
		std::string filename  = node.m_material.m_name+".mat";
		node.m_material.save(directory + "/" + filename);
	}
    
}

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

		if(mesh->mNormals)
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
		out_mesh.m_material.m_name	   = name.C_Str();
		out_mesh.m_material.m_color    = { color.r, color.g, color.b, color.a };
		out_mesh.m_material.m_diffuse  = filesystem::get_basename(texture_by_type(material, aiTextureType_DIFFUSE));
		out_mesh.m_material.m_specular = filesystem::get_basename(texture_by_type(material, aiTextureType_SPECULAR));
		out_mesh.m_material.m_normal   = filesystem::get_basename(texture_by_type(material, aiTextureType_NORMALS));
	}
}

void process_node(aiNode* node, const aiScene* scene, model& out_model)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		out_model.m_nodes.resize(out_model.m_nodes.size() + 1);
		process_mesh(mesh, scene, out_model.m_nodes[out_model.m_nodes.size()-1]);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene, out_model);
	}
}   

void process_model(const std::string& path, model& out_model)
{
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, 
		aiProcess_Triangulate | 
	  //aiProcess_FlipUVs | 
		aiProcess_CalcTangentSpace);
	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// Process ASSIMP's root node recursively
	process_node(scene->mRootNode, scene, out_model);
}

int main(int argc,char* argv[])
{
	if(argc<3) return 1;
	//alloc
	model l_model;
	//load
	process_model(argv[1], l_model);
	//save
	l_model.save(argv[2]);
	//end
	return 0;
}