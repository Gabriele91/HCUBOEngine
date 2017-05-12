#include <string>
#include <sstream>
#include <vector>
#include <cstdio>
#include <iostream>
#include <hcube/math/vector_math.h>
#include <hcube/core/filesystem.h>
#include <hcube/render/render.h>
#include <hcube/render/shader.h>
#include <hcube/core/resources_manager.h>

namespace hcube
{

	#pragma region "Shader parser"

    enum vendor_error_type
    {
        VET_INTEL,
        VET_NVIDIA,
        VET_AMD,
        VET_UNKNOW
    };
    
    static vendor_error_type get_type_of_error()
    {
        std::string vendor = render::get_render_driver_info().m_name;
        std::transform(vendor.begin(), vendor.end(), vendor.begin(), ::tolower);
        if(vendor.find("intel")  != std::string::npos)  return VET_INTEL;
        if(vendor.find("amd")    != std::string::npos)    return VET_AMD;
        if(vendor.find("nvidia") != std::string::npos) return VET_NVIDIA;
        return VET_UNKNOW;
    }
    
	static void skeep_error_line_space(const char*& inout)
	{
		while ((*inout) == ' ' || (*inout) == '\t' || (*inout) == '\r') ++(inout);
    }
    
    static bool generic_parse_error_file_id(const char* in, const char** cout, int& out)
    {
        //jump all
        while(*in && (!::isnumber(*in))) ++in;
        //parse
        out = (int)std::strtod(in, (char**)cout);
        return in != (*cout);
    }
    
    static bool nvidia_parse_error_file_id(const char* in, const char** cout, int& out)
    {
        skeep_error_line_space(in);
        out = (int)std::strtod(in, (char**)cout);
        return in != (*cout);
    }
    
    static std::string parsing_error_log(const shader::filepath_map& filepath_map, const char* error)
	{
		std::stringstream stream_error(error);
		std::string error_line;
		std::stringstream stream_output;
		//start to parse
		while (std::getline(stream_error, error_line))
		{
			const char* c_error_line_in  = error_line.c_str();
			const char* c_error_line_out = c_error_line_in;
			int line_id = -1;
            bool error_id_found = false;
            //type
            //render info
            vendor_error_type type = get_type_of_error();
            //parsing by type
            switch (type)
            {
                default:
                case VET_AMD:
                case VET_INTEL:
                    error_id_found = generic_parse_error_file_id(c_error_line_in, &c_error_line_out, line_id);
                break;
                //AMD/NVIDIA
                case VET_NVIDIA:
                    error_id_found = nvidia_parse_error_file_id(c_error_line_in, &c_error_line_out, line_id);
                break;
            }
            
            if (error_id_found)
            {
                //get file path
                auto it_filepath_map = filepath_map.find(line_id);
                //put file path to output
                if (it_filepath_map != filepath_map.end())
                {
                    stream_output << it_filepath_map->second << ", line ";
                    stream_output << c_error_line_out << "\n";
                    continue;
                }
            }
            //default output
            stream_output << error_line << "\n";
		}
		return stream_output.str();
	}
		
	inline void jmp_spaces(const char*& str)
	{
		//jump space chars
		while ((*str) == ' ' || (*str) == '\t') ++str;
	}

	inline bool compare_and_jmp_keyword(const char*& str, const char* keyword)
	{
		//get len
		size_t len_str = std::strlen(keyword);
		//compare
		if (std::strncmp(str, keyword, len_str) == 0) { str += len_str; return true; };
		//else
		return false;
	}

