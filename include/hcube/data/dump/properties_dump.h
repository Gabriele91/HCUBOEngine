#pragma once
#include <string>
#include <vector>
#include <hcube/data/property.h>
#include <hcube/data/dump/variant_dump.h>

namespace hcube
{
namespace dump
{
	inline std::string properties_dump(void* obj,const properties_vector& properties)
	{
		//output
		std::string out;
		//for all
		for (property* p : properties)
		{
			out += std::string(p->get_name()) + " " + variant_dump(p->get(obj)) + "\n";
		}
		return out;
	}

	template < class T >
	inline std::string properties_dump(T& obj,const properties_vector& properties)
	{
		return properties_dump((void*)&obj, properties);
	}

	template < class T >
	inline std::string properties_dump(T& obj)
	{
		return properties_dump((void*)&obj, T::properties());
	}
}
}