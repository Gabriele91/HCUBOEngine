#pragma once
#include <string>
#include <smesh.h>
#include <filesystem.h>
#define SAVE_OBBS

static const char* mat_template[]
{
	//NO TEXTURE TEMPLATE
	"cullface true\n"
	"{\n"
	"	mode back\n"
	"}\n"
	"\n"
	"blend false\n"
	"\n"
	"shader \"%s\"\n"
	"{\n"
	"	uniforms\n"
	"	{\n"
    "       mask float(-1.0)\n"
	"		color vec4(%f,%f,%f,%f)\n"
	"	}\n"
	"}\n",
	//TEXTURE TEMPLATE
	"cullface true\n"
	"{\n"
	"	mode back\n"
	"}\n"
	"\n"
	"blend false\n"
	"\n"
	"shader \"%s\"\n"
	"{\n"
	"	textures\n"
	"	{\n"
	"		%s\n"
	"		%s\n"
	"		%s\n"
	"	}\n"
	"	\n"
	"	uniforms\n"
    "	{\n"
    "       mask float(-1.0)\n"
	"		color vec4(%f,%f,%f,%f)\n"
	"	}\n"
	"}\n"
};

void material::save(const std::string& path)
{
	//open file
	FILE* mat_file = fopen(path.c_str(), "wb");
	//test open
	if (!mat_file) return;
	//buffer size
	const size_t buffer_size = 2048;
	//write file
	char buffer[buffer_size] = { 0 };

	//shader name
	std::string name_shader;
	std::string diffuse_map(m_diffuse.size() ? "diffuse_map \"" + m_diffuse + "\"" : std::string());
	std::string specular_map(m_specular.size() ? "specular_map \"" + m_specular + "\"" : std::string());
	std::string normal_map(m_normal.size() ? "normal_map \"" + m_normal + "\"" : std::string());

	//select shader
	if (!m_diffuse.size())
		name_shader = "base";
	else if (!m_specular.size() && !m_normal.size())
		name_shader = "base_texture";
	else if (!m_specular.size())
		name_shader = "base_texture_normal";
	else if (!m_normal.size())
		name_shader = "base_texture_specular";
	else
		name_shader = "base_texture_normal_specular";

	if (name_shader == "base")
	{
		std::snprintf
		(
			buffer,
			buffer_size,
			mat_template[0],
			name_shader.c_str(),
			m_color.r,
			m_color.g,
			m_color.b,
			m_color.a
		);
	}
	else
	{
		std::snprintf
		(
			buffer,
			buffer_size,
			mat_template[1],
			name_shader.c_str(),
			diffuse_map.c_str(),
			specular_map.c_str(),
			normal_map.c_str(),
			m_color.r,
			m_color.g,
			m_color.b,
			m_color.a
		);
	}
	//write
	std::fwrite(buffer, std::strlen(buffer), 1, mat_file);
	//close file
	std::fclose(mat_file);
}

void model::save(const std::string& path)
{
	save(path, "");
}
void model::save(const std::string& path, const std::string& mat_prefix)
{
	//open file
	FILE* model_file = fopen(path.c_str(), "wb");
	//test open
	if (!model_file) return;

#ifdef SAVE_OBBS
	//version
	unsigned int version = 2;
	std::fwrite(&version, sizeof(unsigned int), 1, model_file);
#else
	//version
	unsigned int version = 1;
	std::fwrite(&version, sizeof(unsigned int), 1, model_file);
#endif 

	//n save geometry
	unsigned int size_nodes = m_nodes.size();
	std::fwrite(&size_nodes, sizeof(unsigned int), 1, model_file);
	//write
	for (auto& node : m_nodes)
	{
		//mat name
		std::string mat_name = mat_prefix+node.m_material.m_name;
		unsigned int size_mat_name = mat_name.size();
		std::fwrite(&size_mat_name, sizeof(unsigned int), 1, model_file);
		std::fwrite(mat_name.data(), sizeof(char)*size_mat_name, 1, model_file);
		//index
		unsigned int size_index = node.m_index.size();
		std::fwrite(&size_index, sizeof(unsigned int), 1, model_file);
		std::fwrite(node.m_index.data(), sizeof(unsigned int)*size_index, 1, model_file);
		//vertex
		unsigned int size_vertex = node.m_vertex.size();
		std::fwrite(&size_vertex, sizeof(unsigned int), 1, model_file);

		for (vertex& v : node.m_vertex)
		{
			std::fwrite(&v.m_position.x, sizeof(float), 3, model_file);
			std::fwrite(&v.m_normal.x, sizeof(float), 3, model_file);
			std::fwrite(&v.m_uvmap.x, sizeof(float), 2, model_file);
			std::fwrite(&v.m_tangent.x, sizeof(float), 3, model_file);
			std::fwrite(&v.m_bitangent.x, sizeof(float), 3, model_file);
		}
#ifdef SAVE_OBBS
		std::fwrite(&node.m_obb.get_rotation_matrix()[0].x, sizeof(float), 3, model_file);
		std::fwrite(&node.m_obb.get_rotation_matrix()[1].x, sizeof(float), 3, model_file);
		std::fwrite(&node.m_obb.get_rotation_matrix()[2].x, sizeof(float), 3, model_file);
		std::fwrite(&node.m_obb.get_position().x, sizeof(float), 3, model_file);
		std::fwrite(&node.m_obb.get_extension().x, sizeof(float), 3, model_file);
#endif
	}
	//close file
	std::fclose(model_file);
	//dir of model file
	std::string directory = filesystem::get_directory(path);
	//save materials
	for (auto& node : m_nodes)
	{
		std::string filename = mat_prefix + node.m_material.m_name + ".mat";
		node.m_material.save(directory + "/" + filename);
	}

}

void model::compute_obbs()
{
	for (size_t n = 0; n != m_nodes.size(); ++n)
	{
		m_nodes[n].m_obb.build_from_triangles
		(
			(const unsigned char*)m_nodes[n].m_vertex.data(),
			offsetof(vertex, m_position),
			sizeof(vertex),
			m_nodes[n].m_vertex.size(),
			m_nodes[n].m_index.data(),
			m_nodes[n].m_index.size()
		);
	}
}