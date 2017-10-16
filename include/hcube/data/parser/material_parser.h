#pragma once
#include <string>
#include <hcube/render/effect.h>
#include <hcube/data/parser/utils_parser.h>
#include <hcube/data/parser/error_field_parser.h>
#include <hcube/data/parser/parameters_block_parser.h>

namespace hcube
{
namespace parser
{

	class material_parser : protected utils_parser
	{
	public:

		using  map_parameters = effect::map_parameters;
		using  map_techniques = effect::map_techniques;
		using  technique = effect::technique;
		using  parameter = effect::parameter;
		using  parameters = effect::parameters;
		using  parameter_type = effect::parameter_type;
		using  parameter_field = parameters_block_parser::parameter_field;

		struct effect_field
		{
			std::string m_name;
			std::vector< parameter_field > m_parameters;
		};

		struct context
		{
			effect_field				m_effect;
			std::list < error_field >	m_errors;
			size_t						m_line{ 0 };

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
		//////////////////////////////////////////////////////
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
			parameters_block_parser fields_parser;
			return fields_parser.parse(
				parameters_block_parser::context
				{
					  m_context->m_effect.m_parameters
					, m_context->m_errors
					, m_context->m_line
				}
				, ptr
			);
		}
		//////////////////////////////////////////////////////
		void push_error(const std::string& error)
		{
			m_context->m_errors.push_front(error_field{ m_context->m_line,error });
		}

	};

}
}