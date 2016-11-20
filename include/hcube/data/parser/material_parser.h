#pragma once
#include <string>
#include <hcube/render/effect.h>
#include <hcube/data/parser/utils_parser.h>

namespace hcube
{
namespace parser
{

	class material_parser : protected utils_parser
	{

		using  map_parameters = effect::map_parameters;
		using  map_techniques = effect::map_techniques;
		using  technique = effect::technique;
		using  parameter = effect::parameter;
		using  parameters = effect::parameters;
		using  parameter_type = effect::parameter_type;

	public:

		struct parameter_field
		{
			struct values
			{
				float		m_float{ 0.0 };
				int			m_int{ 0 };
				vec2		m_vec2;
				vec3		m_vec3;
				vec4		m_vec4;
				mat4		m_mat4;
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
			return parse(ctx_default, source_ptr);
		}

		bool parse(context& default_context, const std::string& source)
		{
			const char* source_ptr = source.c_str();
			return parse(default_context, source_ptr);
		}

		bool parse(context& default_context, const char*& ptr)
		{
			//set context
			m_context = &default_context;
			//restart
			m_context->m_errors.clear();
			m_context->m_line = 1;
			m_context->m_effect.m_name = "";
			m_context->m_effect.m_parameters.clear();
			//skeep line and comments
			skeep_space_end_comment(m_context->m_line, ptr);
			//get type
			while (*ptr != EOF && *ptr != '\0')
			{
				//parsing a block
				if (cstr_cmp_skip(ptr, "effect")) { if (!parse_effect(ptr))  return false; }
				else { push_error("Not found a valid command"); return false; }
				//skeep line and comments
				skeep_space_end_comment(m_context->m_line, ptr);
			}
			return true;
		}


	protected:

		context* m_context{ nullptr };
		//////////////////////////////////////////////////////
		bool parse_effect(const char*& ptr)
		{
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//name technique
			std::string new_e_name;
			//get name
			if (!parse_cstring(m_context->m_line,ptr, &ptr, new_e_name))
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
			skeep_space_end_comment(m_context->m_line, ptr);
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				//read all values
				while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
				{
					//skeep spaces
					skeep_space_end_comment(m_context->m_line, ptr);
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
					skeep_space_end_comment(m_context->m_line, ptr);
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
			skeep_space_end_comment(m_context->m_line, ptr);
			//parse '('
			if (!is_start_arg(*ptr)) return false;
			//jump '('
			++ptr;
			//space
			skeep_space_end_comment(m_context->m_line, ptr);
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
			skeep_space_end_comment(m_context->m_line, ptr);
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
			if (!parse_cstring(m_context->m_line, ptr, &ptr, field.m_value.m_texture))
			{
				return false;
			}
			return true;
		}

		bool parse_vec2(const char*& ptr, parameter_field& field)
		{
			if (!parse_float(ptr, &ptr, field.m_value.m_vec2.x)) return false;

			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec2.y)) return false;

			return true;
		}

		bool parse_vec3(const char*& ptr, parameter_field& field)
		{
			if (!parse_float(ptr, &ptr, field.m_value.m_vec3.x)) return false;

			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec3.y)) return false;

			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec3.z)) return false;

			return true;
		}

		bool parse_vec4(const char*& ptr, parameter_field& field)
		{
			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.x)) return false;

			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.y)) return false;

			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.z)) return false;

			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.w)) return false;

			return true;
		}

		bool parse_mat4(const char*& ptr, parameter_field& field)
		{
			//first
			if (!parse_float(ptr, &ptr, field.m_value.m_mat4[0][0])) return false;
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
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
					skeep_space_end_comment(m_context->m_line, ptr);
					if (!is_comm_arg(*ptr)) return false; else ++ptr;
					skeep_space_end_comment(m_context->m_line, ptr);
					//parse value
					if (!parse_float(ptr, &ptr, field.m_value.m_mat4[x][y])) return false;
				}

			return true;
		}
		//////////////////////////////////////////////////////
		bool parse_type(const char* in,
						const char** cout,
						parameter_type& type)
		{
			if (cstr_cmp(in, "int"))
			{
				(*cout) += 3; type = parameter_type::PT_INT;  return true;
			}

			if (cstr_cmp(in, "float"))
			{
				(*cout) += 5; type = parameter_type::PT_FLOAT;  return true;
			}

			if (cstr_cmp(in, "texture"))
			{
				(*cout) += 7; type = parameter_type::PT_TEXTURE;  return true;
			}

			if (cstr_cmp(in, "vec2"))
			{
				(*cout) += 4; type = parameter_type::PT_VEC2;  return true;
			}

			if (cstr_cmp(in, "vec3"))
			{
				(*cout) += 4; type = parameter_type::PT_VEC3;  return true;
			}

			if (cstr_cmp(in, "vec4"))
			{
				(*cout) += 4; type = parameter_type::PT_VEC4;  return true;
			}

			if (cstr_cmp(in, "mat4"))
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

}
}