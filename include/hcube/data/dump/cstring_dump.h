#pragma once
#include <string>

namespace hcube
{
	namespace dump
	{
		inline std::string cstring_dump(const char* cstr)
		{
			//copy ptr
			const char *tmp = cstr;
			//start '"'
			std::string out = "\"";
			//push all chars
			while (*tmp)
			{
				switch (*tmp)
				{
				case '\n':
					out += "\\n";
					break;
				case '\t':
					out += "\\t";
					break;
				case '\b':
					out += "\\b";
					break;
				case '\r':
					out += "\\r";
					break;
				case '\f':
					out += "\\f";
					break;
				case '\a':
					out += "\\a";
					break;
				case '\\':
					out += "\\\\";
					break;
				case '\?':
					out += "\\?";
					break;
				case '\'':
					out += "\\\'";
					break;
				case '\"':
					out += "\\\"";
					break;
				default:
					out += *tmp;
					break;
				}
				++tmp;
			}
			return out+"\"";
		}
		inline std::string cstring_dump(const std::string& str)
		{
			return cstring_dump(str.c_str());
		}
	}
}