	inline bool parse_cstring(const char* in_out, std::string& out, size_t& line)
	{
		const char *tmp = in_out;
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
						++line;
						break;
					case '\r': /* jump mac */
						++line;
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
			in_out = tmp + 1;
			return true;
		}
		return false;
	}

	inline bool parse_cinclude(const char* in_out, std::string& out, size_t& line)
	{
		const char *tmp = in_out;
		out = "";
		//start parse
		if ((*tmp) == '<')  //["...."]
		{
			++tmp;  //[...."]
			while ((*tmp) != '>' && (*tmp) != '\n')
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
						++line;
						break;
					case '\r': /* jump mac */
						++line;
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
			in_out = tmp + 1;
			return true;
		}
		return false;
	}

	inline bool process_include(resources_manager* rs_manager, 
								shader::filepath_map& filepath_map,
								std::string& output,
								const std::string& path_file,
								size_t& n_files,
								size_t level = 1)
	{
		//cicle test
		if (level > 16)
		{
			std::cout << "effect shader, header inclusion depth limit reached:" << std::endl
					  << "might be caused by cyclic header inclusion" << std::endl;
			return false;
		}
		//get base dir
		std::string filedir = filesystem::get_directory(path_file);
		//read file
		std::string text_of_file = filesystem::text_file_read_all(path_file);
		text_of_file += "\n";
		//stream
		std::istringstream effect(text_of_file);
		//line buffer
		std::string effect_line;
		//count line
		size_t   line = 0;
		//inc n file
		size_t this_file = n_files++;
		//start line
		output += "#line 1 " + std::to_string(this_file) + "\n";
		//add into the map
		filepath_map[this_file] = path_file;
		//start to parse
		while (std::getline(effect, effect_line))
		{
			//line count
			++line;
			//ptr to line
            const char* c_effect_line = effect_line.c_str();
            //jmp space
            jmp_spaces(c_effect_line);
			//pragma
			if (compare_and_jmp_keyword(c_effect_line, "#pragma"))
			{
				//jmp space
				jmp_spaces(c_effect_line);
				//include?
				if (compare_and_jmp_keyword(c_effect_line, "include"))
				{
					//jmp space
					jmp_spaces(c_effect_line);
					//path
					std::string file_name;
					std::string path_include;
					//path
					std::string output_include;
					//get path
					if (parse_cstring(c_effect_line, file_name, line))
					{
						path_include = filedir + "/" + file_name;
					}
					else
					{
						if (!rs_manager || !parse_cinclude(c_effect_line, file_name, line))
						{
							return false;
						}
						path_include = rs_manager->get_shader_path(file_name);
					}
					//add include
					if (!process_include(rs_manager, filepath_map, output_include, path_include, n_files, level + 1))
					{
						return false;
					}
					output += output_include;
					output += "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
					continue;
				}
			}
			//lines
			output += effect_line; output += "\n";
		}
		return true;
	}

	inline bool process_import(resources_manager* rs_manager,
							   shader::filepath_map& filepath_map,
							   std::stringstream& effect, 
							   const std::string& path_effect_file,
							   std::string& out_vertex,
							   std::string& out_fragment,
							   std::string& out_geometry,
							   std::string& out_tass_control,
							   std::string& out_tass_eval,
							   size_t& n_files,
							   size_t line  /* = 0*/,
							   size_t level /* = 0*/)
	{
		//cicle test
		if (level > 16)
		{
			std::cout << "effect shader, header inclusion depth limit reached:" << std::endl
					  << "might be caused by cyclic header inclusion" << std::endl;
			return false;
		}
		//state
		enum parsing_state
		{
			P_ALL,
			P_VERTEX,
			P_FRAGMENT,
			P_GEOMETRY,
			P_TASS_CONTROL,
			P_TASS_EVAL,
			P_SIZE
		};
		//strings
		std::string buffers[P_SIZE];
		//get base dir
		std::string filedir = filesystem::get_directory(path_effect_file);
		//parsing
		std::string& all = buffers[P_ALL];
		std::string& vertex = buffers[P_VERTEX];
		std::string& fragment = buffers[P_FRAGMENT];
		std::string& geometry = buffers[P_GEOMETRY];
		std::string& tass_control = buffers[P_TASS_CONTROL];
		std::string& tass_eval = buffers[P_TASS_EVAL];
		//start state
		parsing_state state = P_ALL;
		//line buffer
		std::string effect_line;
		//count files
		size_t  this_file = n_files++;
		//put line of "all" buffer
		all += "#line "+ std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
		//add into the map
		filepath_map[this_file] = path_effect_file;
		//start to parse
		while (std::getline(effect, effect_line))
		{
			//line count
			++line;
			//ptr to line
			const char* c_effect_line = effect_line.c_str();
			//jmp space
			jmp_spaces(c_effect_line);
			//is pragma?
			if (compare_and_jmp_keyword(c_effect_line, "#pragma"))
			{
				//jmp space
				jmp_spaces(c_effect_line);
				//type
				if (compare_and_jmp_keyword(c_effect_line, "all"))
				{
					state = P_ALL;
					all += "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
					continue;
				}
				else if (compare_and_jmp_keyword(c_effect_line, "vertex"))
				{
					state = P_VERTEX;
					vertex += "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
					continue;
				}
				else if (compare_and_jmp_keyword(c_effect_line, "fragment"))
				{
					state = P_FRAGMENT;
					fragment += "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
					continue;
				}
				else if (compare_and_jmp_keyword(c_effect_line, "geometry"))
				{
					state = P_GEOMETRY;
					geometry += "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
					continue;
				}
				else if (compare_and_jmp_keyword(c_effect_line, "tass") || compare_and_jmp_keyword(c_effect_line, "tessellation"))
				{
					//jmp space
					jmp_spaces(c_effect_line);
					//test type of tessellation
					if (compare_and_jmp_keyword(c_effect_line, "control"))
					{
						state = P_TASS_CONTROL;
						tass_control += "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
					}
					else if (compare_and_jmp_keyword(c_effect_line, "eval"))
					{
						state = P_TASS_EVAL;
						tass_eval += "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
					}
					continue;
				}
				else if (compare_and_jmp_keyword(c_effect_line, "include"))
				{
					//jmp space
					jmp_spaces(c_effect_line);
					//path
					std::string file_name;
					std::string path_include; 
					//path
					std::string output_include;
					//get path
					if (parse_cstring(c_effect_line, file_name, line))
					{
						path_include = filedir + "/" + file_name;
					}
					else
					{
						if (!rs_manager || !parse_cinclude(c_effect_line, file_name, line))
						{
							return false;
						}
						path_include = rs_manager->get_shader_path(file_name);
					}
					//add include
					if (!process_include(rs_manager, filepath_map, output_include, path_include, n_files, level + 1))
					{
						return false;
					}
					//lines
					buffers[state] += output_include;
					buffers[state] += "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
					continue;
				}
				else if (compare_and_jmp_keyword(c_effect_line, "import"))
				{
					//jmp space
					jmp_spaces(c_effect_line);
					//path
					std::string file_name;
					std::string import_path;			
					//get path
					if (parse_cstring(c_effect_line, file_name, line))
					{
						import_path = filedir + "/" + file_name;
					}
					else
					{
						if (!rs_manager || !parse_cinclude(c_effect_line, file_name, line))
						{
							return false;
						}
						import_path = rs_manager->get_shader_path(file_name);
					}
					//input stream
					std::stringstream import_effect(filesystem::text_file_read_all(import_path));
					//add include
					if (!process_import(rs_manager,
										filepath_map, 
										import_effect, 
										import_path, 
										vertex, 
										fragment, 
										geometry,
										tass_control,
										tass_eval,
									    n_files, line, level + 1))
					{
						return false;
					}
					//lines
					std::string current_line = "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
					buffers[P_VERTEX]		+= current_line;
					buffers[P_FRAGMENT]		+= current_line;
					buffers[P_GEOMETRY]		+= geometry.size() ? current_line : "";
					buffers[P_TASS_CONTROL] += tass_control.size() ? current_line : "";
					buffers[P_TASS_EVAL]	+= tass_eval.size() ? current_line : "";
					continue;
				}
			}
			//append to a buffer
			buffers[state] += effect_line; buffers[state] += "\n";
		}
		//copy output
		out_vertex		 += all + vertex;
		out_fragment	 += all + fragment;
		out_geometry	 += geometry.size()     || out_geometry.size()     ? all + geometry     : "";
		out_tass_control += tass_control.size() || out_tass_control.size() ? all + tass_control : "";
		out_tass_eval    += tass_eval.size()    || out_tass_eval.size()    ? all + tass_eval    : "";

		return true;
	}

	#pragma endregion 

	bool shader::load(resources_manager& resources, const std::string& effect_file)

	{
		//id file
		size_t  this_file = 0;
		//buffers
		std::string vertex, fragment, geometry, tcontrol, teval;
		//input stream
		std::stringstream stream_effect(filesystem::text_file_read_all(effect_file));
		//process
		if (!process_import(&resources,
							m_file_path_id,
							stream_effect,
							effect_file,
							vertex,
							fragment,
							geometry,
							tcontrol,
							teval,
							this_file,
							0, //line 0
							0  //file 0
						)) return false;
		//load shader
		return load_shader(
			vertex, 0,
			fragment, 0,
			geometry, 0,
			tcontrol, 0,
			teval, 0,
			{}
		);
	}

	bool shader::load(const std::string& effect_file,
                      const preprocess_map& defines)
	{
		//id file
		size_t  this_file = 0;
		//buffers
		std::string vertex, fragment, geometry, tcontrol, teval;
		//input stream
		std::stringstream stream_effect(filesystem::text_file_read_all(effect_file));
		//process
		if (!process_import(nullptr,
							m_file_path_id, 
							stream_effect,
						    effect_file,
							vertex, 
							fragment, 
							geometry, 
							tcontrol,
							teval,			
							this_file,
							0, //line 0
							0  //file 0
                            )) return false;
		//load shader
		return load_shader(vertex, 0,
						   fragment, 0,
						   geometry, 0,
						   tcontrol, 0,
			               teval, 0,
						   defines);
	}


	bool shader::load_effect(const std::string& effect,
					         const std::string& effect_file,
					         const preprocess_map& defines,
                             const size_t line_effect)
	{

		//id file
		size_t  this_file = 0;
		//buffers
		std::string vertex, fragment, geometry, tcontrol, teval;
		//input stream
		std::stringstream stream_effect(effect);
		//process
		if (!process_import(nullptr,
						    m_file_path_id,
							stream_effect,
							effect_file,
							vertex,
							fragment,
							geometry,
							tcontrol,
							teval,
							this_file,
						    line_effect,
							0)) return false;
		//load shader
		return load_shader(vertex, 0,
						  fragment, 0,
						  geometry, 0,
						  tcontrol, 0,
						  teval, 0,
						  defines);
	}


	bool shader::load_effect(resources_manager& resources,
							 const std::string& effect,
							 const std::string& effect_file,
							 const preprocess_map& defines,
							 const size_t line_effect)

	{

		//id file
		size_t  this_file = 0;
		//buffers
		std::string vertex, fragment, geometry, tcontrol, teval;
		//input stream
		std::stringstream stream_effect(effect);
		//process
		if (!process_import(&resources,
							m_file_path_id,
							stream_effect,
							effect_file,
							vertex,
							fragment,
							geometry,
							tcontrol,
							teval,
							this_file,
							line_effect,
							0)) return false;
		//load shader
		return load_shader(
			vertex, 0,
			fragment, 0,
			geometry, 0,
			tcontrol, 0,
			teval, 0,
			defines
		);
	}


	bool shader::load_shader(const std::string& vs_str,const size_t line_vs,
						     const std::string& fs_str,const size_t line_fs,
							 const std::string& gs_str,const size_t line_gs,
							 const std::string& tcs_str,const size_t line_tcs,
							 const std::string& tes_str,const size_t line_tes,
							 const preprocess_map& all_process)
	{
		bool success_to_compile = true;
		//delete last shader
		if (m_shader) render::delete_shader(m_shader);
		//int shader version
		int shader_version = 410;
		//list define
		std::string header_string;
        //defines
        for (auto& p : all_process)
        {
            //get version
            if(std::get<0>(p)=="version")
			{
				shader_version = std::atoi(std::get<1>(p).c_str());
				continue;
			}
            //add
			header_string += "#" + std::get<0>(p) +" "+ std::get<1>(p) + "\n";
        }
        ////////////////////////////////////////////////////////////////////////////////
        //define version
		header_string = "#version " + std::to_string(410) + "\n" + header_string;
		////////////////////////////////////////////////////////////////////////////////
		// load shaders from files
		std::vector < shader_source_information > shader_infos;
		//add
		if(vs_str.c_str())
			shader_infos.emplace_back(shader_source_information{ ST_VERTEX_SHADER, header_string, vs_str, line_vs });
		if (fs_str.c_str())
			shader_infos.emplace_back(shader_source_information{ ST_FRAGMENT_SHADER, header_string, fs_str, line_fs });
		if (gs_str.c_str())
			shader_infos.emplace_back(shader_source_information{ ST_GEOMETRY_SHADER, header_string, gs_str, line_gs });
		if (tcs_str.c_str())
			shader_infos.emplace_back(shader_source_information{ ST_TASSELLATION_CONTROL_SHADER, header_string, tcs_str, line_tcs });
		if (tes_str.c_str())
			shader_infos.emplace_back(shader_source_information{ ST_TASSELLATION_EVALUATION_SHADER, header_string, tes_str, line_tes });
		//compile
		m_shader = render::create_shader(shader_infos);
		//return success
		return m_shader
		   && !render::shader_compiled_with_errors(m_shader)
		   && !render::shader_linked_with_error(m_shader);
	}

	std::string shader::get_errors() const
	{
		//output
		std::string errors;
		//liker error:
		if (render::shader_linked_with_error(m_shader))
		{
			errors += "Liker, error to liking:\n";
			errors += render::get_shader_liker_error(m_shader);
		}
		//shader errors:
		if (render::shader_compiled_with_errors(m_shader))
		for(const std::string& error : render::get_shader_compiler_errors(m_shader))
		{
			errors += "Shader, error to compile:\n";
			errors += parsing_error_log(m_file_path_id, error.c_str());
		}
		//return
		return errors;
	}

	shader::~shader()
	{
		//delete last shader
		if (m_shader) render::delete_shader(m_shader);
	}

	void shader::bind()
	{
		render::bind_shader(m_shader);
	}
	
	//disabilita shader
	void shader::unbind()
	{
		render::unbind_shader(m_shader);
	}

	//program
	context_shader* shader::get_raw_shader() const
	{
		return m_shader;
	}

	//get consts
	context_uniform* shader::get_uniform(const char *name)
	{
		return render::get_uniform(m_shader, std::string(name));
	}
	context_uniform* shader::get_uniform_buffer(const char *name)
	{
		return nullptr;
	}
}
