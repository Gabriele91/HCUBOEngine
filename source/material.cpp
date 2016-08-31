//
//  material.cpp
//  HCubo
//
//  Created by Gabriele on 21/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <list>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <material.h>
#include <filesystem.h>
#include <resources_manager.h>

namespace hcube
{

	#define CSTRCMP( x, y ) ( std::strncmp( x, y, sizeof(y)-1 ) == 0 )
	#define CSTRCMP_SKIP( x, y ) ( [&x] () -> bool { if ( CSTRCMP(x,y) ) { x += sizeof(y)-1; return true; } return false; } )()

	using  map_parameters = effect::map_parameters;
	using  map_techniques = effect::map_techniques;
	using  technique = effect::technique;
	using  parameter = effect::parameter;
	using  parameters = effect::parameters;
	using  parameter_type = effect::parameter_type;

	class material_parser
	{
	public:

		struct parameter_field
		{
			struct values
			{
				float		m_float{ 0.0 };
				int			m_int{ 0 };
				vec2	m_vec2;
				vec3	m_vec3;
				vec4	m_vec4;
				mat4	m_mat4;
				std::string m_texture;
			};

			std::string     m_name;
			parameter_type  m_type;
			values          m_value;
		};

		struct effect_field
		{
			std::string m_name;
			std::vector< parameter_field > m_parameters;
		};

		struct error_field
		{
			size_t m_line{ 0 };
			std::string m_error;

			error_field()
			{
			}

			error_field(
				size_t line,
				const std::string& error)
				: m_line(line)
				, m_error(error)
			{
			}
		};

		struct context
		{
			effect_field m_effect;
			std::list < error_field >	   m_errors;
			size_t						   m_line{ 0 };

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

		bool parse(std::string& source)
		{
			context ctx_default;
			const char* source_ptr = source.c_str();
			return parse(ctx_default,source_ptr);
		}
    
		bool parse(context& default_context,const std::string& source)
		{
			const char* source_ptr = source.c_str();
			return parse(default_context,source_ptr);
		}
    
		bool parse(context& default_context,const char*& ptr)
		{
			//set context
			m_context = &default_context;
			//restart
			m_context->m_errors.clear();
			m_context->m_line = 1;
			m_context->m_effect.m_name = "";
			m_context->m_effect.m_parameters.clear();
			//skeep line and comments
			skeep_space_end_comment(ptr);
			//get type
			while(*ptr != EOF && *ptr != '\0')
			{
				//parsing a block
				if(CSTRCMP_SKIP(ptr,"effect")) { if(!parse_effect(ptr))  return false; }
				else						   { push_error("Not found a valid command"); return false; }
				//skeep line and comments
				skeep_space_end_comment(ptr);
			}
			return true;
		}
    
    
	protected:

		context* m_context{ nullptr };
		//////////////////////////////////////////////////////
		bool parse_effect(const char*& ptr)
		{
			//skeep spaces
			skeep_space_end_comment(ptr);
			//name technique
			std::string new_e_name;
			//get name
			if (!parse_cstring(ptr, &ptr, new_e_name))
			{
				push_error("Not valid effect name");
				return false;
			}
			//save effect name
			m_context->m_effect.m_name = new_e_name;
			//parse block
			return parse_parameters_block(ptr);
		}
		//////////////////////////////////////////////////////
		bool parse_parameters_block(const char*& ptr)
		{
			//skeep spaces
			skeep_space_end_comment(ptr);
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				//read all values
				while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
				{
					//skeep spaces
					skeep_space_end_comment(ptr);
					//alloc uniform field
					parameter_field field;
					//parse
					if (!parse_name(ptr, &ptr, field.m_name))
					{
						push_error("Not valid uniform name");
						return false;
					}
					//skeep "line" space
					skeep_line_space(ptr);
					//value
					if (!parse_value(ptr, field))
					{
						push_error("Not valid uniform field");
						return false;
					}
					//push
					m_context->m_effect.m_parameters.push_back(field);
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

		bool parse_value(const char*& ptr, parameter_field& field)
		{
			if (!parse_type(ptr, &ptr, field.m_type)) return false;
			//skeep spaces
			skeep_space_end_comment(ptr);
			//parse '('
			if (!is_start_arg(*ptr)) return false;
			//jump '('
			++ptr;
			//space
			skeep_space_end_comment(ptr);
			//parse by type
			switch (field.m_type)
			{
			case parameter_type::PT_INT:
				if (!parse_int(ptr, &ptr, field.m_value.m_int)) return false;
				break;
			case parameter_type::PT_FLOAT:
				if (!parse_float(ptr, &ptr, field.m_value.m_float)) return false;
				break;
			case parameter_type::PT_TEXTURE:
				if (!parse_texture(ptr, field)) return false;
				break;
			case parameter_type::PT_VEC2:
				if (!parse_vec2(ptr, field)) return false;
				break;
			case parameter_type::PT_VEC3:
				if (!parse_vec3(ptr, field)) return false;
				break;
			case parameter_type::PT_VEC4:
				if (!parse_vec4(ptr, field)) return false;
				break;
			case parameter_type::PT_MAT4:
				if (!parse_mat4(ptr, field)) return false;
				break;
			default: return false; break;
			}
			//jump space
			skeep_space_end_comment(ptr);
			//parse ')'
			if (!is_end_arg(*ptr))  return false;
			//jump ')'
			++ptr;
			//...
			return true;
		}

		bool parse_texture(const char*& ptr, parameter_field& field)
		{
			//texture name
			std::string texture_name;
			//parse
			if (!parse_cstring(ptr, &ptr, field.m_value.m_texture))
			{
				return false;
			}
			return true;
		}

		bool parse_vec2(const char*& ptr, parameter_field& field)
		{
			if (!parse_float(ptr, &ptr, field.m_value.m_vec2.x)) return false;

			skeep_space_end_comment(ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec2.y)) return false;

			return true;
		}

		bool parse_vec3(const char*& ptr, parameter_field& field)
		{
			if (!parse_float(ptr, &ptr, field.m_value.m_vec3.x)) return false;

			skeep_space_end_comment(ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec3.y)) return false;

			skeep_space_end_comment(ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec3.z)) return false;

			return true;
		}

		bool parse_vec4(const char*& ptr, parameter_field& field)
		{
			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.x)) return false;

