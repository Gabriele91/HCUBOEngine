//
//  resources.cpp
//  HCubo
//
//  Created by Gabriele on 12/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <iostream>
#include <hcube/core/resource.h>
#include <hcube/core/resources_manager.h>
#include <hcube/core/filesystem.h>
#include <hcube/resources/static_model.h>

static const std::string void_path_file;

namespace hcube
{

	#pragma region "Parser"

	#define CSTRCMP( x, y ) ( std::strncmp( x, y, sizeof(y)-1 ) == 0 )
	#define CSTRCMP_SKIP( x, y ) ( [&x] () -> bool { if ( CSTRCMP(x,y) ) { x += sizeof(y)-1; return true; } return false; } )()

	class HCUBE_API resources_parser
	{
	public:

		struct path_field
		{
			std::string m_path;
			std::string m_reg_exp;
			bool		m_recursive{ false };
			bool		m_filtered { false };
		};

		struct file_field
		{
			std::string m_path;
			std::string m_asset_name;
			bool		m_use_asset_name { false };
		};

		struct error_field
		{
			size_t m_line{ 0 };
			std::string m_error;

			error_field()
			{
			}

			error_field
			(
			size_t line,
			const std::string& error
			)
			: m_line(line)
			, m_error(error)
			{
			}
		};

		struct context
		{
			std::vector< path_field > m_paths;
			std::vector< file_field > m_files;

			std::list < error_field >	    m_errors;
			size_t						    m_line{ 0 };

			std::string errors_to_string() const
			{
				std::stringstream sbuffer;

				for (auto& error : m_errors)
				{
					sbuffer << "Error:" << error.m_line << ": " << error.m_error << "\n";
				}

				return sbuffer.str();
			}
		};

		bool parse(context& default_context, const std::string& source)
		{
			const char* c_ptr = source.c_str();
			return parse(default_context, c_ptr);
		}

		bool parse(context& default_context, const char*& ptr)
		{
			m_context = &default_context;
			//restart
			m_context->m_paths.clear();
			m_context->m_files.clear();
			m_context->m_errors.clear();
			m_context->m_line = 1;
			//get type
			while (*ptr != EOF && *ptr != '\0')
			{
				//skeep line and comments
				skeep_space_end_comment(ptr);
				//parsing a block
				if (CSTRCMP_SKIP(ptr, "resources")) { if (!parse_resources_block(ptr)) return false; }
				else { push_error("Not found a valid command"); return false; }
				//skeep line and comments
				skeep_space_end_comment(ptr);
			}
			return true;
		}

	protected:

		context* m_context{ nullptr };
		//////////////////////////////////////////////////////
		bool parse_resources_block(const char*& ptr)
		{
			//skeep spaces
			skeep_space_end_comment(ptr);
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				//skeep spaces
				skeep_space_end_comment(ptr);
				//read all values
				while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
				{
					//alloc uniform field
					if (CSTRCMP_SKIP(ptr, "path"))
					{
						path_field n_path_field;
						//skeep spaces
						skeep_space_end_comment(ptr);
						//parse textures
						if (!parse_cstring(ptr, &ptr, n_path_field.m_path))
						{
							push_error("Not valid path field");
							return false;
						}
						//test, is recursive
						n_path_field.m_recursive =
							filesystem::get_filename(n_path_field.m_path) == "**";
						//only dir
						if (n_path_field.m_recursive)
						{
							n_path_field.m_path =
								filesystem::get_directory(n_path_field.m_path);
						}
						//skeep spaces
						skeep_space_end_comment(ptr);
						//test, name?
						if (CSTRCMP_SKIP(ptr, "filter"))
						{
							//skeep spaces
							skeep_space_end_comment(ptr);
							//parse textures
							if (!parse_cstring(ptr, &ptr, n_path_field.m_reg_exp))
							{
								push_error("Not valid filter field");
								return false;
							}
							//enable
							n_path_field.m_filtered = true;
							//skeep spaces
							skeep_space_end_comment(ptr);

						}
						//skeep spaces
						skeep_space_end_comment(ptr);
						//add
						m_context->m_paths.push_back(n_path_field);
					}
					else if (CSTRCMP_SKIP(ptr, "file"))
					{
						file_field n_file_field;
						//skeep spaces
						skeep_space_end_comment(ptr);
						//parse textures
						if (!parse_cstring(ptr, &ptr, n_file_field.m_path))
						{
							push_error("Not valid path field");
							return false;
						}
						//skeep spaces
						skeep_space_end_comment(ptr);
						//test, name?
						if (CSTRCMP_SKIP(ptr, "name"))
						{
							//skeep spaces
							skeep_space_end_comment(ptr);
							//parse textures
							if (!parse_cstring(ptr, &ptr, n_file_field.m_asset_name))
							{
								push_error("Not valid name field");
								return false;
							}
							//enable
							n_file_field.m_asset_name = true;
							//skeep spaces
							skeep_space_end_comment(ptr);

						}
						//add
						m_context->m_files.push_back(n_file_field);
					}
					else
					{
						push_error("Not valid resource field");
						return false;
					}
					//skeep spaces
					skeep_space_end_comment(ptr);
				}
				//end while
				if (!is_end_table(*ptr))
				{
					push_error("Not found }");
					return false;
				}
				//skip }
				++ptr;
			}
			return true;
		}
		//////////////////////////////////////////////////////
		static bool is_line_space(char c)
		{
			return 	 c == ' ' || c == '\t';
		}

