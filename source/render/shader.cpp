#include <string>
#include <sstream>
#include <vector>
#include <cstdio>
#include <iostream>
#include <hcube/math/vector_math.h>
#include <hcube/core/filesystem.h>
#include <hcube/render/render.h>
#include <hcube/render/OpenGL4.h>
#include <hcube/render/shader.h>
#include <hcube/core/resources_manager.h>

namespace hcube
{

static const char default_glsl_defines[] =
R"GLSL(
//POSITION TRANSFORM
#define ATT_POSITIONT 0
#define ATT_NORMAL0   1
#define ATT_TEXCOORD0 2
#define ATT_TANGENT0  3
#define ATT_BINORMAL0 4
#define ATT_COLOR0    5

//POSITION 0
#define ATT_POSITION0 6
#define ATT_NORMAL1   7
#define ATT_TEXCOORD1 8
#define ATT_TANGENT1  9
#define ATT_BINORMAL1 10
#define ATT_COLOR1    11

//POSITION 1
#define ATT_POSITION1 12
#define ATT_NORMAL2   13
#define ATT_TEXCOORD2 14
#define ATT_TANGENT2  15
#define ATT_BINORMAL2 16
#define ATT_COLOR2    17
)GLSL";

	static const char* glsl_type_to_string(GLenum type)
	{
		switch (type)
		{
		case GL_BOOL: return "bool";
		case GL_INT: return "int";
		case GL_FLOAT: return "float";
		case GL_FLOAT_VEC2: return "vec2";
		case GL_FLOAT_VEC3: return "vec3";
		case GL_FLOAT_VEC4: return "vec4";
		case GL_FLOAT_MAT2: return "mat2";
		case GL_FLOAT_MAT3: return "mat3";
		case GL_FLOAT_MAT4: return "mat4";
		case GL_SAMPLER_2D: return "sampler2D";
		case GL_SAMPLER_3D: return "sampler3D";
		case GL_SAMPLER_CUBE: return "samplerCube";
		case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
		default: break;
		}
		return "other";
	}
	
	#pragma region "Shader parser"

	static void skeep_error_line_space(const char*& inout)
	{
		while ((*inout) == ' ' || (*inout) == '\t' || (*inout) == '\r') ++(inout);
	}

	static bool parse_error_file_id(const char* in, const char** cout, int& out)
	{
		skeep_error_line_space(in);
		out = (int)std::strtod(in, (char**)cout);
		return in != (*cout);
	}

	static std::string parsing_error_log(shader::filepath_map& filepath_map, const char* error)
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
			if (parse_error_file_id(c_error_line_in, &c_error_line_out, line_id))
			{
				//get file path
				auto it_filepat_map = filepath_map.find(line_id);
				//put file path to output
				if (it_filepat_map != filepath_map.end())
				{
					stream_output << it_filepat_map->second << " : ";
					stream_output << c_error_line_out << "\n";
					continue;
				}
			}
			//put file path to output
			stream_output << error_line << "\n";
		}
		return stream_output.str();
	}

	static bool log_error(shader::filepath_map& filepath_map,
						  unsigned int shader,
						  int status, 
						  const std::string& type)
	{
		if (!status)
		{
			//get len
			GLint info_len = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
			//print
			if (info_len > 1)
			{
				char* info_log = (char*)malloc(sizeof(char) * info_len);
				glGetShaderInfoLog(shader, info_len, NULL, info_log);
				std::cout << type << ", error compiling shader:\n" << parsing_error_log(filepath_map,info_log) << std::endl;
				free(info_log);
			}
			return false;
		}
		return true;
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
				if (compare_and_jmp_keyword(c_effect_line, "vertex"))
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
		out_geometry	 += geometry.size()     ? all + geometry     : "";
		out_tass_control += tass_control.size() ? all + tass_control : "";
		out_tass_eval    += tass_eval.size()    ? all + tass_eval    : "";

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
		return load_shader(vertex, 0,
			fragment, 0,
			geometry, 0,
			tcontrol, 0,
			teval, 0,
			defines);
	}

	bool shader::load(const std::string& vs_file,
		              const std::string& fs_file,
                      const std::string& gs_file,
					  const std::string& tcs_file,
					  const std::string& tes_file,
                      const preprocess_map& defines)
	{
		return load_shader
		(
			filesystem::text_file_read_all(vs_file),                         0,
			filesystem::text_file_read_all(fs_file),                         0,
			gs_file.size()  ? filesystem::text_file_read_all(gs_file)  : "", 0,
			tcs_file.size() ? filesystem::text_file_read_all(tcs_file) : "", 0,
			tes_file.size() ? filesystem::text_file_read_all(tes_file) : "", 0,
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
		delete_program();
		//int variables
		GLint compiled = 0, linked = 0;
        //version is defined?
        bool version_is_defined = false;
		//list define
		std::string defines_string;
        //defines
        for (auto& p : all_process)
        {
            //get version
            if(std::get<0>(p)=="version"){ version_is_defined = true; }
            //add
            defines_string += "#" + std::get<0>(p) +" "+ std::get<1>(p) + "\n";
        }
        ////////////////////////////////////////////////////////////////////////////////
        //define version
        if(!version_is_defined) defines_string="#version 410\n"+defines_string;
		////////////////////////////////////////////////////////////////////////////////
		// load shaders files
		// vertex
		std::string file_vs =
			defines_string +
			default_glsl_defines +
            "#define cbuffer     layout(std140) uniform     \n"
			"#define saturate(x) clamp( x, 0.0, 1.0 )       \n"
			"#define lerp        mix                        \n"
			"#line " + std::to_string(line_vs) + "\n" +
			vs_str;
		//create a vertex shader
		m_shader_vs = glCreateShader(GL_VERTEX_SHADER);
		const char* c_vs_source = file_vs.c_str();
		glShaderSource(m_shader_vs, 1, &c_vs_source, 0);
		//compiling
		compiled = 0;
		glCompileShader(m_shader_vs);
		glGetShaderiv(m_shader_vs, GL_COMPILE_STATUS, &compiled);
		if (!log_error(m_file_path_id,m_shader_vs, compiled, "vertex shader"))
        {
            glDeleteShader(m_shader_vs);
            m_shader_vs=0;
			success_to_compile = false;
        }
		////////////////////////////////////////////////////////////////////////////////
		//fragmentx
		std::string file_fs =
			defines_string +
            default_glsl_defines +
            "#define cbuffer     layout(std140) uniform     \n"
			"#define saturate(x) clamp( x, 0.0, 1.0 )       \n"
			"#define lerp        mix                        \n"
			"#define bestp                                  \n"
			"#define highp                                  \n"
			"#define mediump                                \n"
			"#define lowp                                   \n"
			"#line " + std::to_string(line_fs) + "\n" +
			fs_str;
		//create a fragment shader
		m_shader_fs = glCreateShader(GL_FRAGMENT_SHADER);
		const char* c_fs_source = file_fs.c_str();
		glShaderSource(m_shader_fs, 1, &c_fs_source, 0);
		//compiling
		compiled = 0;
		glCompileShader(m_shader_fs);
		glGetShaderiv(m_shader_fs, GL_COMPILE_STATUS, &compiled);
		if (!log_error(m_file_path_id, m_shader_fs, compiled, "fragment shader"))
        {
            glDeleteShader(m_shader_fs);
            m_shader_fs = 0;
			success_to_compile = false;
        }
		////////////////////////////////////////////////////////////////////////////////
		//geometry
		if (gs_str.size())
		{
			//geometry header
			std::string file_gs =
				defines_string +
				default_glsl_defines +
                "#define cbuffer     layout(std140) uniform     \n"
				"#define saturate(x) clamp( x, 0.0, 1.0 )       \n"
				"#define lerp        mix                        \n"
				"#line " + std::to_string(line_gs) + "\n" +
				gs_str;
			//geometry
			m_shader_gs = glCreateShader(GL_GEOMETRY_SHADER);
			const char* c_gs_source = file_gs.c_str();
			glShaderSource(m_shader_gs, 1, &(c_gs_source), 0);
			//compiling
			compiled = 0;
			glCompileShader(m_shader_gs);
			glGetShaderiv(m_shader_gs, GL_COMPILE_STATUS, &compiled);
            if (!log_error(m_file_path_id, m_shader_gs, compiled, "Geometry"))
            {
                glDeleteShader(m_shader_gs);
                m_shader_gs = 0;
				success_to_compile = false;
            }
		}
		////////////////////////////////////////////////////////////////////////////////
		//tessellation control
		if (tcs_str.size())
		{
			//geometrfs_stry
			std::string file_tcs =
				defines_string +
				default_glsl_defines +
				"#define cbuffer     layout(std140) uniform     \n"
				"#define saturate(x) clamp( x, 0.0, 1.0 )       \n"
				"#define lerp        mix                        \n"
				"#line " + std::to_string(line_tcs) + "\n" +
				tcs_str;
			//geometry
			m_shader_tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
			const char* c_tcs_source = file_tcs.c_str();
			glShaderSource(m_shader_tcs, 1, &(c_tcs_source), 0);
			//compiling
			compiled = 0;
			glCompileShader(m_shader_tcs);
			glGetShaderiv(m_shader_tcs, GL_COMPILE_STATUS, &compiled);
			if (!log_error(m_file_path_id, m_shader_tcs, compiled, "Tessellation control"))
			{
				glDeleteShader(m_shader_tcs);
				m_shader_tcs = 0;
				success_to_compile = false;
			}
		}
		////////////////////////////////////////////////////////////////////////////////
		//tessellation evaluation
		if (tes_str.size())
		{
			//geometrfs_stry
			std::string file_tes =
				defines_string +
				default_glsl_defines +
				"#define cbuffer     layout(std140) uniform     \n"
				"#define saturate(x) clamp( x, 0.0, 1.0 )       \n"
				"#define lerp        mix                        \n"
				"#line " + std::to_string(line_tes) + "\n" +
				tes_str;
			//geometry
			m_shader_tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
			const char* c_tes_source = file_tes.c_str();
			glShaderSource(m_shader_tes, 1, &(c_tes_source), 0);
			//compiling
			compiled = 0;
			glCompileShader(m_shader_tes);
			glGetShaderiv(m_shader_tes, GL_COMPILE_STATUS, &compiled);
			if (!log_error(m_file_path_id, m_shader_tes, compiled, "Tessellation evaluation"))
			{
				glDeleteShader(m_shader_tes);
				m_shader_tes = 0;
				success_to_compile = false;
			}
		}
		////////////////////////////////////////////////////////////////////////////////
		//made a shader program
        m_shader_id = glCreateProgram();
        if (m_shader_fs)  glAttachShader(m_shader_id, m_shader_fs);
		if (m_shader_vs)  glAttachShader(m_shader_id, m_shader_vs);
		if (m_shader_gs)  glAttachShader(m_shader_id, m_shader_gs);
		if (m_shader_tcs) glAttachShader(m_shader_id, m_shader_tcs);
		if (m_shader_tes) glAttachShader(m_shader_id, m_shader_tes);
		glLinkProgram(m_shader_id);
		//get link status
		glGetProgramiv(m_shader_id, GL_LINK_STATUS, &linked);
		//out errors
		if (!linked)
		{
            //get liking errors
			GLint info_len = 0;
			glGetProgramiv(m_shader_id, GL_INFO_LOG_LENGTH, &info_len);
			if (info_len)
			{
				char* info_log = (char*)malloc(sizeof(char) * info_len);
				glGetProgramInfoLog(m_shader_id, info_len, NULL, info_log);
				std::cout << "error linking program: \n" << parsing_error_log(m_file_path_id,info_log) << std::endl;
				free(info_log);
			}
			std::cout << "error linking" << std::endl;
			//"detach" all shaders
			if (m_shader_fs)  glDetachShader(m_shader_id, m_shader_fs);
			if (m_shader_vs)  glDetachShader(m_shader_id, m_shader_vs);
			if (m_shader_gs)  glDetachShader(m_shader_id, m_shader_gs);
			if (m_shader_tcs) glDetachShader(m_shader_id, m_shader_tcs);
			if (m_shader_tes) glDetachShader(m_shader_id, m_shader_tes);
			//destoy all shaders
			if (m_shader_fs)  glDeleteShader(m_shader_fs);
			if (m_shader_vs)  glDeleteShader(m_shader_vs);
			if (m_shader_gs)  glDeleteShader(m_shader_gs);
			if (m_shader_tcs) glDeleteShader(m_shader_tcs);
			if (m_shader_tes) glDeleteShader(m_shader_tes);
			//destoy program
			if(m_shader_id) glDeleteProgram(m_shader_id);
			//all to 0
			m_shader_vs  = 0;
			m_shader_fs  = 0;
			m_shader_gs  = 0;
			m_shader_tcs = 0;
			m_shader_tes = 0;
			m_shader_id  = 0;
			//fail
			success_to_compile = false;
		}
		return success_to_compile;
	}

	void shader::delete_program()
	{
		if (m_shader_id)
		{
			glUseProgram(0);
			//detach shader
			glDetachShader(m_shader_id, m_shader_fs);
			glDetachShader(m_shader_id, m_shader_vs);
			if (m_shader_gs)  glDetachShader(m_shader_id, m_shader_gs);
			if (m_shader_tcs) glDetachShader(m_shader_id, m_shader_tcs);
			if (m_shader_tes) glDetachShader(m_shader_id, m_shader_tes);
			//delete all shaders
			glDeleteShader(m_shader_fs);
			glDeleteShader(m_shader_vs);
			if (m_shader_gs)  glDeleteShader(m_shader_gs);
			if (m_shader_tcs) glDeleteShader(m_shader_tcs);
			if (m_shader_tes) glDeleteShader(m_shader_tes);
			//delete shader program
			glDeleteProgram(m_shader_id);
			//all to null
			m_shader_vs = 0;
			m_shader_fs = 0;
			m_shader_gs = 0;
			m_shader_tcs = 0;
			m_shader_tes = 0;
			m_shader_id = 0;
		}
	}

	shader::~shader()
	{
		delete_program();
	}

	void shader::bind()
	{
		//start texture uniform
		m_uniform_ntexture = -1;
		//uniform parogram shaders
		glUseProgram(m_shader_id);
	}

	//get uniform id
	int shader::get_uniform_id(const char *name)
	{
		return glGetUniformLocation(m_shader_id, name);
	}
	unsigned int shader::get_uniform_buffer_id(const char *name)
	{
		return glGetUniformBlockIndex(m_shader_id, name);
	}

	//disabilita shader
	void shader::unbind()
	{
		//disable textures
		while (m_uniform_ntexture >= 0)
		{
            render::unbind_texture((int)m_uniform_ntexture);
			--m_uniform_ntexture;
		}
		//disable program
		glUseProgram(0);
	}

	//id programma
	unsigned int shader::program_id() const
	{
		return m_shader_id;
	}

	//uniform

	void uniform::set_value(texture::ptr in_texture)
	{
		long n_texture = ++m_shader->m_uniform_ntexture;
		//bind texture
		render::bind_texture(in_texture->get_context_texture(), (int)n_texture);
		//bind id
		glUniform1i((GLint)m_id, (int)n_texture);
	}

	void uniform::set_value(context_texture* in_texture)
	{
		long n_texture = ++m_shader->m_uniform_ntexture;
		//bind texture
		render::bind_texture(in_texture, (int)n_texture);
		//bind id
		glUniform1i((GLint)m_id, (int)n_texture);
	}

	void uniform::set_value(int i)
	{
		glUniform1i((GLint)m_id, i);
	}
	void uniform::set_value(float f)
	{
		glUniform1f((GLint)m_id, f);
	}
	void uniform::set_value(const vec2& v2)
	{
		glUniform2fv((GLint)m_id, 1, value_ptr(v2));
	}
	void uniform::set_value(const vec3& v3)
	{
		glUniform3fv((GLint)m_id, 1, value_ptr(v3));
	}
	void uniform::set_value(const vec4& v4)
	{
		glUniform4fv((GLint)m_id, 1, value_ptr(v4));
	}
	void uniform::set_value(const mat3& m3)
	{
		glUniformMatrix3fv((GLint)m_id, 1, GL_FALSE, value_ptr(m3));
	}
	void uniform::set_value(const mat4& m4)
	{
		glUniformMatrix4fv((GLint)m_id, 1, GL_FALSE, value_ptr(m4));
	}

	void uniform::set_value(const int* i, size_t n)
	{
		glUniform1iv((GLint)m_id, (GLsizei)n, i);
	}
	void uniform::set_value(const float* f, size_t n)
	{
		glUniform1fv((GLint)m_id, (GLsizei)n, f);
	}
	void uniform::set_value(const vec2* v2, size_t n)
	{
		glUniform2fv((GLint)m_id, (GLsizei)n, value_ptr(*v2));
	}
	void uniform::set_value(const vec3* v3, size_t n)
	{
		glUniform3fv((GLint)m_id, (GLsizei)n, value_ptr(*v3));
	}
	void uniform::set_value(const vec4* v4, size_t n)
	{
		glUniform4fv((GLint)m_id, (GLsizei)n, value_ptr(*v4));
	}
	void uniform::set_value(const mat3* m3, size_t n)
	{
		glUniformMatrix3fv((GLint)m_id, (GLsizei)n, GL_FALSE, value_ptr(*m3));
	}
	void uniform::set_value(const mat4* m4, size_t n)
	{
		glUniformMatrix4fv((GLint)m_id, (GLsizei)n, GL_FALSE, value_ptr(*m4));
	}

	void uniform::set_value(const std::vector < int >& i)
	{
		set_value(i.data(), i.size());
	}
	void uniform::set_value(const std::vector < float >& f)
	{
		set_value(f.data(), f.size());
	}
	void uniform::set_value(const std::vector < vec2 >& v2)
	{
		set_value(v2.data(), v2.size());
	}
	void uniform::set_value(const std::vector < vec3 >& v3)
	{
		set_value(v3.data(), v3.size());
	}
	void uniform::set_value(const std::vector < vec4 >& v4)
	{
		set_value(v4.data(), v4.size());
	}
	void uniform::set_value(const std::vector < mat3 >& m3)
	{
		set_value(m3.data(), m3.size());
	}
	void uniform::set_value(const std::vector < mat4 >& m4)
	{
		set_value(m4.data(), m4.size());
	}

	void uniform::set_value(const context_const_buffer* buffer)
	{
		glUniformBlockBinding(GL_UNIFORM_BUFFER,(GLuint)m_id, (GLuint)render::get_native_CB( buffer ));
	}
	
	uniform* shader::get_uniform(const char *name)
	{
		auto uit = m_uniform_map.find(name);
		//if find
		if (uit != m_uniform_map.end()) return &uit->second;
		//else
		int uid = get_uniform_id(name);
		if (uid < 0) return nullptr;
		//add and return
		return &(m_uniform_map[name] = uniform(this, (long)uid));
	}

	uniform* shader::get_uniform_buffer(const char *name)
	{
		auto uit = m_uniform_map.find(name);
		//if find
		if (uit != m_uniform_map.end()) return &uit->second;
		//else
		unsigned int uid = get_uniform_buffer_id(name);
		if (uid == GL_INVALID_INDEX) return nullptr;
		//add and return
		return &(m_uniform_map[name] = uniform(this, (long)uid));
	}
}