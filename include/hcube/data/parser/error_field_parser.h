#pragma once
#include <string>

namespace hcube {
namespace parser {

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

}
}