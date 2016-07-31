#pragma once
#include <string>
#include <smesh.h>
#include <filesystem.h>
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
	//open file
	FILE* model_file = fopen(path.c_str(), "wb");
	//test open
	if (!model_file) return;
	//version
	unsigned int version = 1;
	std::fwrite(&version, sizeof(unsigned int), 1, model_file);
	//n save geometry
	unsigned int size_nodes = m_nodes.size();
	std::fwrite(&size_nodes, sizeof(unsigned int), 1, model_file);
	//write
	for (auto& node : m_nodes)
	{
		//mat name
		unsigned int size_mat_name = node.m_material.m_name.size();
		std::fwrite(&size_mat_name, sizeof(unsigned int), 1, model_file);
		std::fwrite(node.m_material.m_name.data(), sizeof(char)*size_mat_name, 1, model_file);
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
	}
	//close file
	std::fclose(model_file);
	//dir of model file
	std::string directory = filesystem::get_directory(path);
	//save materials
	for (auto& node : m_nodes)
	{
		std::string filename = node.m_material.m_name + ".mat";
		node.m_material.save(directory + "/" + filename);
	}

}