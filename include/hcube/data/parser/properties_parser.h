#pragma once
#include <vector>
#include <hcube/data/property.h>
#include <hcube/data/parser/utils_parser.h>
#include <hcube/data/parser/variant_parser.h>

namespace hcube
{
namespace parser
{

	class properties_parser : protected utils_parser
	{
	public:

		struct error_field
		{
			size_t m_line{ 0 };
			std::string m_error;

			error_field()
			{
			}
			error_field(size_t line, const std::string& error)
				: m_line(line)
				, m_error(error)
			{
			}
		};

		template<typename T>
		bool parse(const std::string& str, T& obj)
		{
			return parser(str.c_str(), (void*)&obj, T::properties());
		}

		template<typename T>
		bool parse(const char* ptr, T& obj)
		{
			return parser(ptr, (void*)&obj, T::properties());
		}

		template<typename T>
		bool parse(const std::string& str, T& obj, std::vector< property* >& properties)
		{
			return parser(str.c_str(), (void*)&obj, properties);
		}

		template<typename T>
		bool parse(const char* ptr, T& obj, std::vector< property* >& properties)
		{
			return parser(ptr, (void*)&obj, properties);
		}

	protected:
		
		bool parser(const char* ptr, void* obj, const std::vector< property* >& properties)
		{
			//info
			size_t line = 0;
			std::string name;
			//variant parser
			variant_parser			v_parser;
			variant_parser::context v_parser_ctx;
			//loop
			while (*ptr)
			{
				//skeep
				skeep_space_end_comment(line, ptr);
				//name
				if (!parse_name(ptr, &ptr, name)) return false;
				//skeep
				parser::utils_parser::skeep_space_end_comment(line, ptr);
				//parse
				if (!v_parser.parse(v_parser_ctx,ptr)) return false;
				//add lines
				line += v_parser_ctx.m_line;
				//search
				for (property* p : properties)
				{
					if (p->get_name() == name)
					{
						p->set(obj, v_parser_ctx.m_variant);
						break;
					}
				}
			}
			//success
			return true;
		}

	};
}
}