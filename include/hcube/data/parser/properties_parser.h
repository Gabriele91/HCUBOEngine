#pragma once
#include <vector>
#include <hcube/data/property.h>
#include <hcube/data/parser/utils_parser.h>

namespace hcube
{
namespace parser
{
	inline bool properties_parser(const char* ptr, void* obj, const std::vector< property* >& properties)
	{
		size_t line = 0;
		variant value;
		std::string name;
		while (*ptr)
		{
			//skeep
			parser::utils_parser::skeep_space_end_comment(line, ptr);
			//name
			if (!parser::utils_parser::parse_name(ptr, &ptr, name)) return false;
			//skeep
			parser::utils_parser::skeep_space_end_comment(line, ptr);
			//parse
			if (!parser::utils_parser::parse_variant(line, ptr, value)) return false;
			//search
			for (property* p : properties)
			{
				if (p->get_name() == name)
				{
					p->set(obj, value);
					break;
				}
			}
		}
		//success
		return true;
	}

	template<typename T>
	inline bool properties_parser(const char* ptr, T& obj)
	{
		return properties_parser(ptr, (void*)&obj, T::properties());
	}
}
}