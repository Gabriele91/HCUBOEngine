#pragma once
#include <string>
#include <hcube/data/variant.h>
namespace hcube 
{
namespace parser 
{
	class utils_parser
	{
	public:
		//////////////////////////////////////////////////////
		// KEYWORD UTILS
		static inline bool cstr_cmp(const char* x, const char key[])
		{
			return std::strncmp(x, key, std::strlen(key)) == 0;
		}

		static inline bool cstr_cmp_skip(const char*& x, const char key[])
		{
			if (cstr_cmp(x, key))
			{ 
				x += std::strlen(key);
				return true; 
			} 
			return false;
		}

		//////////////////////////////////////////////////////
		// WHO IS UTILS
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
		//////////////////////////////////////////////////////
		// COMMENTS UTILS
		static void skeep_line_comment(const char*& inout)
		{
			if (is_line_comment(inout))
			{
				while (*(inout) != EOF &&
					*(inout) != '\0'&&
					*(inout) != '\n')
					++(inout);
			}
		}

		static void skeep_multy_line_comment(size_t& line,const char*& inout)
		{
			if (is_start_multy_line_comment(inout))
			{
				while (*(inout) != EOF &&
					*(inout) != '\0' &&
					!is_end_multy_line_comment(inout))
				{
					line += (*(inout)) == '\n';
					++(inout);
				}
				if ((*(inout)) == '*') ++(inout);
			}
		}

		static void skeep_space_end_comment(size_t& line, const char*& inout)
		{
			while (is_space(*(inout)) ||
				is_line_comment(inout) ||
				is_start_multy_line_comment(inout))
			{
				skeep_line_comment(inout);
				skeep_multy_line_comment(line,inout);
				line += (*(inout)) == '\n';
				++(inout);
			}
		}

		static void skeep_line_space(const char*& inout)
		{
			while (is_line_space(*inout)) ++(inout);
		}
		//////////////////////////////////////////////////////
		// TYPE UTILS
		static bool parse_bool(const char* in, const char** cout, bool& out)
		{
			if (cstr_cmp(in, "true")) { (*cout) += 4; out = true;  return true; }
			if (cstr_cmp(in, "false")) { (*cout) += 5; out = false; return true; };
			return false;
		}

		static bool parse_short(const char* in, const char** cout, short& out)
		{
			out = (short)std::strtol(in, (char**)cout, 10);
			return in != (*cout);
		}

		static bool parse_int(const char* in, const char** cout, int& out)
		{
			out = (int)std::strtol(in, (char**)cout, 10);
			return in != (*cout);
		}

		static bool parse_long(const char* in, const char** cout, long& out)
		{
			out = std::strtol(in, (char**)cout, 10);
			return in != (*cout);
		}

		static bool parse_long_long(const char* in, const char** cout, long long& out)
		{
			out = std::strtoll(in, (char**)cout, 10);
			return in != (*cout);
		}

		static bool parse_unsigned_short(const char* in, const char** cout, unsigned short& out)
		{
			out = (unsigned short)std::strtoul(in, (char**)cout, 10);
			return in != (*cout);
		}

		static bool parse_unsigned_int(const char* in, const char** cout, unsigned int& out)
		{
			out = (unsigned int)std::strtoul(in, (char**)cout, 10);
			return in != (*cout);
		}

		static bool parse_unsigned_long(const char* in, const char** cout, unsigned long& out)
		{
			out = std::strtoul(in, (char**)cout, 10);
			return in != (*cout);
		}

		static bool parse_unsigned_long_long(const char* in, const char** cout, unsigned long long& out)
		{
			out = std::strtoull(in, (char**)cout, 10);
			return in != (*cout);
		}

		static bool parse_float(const char* in, const char** cout, float& out)
		{
			out = std::strtof(in, (char**)cout);
			return in != (*cout);
		}

		static bool parse_double(const char* in, const char** cout, double& out)
		{
			out = std::strtod(in, (char**)cout);
			return in != (*cout);
		}

		static bool parse_name(const char* in, const char** cout, std::string& out)
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

		static bool parse_cstring(size_t& line,  const char* in, const char** cout, std::string& out)
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
				if (cout) (*cout) = tmp + 1;
				return true;
			}
			return false;
		}

	};
}
}