		static bool is_space(char c)
		{
			return 	 c == ' ' || c == '\t' || c == '\r' || c == '\n';
		}

		static bool is_start_name(char c)
		{
			return 	 (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
		}

		static bool is_char_name(char c)
		{
			return 	 (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
		}

		static bool is_uint_number(char c)
		{
			return 	 (c >= '0' && c <= '9');
		}

		static bool is_start_int_number(char c)
		{
			return 	 (c >= '0' && c <= '9') || c == '-';
		}

		static bool is_start_float_number(char c)
		{
			return 	 (c >= '0' && c <= '9') || c == '-' || c == '.';
		}

		static bool is_start_string(char c)
		{
			return c == '\"';
		}

		static bool is_start_table(char c)
		{
			return (c == '{');
		}

		static bool is_end_table(char c)
		{
			return (c == '}');
		}

		static bool is_start_arg(char c)
		{
			return (c == '(');
		}

		static bool is_comm_arg(char c)
		{
			return (c == ',');
		}

		static bool is_end_arg(char c)
		{
			return (c == ')');
		}

		static bool is_start_obj_list(char c)
		{
			return (c == '|');
		}

		static bool is_line_comment(const char* c)
		{
			return (*c) == '/' && (*(c + 1)) == '/';
		}

		static bool is_start_multy_line_comment(const char* c)
		{
			return (*c) == '/' && (*(c + 1)) == '*';
		}

		static bool is_end_multy_line_comment(const char* c)
		{
			return (*c) == '*' && (*(c + 1)) == '/';
		}

		void skeep_line_comment(const char*& inout)
		{
			if (is_line_comment(inout))
			{
				while (*(inout) != EOF &&
					*(inout) != '\0'&&
					*(inout) != '\n')
					++(inout);
			}
		}

		void skeep_multy_line_comment(const char*& inout)
		{
			if (is_start_multy_line_comment(inout))
			{
				while (*(inout) != EOF &&
					*(inout) != '\0' &&
					!is_end_multy_line_comment(inout))
				{
					m_context->m_line += (*(inout)) == '\n';
					++(inout);
				}
				if ((*(inout)) == '*') ++(inout);
			}
		}

		void skeep_space_end_comment(const char*& inout)
		{
			while (is_space(*(inout)) ||
				is_line_comment(inout) ||
				is_start_multy_line_comment(inout))
			{
				skeep_line_comment(inout);
				skeep_multy_line_comment(inout);
				m_context->m_line += (*(inout)) == '\n';
				++(inout);
			}
		}

		void skeep_line_space(const char*& inout)
		{
			while (is_line_space(*inout)) ++(inout);
		}
		//////////////////////////////////////////////////////
		bool parse_bool(const char* in, const char** cout, bool& out)
		{
			if (CSTRCMP(in, "true")) { (*cout) += 4; out = true;  return true; }
			if (CSTRCMP(in, "false")) { (*cout) += 5; out = false; return true; };
			return false;
		}

		bool parse_int(const char* in, const char** cout, int& out)
		{
			out = (int)std::strtod(in, (char**)cout);
			return in != (*cout);
		}

		bool parse_float(const char* in, const char** cout, float& out)
		{
			out = (float)std::strtod(in, (char**)cout);
			return in != (*cout);
		}

		bool parse_name(const char* in, const char** cout, std::string& out)
		{
			if (!is_start_name(*in)) return false;
			out = "";
			out += *in;
			++in;
			while (is_char_name(*in))
			{
				out += *in;
				++in;
			}
			(*cout) = in;
			return true;
		}

		bool parse_cstring(const char* in, const char** cout, std::string& out)
		{
			const char *tmp = in;
			out = "";
			//start parse
			if ((*tmp) == '\"')  //["...."]
			{
				++tmp;  //[...."]
				while ((*tmp) != '\"' && (*tmp) != '\n')
				{
					if ((*tmp) == '\\') //[\.]
					{
						++tmp;  //[.]
						switch (*tmp)
						{
						case 'n':
							out += '\n';
							break;
						case 't':
							out += '\t';
							break;
						case 'b':
							out += '\b';
							break;
						case 'r':
							out += '\r';
							break;
						case 'f':
							out += '\f';
							break;
						case 'a':
							out += '\a';
							break;
						case '\\':
							out += '\\';
							break;
						case '?':
							out += '\?';
							break;
						case '\'':
							out += '\'';
							break;
						case '\"':
							out += '\"';
							break;
						case '\n': /* jump unix */
							++m_context->m_line;
							break;
						case '\r': /* jump mac */
							++m_context->m_line;
							if ((*(tmp + 1)) == '\n') ++tmp; /* jump window (\r\n)*/
							break;
						default:
							return true;
							break;
						}
					}
					else
					{
						if ((*tmp) != '\0') out += (*tmp);
						else return false;
					}
					++tmp;//next char
				}
				if ((*tmp) == '\n') return false;
				if (cout) (*cout) = tmp + 1;
				return true;
			}
			return false;
		}
		//////////////////////////////////////////////////////
		void push_error(const std::string& error)
		{
			m_context->m_errors.push_front(error_field{ m_context->m_line,error });
		}

	};
	#pragma endregion

	bool resources_manager::add_resources_file(const std::string& path)
	{
		//parser
		resources_parser		  r_parser;
		resources_parser::context r_context;
		//do parsing
		if (!r_parser.parse(r_context, filesystem::text_file_read_all(path)))
		{
			std::cout 
				<< "Resources file: "
				<< path
				<< std::endl
				<< r_context.errors_to_string()
				<< std::endl;
			return false;
		}
		//add all paths
		for (const resources_parser::path_field& path : r_context.m_paths)
		{
			if (path.m_filtered) add_directory(path.m_path, path.m_reg_exp, path.m_recursive);
			else				 add_directory(path.m_path, path.m_recursive);
		}
		//add all files
		for (const resources_parser::file_field& files : r_context.m_files)
		{
			if (files.m_use_asset_name) add_file(files.m_asset_name, files.m_path);
			else				        add_file(files.m_path);
		}
		//end
		return true;
	}
	
	void resources_manager::add_file(const std::string& path)
	{
		std::string directory = filesystem::get_directory(path);
		std::string filename = filesystem::get_filename(path);
		std::string basename = filesystem::get_basename(filename);
		//add
		add_file(basename, directory, filename);
	}

	void resources_manager::add_file(const std::string& asset_name, const std::string& path)
	{
		std::string directory = filesystem::get_directory(path);
		std::string filename  = filesystem::get_filename(path);
		//add
		add_file(asset_name,directory, filename);
	}

	void resources_manager::add_directory(const std::string& directory, bool recursive)
	{
		//get all files
		filesystem::files_list files = filesystem::get_files(directory);
		//success?
		if (!files.m_success) return;
		//put into table
		for (const std::string& filename : files.m_fields)
		{
			//get asset name
			std::string basename = filesystem::get_basename(filename);
			//add
			add_file(basename, directory, filename);
		}
		//sub directories
		if (recursive)
		{
			//get all directories
			filesystem::directories_list directories = filesystem::get_sub_directories(directory);
			//success?
			if (!directories.m_success) return;
			//push dir into table
			for (const std::string& directoryname : directories.m_fields)
			{
				add_directory(directory + "/" + directoryname, true);
			}
		}
	}

	void resources_manager::add_directory(const std::string& directory, const std::string& filter, bool recursive)
	{
		//get all files
		filesystem::files_list files = filesystem::get_files(directory);
		//success?
		if (!files.m_success) return;
		//put into table
		try
		{
			std::regex reg_exp(filter, std::regex::ECMAScript);
			add_directory(directory, reg_exp, recursive);
		}
		catch (std::regex_error& e)
		{
			std::cout << "Resources filter error:" << std::endl;
			std::cout << e.what() << std::endl; 
		}
	}
	
	void resources_manager::add_file
	(
		const std::string& asset_name,
		const std::string& directory,
		const std::string& filename
	)
	{
		//file ext
		std::string ext = filesystem::get_extension(filename);
		//types
		if (   ext == ".png"
			|| ext == ".jpg"
			|| ext == ".jpeg"
			|| ext == ".tga")
		{
			set_texture_path(asset_name, directory + "/" + filename);
		}
		else if (ext == ".glsl")
		{
			set_shader_path(asset_name, directory + "/" + filename);
		}
		else if (ext == ".fxhc")
		{
			set_effect_path(asset_name, directory + "/" + filename);
		}
		else if (ext == ".mat")
		{
			set_material_path(asset_name, directory + "/" + filename);
		}
		else if (ext == ".smhc")
		{
			set_prefab_path(asset_name, directory + "/" + filename);
		}
		else if (ext == ".json")
		{
			//none
		}
	}

	void resources_manager::add_directory
	(
		const std::string& directory,
		const std::regex& filter,
		bool recursive
	)
	{
		//get all files
		filesystem::files_list files = filesystem::get_files(directory);
		//success?
		if (!files.m_success) return;
		//put into table
		for (const std::string& filename : files.m_fields)
		{
			if (std::regex_match(filename, filter))
			{
				//get asset name
				std::string basename = filesystem::get_basename(filename);
				//add
				add_file(basename, directory, filename);
			}
		}
		//sub directories
		if (recursive)
		{
			//get all directories
			filesystem::directories_list directories = filesystem::get_sub_directories(directory);
			//success?
			if (!directories.m_success) return;
			//push dir into table
			for (const std::string& directoryname : directories.m_fields)
			{
				add_directory(directory + "/" + directoryname, filter, true);
			}
		}
	}

	void resources_manager::set_shader_path(const std::string& name, const std::string& path)
	{
		//shader name
		std::string resource_name = "shader:" + name;
		//set path
		m_resources_path_map[resource_name] = path;
	}

	void resources_manager::set_effect_path(const std::string& name, const std::string& path)
	{
		//shader name
		std::string resource_name = "effect:" + name;
		//set path
		m_resources_path_map[resource_name] = path;
	}

	void resources_manager::set_texture_path(const std::string& name, const std::string& path)
	{
		//shader name
		std::string resource_name = "texture:" + name;
		//set path
		m_resources_path_map[resource_name] = path;

	}

	void resources_manager::set_material_path(const std::string& name, const std::string& path)
	{
		//shader name
		std::string resource_name = "material:" + name;
		//set path
		m_resources_path_map[resource_name] = path;
	}

	void resources_manager::set_prefab_path(const std::string& name, const std::string& path)
	{
		//shader name
		std::string resource_name = "prefab:" + name;
		//set path
		m_resources_path_map[resource_name] = path;
	}

	const std::string& resources_manager::get_shader_path(const std::string& name)
	{
		auto it_path = m_resources_path_map.find("shader:" + name);
		if (it_path != m_resources_path_map.end()) return it_path->second;
		return void_path_file;
	}
	const std::string& resources_manager::get_effect_path(const std::string& name)
	{
		auto it_path = m_resources_path_map.find("effect:" + name);
		if (it_path != m_resources_path_map.end()) return it_path->second;
		return void_path_file;
	}
	const std::string& resources_manager::get_texture_path(const std::string& name)
	{
		auto it_path = m_resources_path_map.find("texture:" + name);
		if (it_path != m_resources_path_map.end()) return it_path->second;
		return void_path_file;
	}
	const std::string& resources_manager::get_material_path(const std::string& name)
	{
		auto it_path = m_resources_path_map.find("material:" + name);
		if (it_path != m_resources_path_map.end()) return it_path->second;
		return void_path_file;
	}
	const std::string& resources_manager::get_prefab_path(const std::string& name)
	{
		auto it_path = m_resources_path_map.find("prefab:" + name);
		if (it_path != m_resources_path_map.end()) return it_path->second;
		return void_path_file;
	}

	shader::ptr resources_manager::get_shader(const std::string& name)
	{
		//shader name
		std::string shader_name = "shader:" + name;
		//rerouce
		resources_map_it resource_it = m_resources_map.find(shader_name);
		//alloc
		if (resource_it == m_resources_map.end())
		{
			m_resources_map[shader_name] = std::static_pointer_cast<resource>(shader::snew());
			resource_it = m_resources_map.find(shader_name);
			resource_it->second->load(*this, m_resources_path_map[shader_name]);
		}
		//return
		return std::static_pointer_cast<shader>(resource_it->second);
	}

	effect::ptr resources_manager::get_effect(const std::string& name)
	{
		//shader name
		std::string effect_name = "effect:" + name;
		//rerouce
		resources_map_it resource_it = m_resources_map.find(effect_name);
		//alloc
		if (resource_it == m_resources_map.end())
		{
			m_resources_map[effect_name] = std::static_pointer_cast<effect>(effect::snew());
			resource_it = m_resources_map.find(effect_name);
			resource_it->second->load(*this, m_resources_path_map[effect_name]);
		}
		//return
		return std::static_pointer_cast<effect>(resource_it->second);
	}

	texture::ptr resources_manager::get_texture(const std::string& name)
	{
		//shader name
		std::string texture_name = "texture:" + name;
		//rerouce
		resources_map_it resource_it = m_resources_map.find(texture_name);
		//alloc
		if (resource_it == m_resources_map.end())
		{
			m_resources_map[texture_name] = std::static_pointer_cast<resource>(texture::snew());
			resource_it = m_resources_map.find(texture_name);
			resource_it->second->load(*this, m_resources_path_map[texture_name]);
		}
		//return
		return std::static_pointer_cast<texture>(resource_it->second);
	}

	material_ptr resources_manager::get_material(const std::string& name)
	{
		//shader name
		std::string material_name = "material:" + name;
		//rerouce
		resources_map_it resource_it = m_resources_map.find(material_name);
		//alloc
		if (resource_it == m_resources_map.end())
		{
			m_resources_map[material_name] = std::static_pointer_cast<resource>(material_snew());
			resource_it = m_resources_map.find(material_name);
			resource_it->second->load(*this, m_resources_path_map[material_name]);
		}
		//return
		return std::static_pointer_cast<material>(resource_it->second);
	}

	prefab_ptr resources_manager::get_prefab(const std::string& name)
	{
		//shader name
		std::string static_model_name = "prefab:" + name;
		//rerouce
		resources_map_it resource_it = m_resources_map.find(static_model_name);
		//alloc
		if (resource_it == m_resources_map.end())
		{
			m_resources_map[static_model_name] = std::static_pointer_cast<resource>(static_model::snew());
			resource_it = m_resources_map.find(static_model_name);
			resource_it->second->load(*this, m_resources_path_map[static_model_name]);
		}
		//return
		return std::static_pointer_cast<static_model>(resource_it->second);
	}
}