			skeep_space_end_comment(ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.y)) return false;

			skeep_space_end_comment(ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.z)) return false;

			skeep_space_end_comment(ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.w)) return false;

			return true;
		}

		bool parse_mat4(const char*& ptr, parameter_field& field)
		{
			//first
			if (!parse_float(ptr, &ptr, field.m_value.m_mat4[0][0])) return false;
			//skeep spaces
			skeep_space_end_comment(ptr);
			// if one param, call mat4 constructor
			if (!is_comm_arg(*ptr))
			{
				// is 'mat4('<float>')'
				field.m_value.m_mat4 = mat4(field.m_value.m_mat4[0][0]);
				//success
				return true;
			}
			//for all
			for (int x = 0; x != 4; ++x)
				for (int y = 0; y != 4; ++y)
				{
					//jmp first
					if ((x + y) == 0) continue;
					//parse ','
					skeep_space_end_comment(ptr);
					if (!is_comm_arg(*ptr)) return false; else ++ptr;
					skeep_space_end_comment(ptr);
					//parse value
					if (!parse_float(ptr, &ptr, field.m_value.m_mat4[x][y])) return false;
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
		bool parse_type(const char* in,
						const char** cout,
						parameter_type& type)
		{
			if (CSTRCMP(in, "int"))
			{
				(*cout) += 3; type = parameter_type::PT_INT;  return true;
			}

			if (CSTRCMP(in, "float"))
			{
				(*cout) += 5; type = parameter_type::PT_FLOAT;  return true;
			}

			if (CSTRCMP(in, "texture"))
			{
				(*cout) += 7; type = parameter_type::PT_TEXTURE;  return true;
			}

			if (CSTRCMP(in, "vec2"))
			{
				(*cout) += 4; type = parameter_type::PT_VEC2;  return true;
			}

			if (CSTRCMP(in, "vec3"))
			{
				(*cout) += 4; type = parameter_type::PT_VEC3;  return true;
			}

			if (CSTRCMP(in, "vec4"))
			{
				(*cout) += 4; type = parameter_type::PT_VEC4;  return true;
			}

			if (CSTRCMP(in, "mat4"))
			{
				(*cout) += 4; type = parameter_type::PT_MAT4;  return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////
		void push_error(const std::string& error)
		{
			m_context->m_errors.push_front(error_field{ m_context->m_line,error });
		}
    
	};

	material::~material() { if (m_parameters) delete m_parameters; };

	static std::string name_of_default_parameters[material::MAT_DEFAULT_MAX] =
	{
		"diffuse_map",
		"normal_map",
		"specular_map",
		"color",
        "mask"
	};
	static void save_if_is_a_default_parameter(int id_param,const std::string& name, int default_parameters[])
	{
		for (int i = 0; i != material::MAT_DEFAULT_MAX; ++i)
		{
			if (name_of_default_parameters[i] == name)
			{
				default_parameters[i] = id_param;
				return;
			}
		}
	}

	bool material::load(resources_manager& resources,const std::string& path)
	{
    
		//parse
		material_parser parser;
		material_parser::context context;
		//do parsing
		if(!parser.parse(context, filesystem::text_file_read_all(path)))
		{
			std::cout << "Material: "
					  << path
					  << std::endl
					  << context.errors_to_string()
					  << std::endl;
			return false;
		}
		//load effect
		m_effect = resources.get_effect(context.m_effect.m_name);
		//load params
		if (m_effect)
		{
			//copy context
			m_parameters = m_effect->copy_all_parameters();
			//set local params
			for (size_t i = 0; i != context.m_effect.m_parameters.size(); ++i)
			{
				//get field
				material_parser::parameter_field & ctx_param = context.m_effect.m_parameters[i];
				//get id
				int   id_param                               = m_effect->get_parameter_id(ctx_param.m_name);
				//test
				if (id_param < 0) continue;
				//get pram
				effect::parameter* param                     = (*m_parameters)[id_param].get();
				//select
				switch (ctx_param.m_type)
				{
				default:
				case effect::PT_TEXTURE_ARRAY:
				case effect::PT_NONE:
					/* void */
				break;
				case effect::PT_INT:    param->set_value(ctx_param.m_value.m_int); break;
				case effect::PT_FLOAT:  param->set_value(ctx_param.m_value.m_float); break;
				case effect::PT_TEXTURE:param->set_value(resources.get_texture(ctx_param.m_value.m_texture)); break;
				case effect::PT_VEC2:	param->set_value(ctx_param.m_value.m_vec2); break;
				case effect::PT_VEC3:	param->set_value(ctx_param.m_value.m_vec3); break;
				case effect::PT_VEC4:	param->set_value(ctx_param.m_value.m_vec4); break;
				case effect::PT_MAT4:	param->set_value(ctx_param.m_value.m_mat4); break;
				}
				//if a default parameter
				save_if_is_a_default_parameter(id_param, ctx_param.m_name, m_default_parameters);
			}
		}
		return true;
	}
	
	effect::ptr	material::get_effect() const { return m_effect; }

	effect::parameters* material::get_parameters() const { return m_parameters; }
	
	const effect::parameter* material::get_default_parameter(material::default_parameters dp) const
	{
		int id_param = m_default_parameters[dp];
		if(id_param>=0) return (*m_parameters)[ m_default_parameters[dp] ].get();
		return nullptr;
	}

	material_ptr material::copy() const
	{
		auto omaterial = material_snew();
		//effect
		omaterial->m_effect = m_effect;
		//parameters alloc
		omaterial->m_parameters = new effect::parameters(m_parameters->size());
		//copy
		for (size_t i = 0; i != m_parameters->size(); ++i)
		{
			(*omaterial->m_parameters)[i] = std::unique_ptr< effect::parameter >((*m_parameters)[i]->copy());
		}
		//return
		return omaterial;
	}
}