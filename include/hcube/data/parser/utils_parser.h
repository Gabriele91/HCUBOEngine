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
		//////////////////////////////////////////////////////
		template < typename T >
		static bool parse_type(size_t& line, const char*& inout, T& out)
		{
			return false;
		}

		template < typename T = vec2 >
		static bool parse_vec2(size_t& line, const char*& ptr, T& field)
		{
			if (!parse_type<T::value_type>(line, ptr, field.x)) return false;

			skeep_space_end_comment(line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(line, ptr);

			if (!parse_type<T::value_type>(line, ptr, field.y)) return false;

			return true;
		}

		template < typename T = vec3 >
		static bool parse_vec3(size_t& line, const char*& ptr, T& field)
		{
			if (!parse_type<T::value_type>(line, ptr, field.x)) return false;

			skeep_space_end_comment(line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(line, ptr);

			if (!parse_type<T::value_type>(line, ptr, field.y)) return false;

			skeep_space_end_comment(line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(line, ptr);

			if (!parse_type<T::value_type>(line, ptr, field.z)) return false;

			return true;
		}

		template < typename T = vec4 >
		static bool parse_vec4(size_t& line, const char*& ptr, T& field)
		{
			if (!parse_type<T::value_type>(line, ptr, field.x)) return false;

			skeep_space_end_comment(line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(line, ptr);

			if (!parse_type<T::value_type>(line, ptr, field.y)) return false;

			skeep_space_end_comment(line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(line, ptr);

			if (!parse_type<T::value_type>(line, ptr, field.z)) return false;

			skeep_space_end_comment(line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(line, ptr);

			if (!parse_type<T::value_type>(line, ptr, field.w)) return false;

			return true;
		}

		template < typename T = quat >
		static bool parse_quat(size_t& line, const char*& ptr, T& field)
		{
			if (!parse_type<T::value_type>(line, ptr, field.x)) return false;

			skeep_space_end_comment(line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(line, ptr);

			if (!parse_type<T::value_type>(line, ptr, field.y)) return false;

			skeep_space_end_comment(line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(line, ptr);

			if (!parse_type<T::value_type>(line, ptr, field.z)) return false;

			skeep_space_end_comment(line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(line, ptr);

			if (!parse_type<T::value_type>(line, ptr, field.w)) return false;

			return true;
		}

		template < typename T = mat3 >
		static bool parse_mat3(size_t& line, const char*& ptr, T& field)
		{
			//first
			if (!parse_type<T::value_type>(line, ptr, field[0][0])) return false;
			//skeep spaces
			skeep_space_end_comment(line, ptr);
			// if one param, call mat4 constructor
			if (!is_comm_arg(*ptr))
			{
				// is 'T('<T::value_type>')'
				field = T(field[0][0]);
				//success
				return true;
			}
			//for all
			for (int x = 0; x != 3; ++x)
			for (int y = 0; y != 3; ++y)
			{
				//jmp first
				if ((x + y) == 0) continue;
				//parse ','
				skeep_space_end_comment(line, ptr);
				if (!is_comm_arg(*ptr)) return false; else ++ptr;
				skeep_space_end_comment(line, ptr);
				//parse value
				if (!parse_type<T::value_type>(line, ptr, field[x][y])) return false;
			}

			return true;
		}

		template < typename T = mat4 >
		static bool parse_mat4(size_t& line, const char*& ptr, T& field)
		{
			//first
			if (!parse_type<T::value_type>(line, ptr, field[0][0])) return false;
			//skeep spaces
			skeep_space_end_comment(line, ptr);
			// if one param, call mat4 constructor
			if (!is_comm_arg(*ptr))
			{
				// is 'T('<T::value_type>')'
				field = T(field[0][0]);
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
				skeep_space_end_comment(line, ptr);
				if (!is_comm_arg(*ptr)) return false; else ++ptr;
				skeep_space_end_comment(line, ptr);
				//parse value
				if (!parse_type<T::value_type>(line, ptr, field[x][y])) return false;
			}

			return true;
		}

		template < typename T = std::vector<int> >
		static bool parse_vector(size_t& line, const char* ptr, T& field)
		{
			//todo
			return false;
		}
		//////////////////////////////////////////////////////
		// VARIANT UTILS
		using variant_generic_type = std::vector < std::tuple< std::string, variant_type  > >;

		static bool parse_variant_generic_type(const variant_generic_type& g_types,const char* in, const char** cout, variant_type& type)
		{
			for (const auto& this_type : g_types)
				if (cstr_cmp(std::get<0>(this_type).c_str(), in))
				{
					(*cout) += std::get<0>(this_type).length(); type = std::get<1>(this_type);  return true;
				}
			return false;
		}
		
		static bool parse_variant_type(const char* in, const char** cout, variant_type& type)
		{
			//VECTOR INTEGER
			if (cstr_cmp(in, "int[]"))
			{
				(*cout) += 5; type = variant_type::VR_STD_VECTOR_INT;  return true;
			}

			//INTEGER
			if (cstr_cmp(in, "char"))
			{
				(*cout) += 4; type = variant_type::VR_CHAR;  return true;
			}
			if (cstr_cmp(in, "int"))
			{
				(*cout) += 3; type = variant_type::VR_INT;  return true;
			}
			if (cstr_cmp(in, "short"))
			{
				(*cout) += 5; type = variant_type::VR_SHORT;  return true;
			}
			if (cstr_cmp(in, "longlong"))
			{
				(*cout) += 8; type = variant_type::VR_LONGLONG;  return true;
			}
			if (cstr_cmp(in, "long"))
			{
				(*cout) += 4; type = variant_type::VR_LONG;  return true;
			}

			//UINTEGER
			if (cstr_cmp(in, "uchar"))
			{
				(*cout) += 5; type = variant_type::VR_UCHAR;  return true;
			}
			if (cstr_cmp(in, "uint"))
			{
				(*cout) += 4; type = variant_type::VR_UINT;  return true;
			}
			if (cstr_cmp(in, "ushort"))
			{
				(*cout) += 6; type = variant_type::VR_USHORT;  return true;
			}
			if (cstr_cmp(in, "ulonglong"))
			{
				(*cout) += 9; type = variant_type::VR_ULONGLONG;  return true;
			}
			if (cstr_cmp(in, "ulong"))
			{
				(*cout) += 5; type = variant_type::VR_ULONG;  return true;
			}

			//VECTOR FPOINTS
			if (cstr_cmp(in, "float[]"))
			{
				(*cout) += 7; type = variant_type::VR_STD_VECTOR_FLOAT;  return true;
			}

			//FPOINTS
			if (cstr_cmp(in, "float"))
			{
				(*cout) += 5; type = variant_type::VR_FLOAT;  return true;
			}
			if (cstr_cmp(in, "double"))
			{
				(*cout) += 6; type = variant_type::VR_DOUBLE;  return true;
			}

			//VECTOR VEC*
			if (cstr_cmp(in, "vec2[]"))
			{
				(*cout) += 6; type = variant_type::VR_STD_VECTOR_VEC2;  return true;
			}
			if (cstr_cmp(in, "vec3[]"))
			{
				(*cout) += 6; type = variant_type::VR_STD_VECTOR_VEC3;  return true;
			}
			if (cstr_cmp(in, "vec4[]"))
			{
				(*cout) += 6; type = variant_type::VR_STD_VECTOR_VEC4;  return true;
			}

			//VEC
			if (cstr_cmp(in, "vec2"))
			{
				(*cout) += 4; type = variant_type::VR_VEC2;  return true;
			}
			if (cstr_cmp(in, "vec3"))
			{
				(*cout) += 4; type = variant_type::VR_VEC3;  return true;
			}
			if (cstr_cmp(in, "vec4"))
			{
				(*cout) += 4; type = variant_type::VR_VEC4;  return true;
			}

			if (cstr_cmp(in, "mat3"))
			{
				(*cout) += 4; type = variant_type::VR_MAT3;  return true;
			}
			if (cstr_cmp(in, "mat4"))
			{
				(*cout) += 4; type = variant_type::VR_MAT4;  return true;
			}

			if (cstr_cmp(in, "quat"))
			{
				(*cout) += 4; type = variant_type::VR_QUAT;  return true;
			}

			//I VEC
			if (cstr_cmp(in, "ivec2"))
			{
				(*cout) += 5; type = variant_type::VR_IVEC2;  return true;
			}
			if (cstr_cmp(in, "ivec3"))
			{
				(*cout) += 5; type = variant_type::VR_IVEC3;  return true;
			}
			if (cstr_cmp(in, "ivec4"))
			{
				(*cout) += 5; type = variant_type::VR_IVEC4;  return true;
			}

			//D VEC
			if (cstr_cmp(in, "dvec2"))
			{
				(*cout) += 5; type = variant_type::VR_DVEC2;  return true;
			}
			if (cstr_cmp(in, "dvec3"))
			{
				(*cout) += 5; type = variant_type::VR_DVEC3;  return true;
			}
			if (cstr_cmp(in, "dvec4"))
			{
				(*cout) += 5; type = variant_type::VR_DVEC4;  return true;
			}

			if (cstr_cmp(in, "dmat3"))
			{
				(*cout) += 5; type = variant_type::VR_DMAT3;  return true;
			}
			if (cstr_cmp(in, "dmat4"))
			{
				(*cout) += 5; type = variant_type::VR_DMAT4;  return true;
			}

			if (cstr_cmp(in, "dquat"))
			{
				(*cout) += 5; type = variant_type::VR_DQUAT;  return true;
			}

			//VECTOR STRINGS
			if (cstr_cmp(in, "string[]"))
			{
				(*cout) += 8; type = variant_type::VR_STD_VECTOR_STRING;  return true;
			}

			//STRINGS
			if (cstr_cmp(in, "string"))
			{
				(*cout) += 6; type = variant_type::VR_STD_STRING;  return true;
			}


			return false;
		}		
				
		static bool parse_variant_args(size_t& line, const char*& ptr, variant& field)
		{
			//space
			skeep_space_end_comment(line, ptr);
			//parse by type
			switch (field.get_type())
			{
			case variant_type::VR_SHORT:      if (!parse_short(ptr, &ptr, field.get<short>()))			   return false; break;
			case variant_type::VR_INT:        if (!parse_int(ptr, &ptr, field.get<int>()))				   return false; break;
			case variant_type::VR_LONG:       if (!parse_long(ptr, &ptr, field.get<long>()))			   return false; break;
			case variant_type::VR_LONGLONG:   if (!parse_long_long(ptr, &ptr, field.get<long long>()))     return false; break;

			case variant_type::VR_USHORT:      if (!parse_unsigned_short(ptr, &ptr, field.get<unsigned short>()))		   return false; break;
			case variant_type::VR_UINT:        if (!parse_unsigned_int(ptr, &ptr, field.get<unsigned int>()))			   return false; break;
			case variant_type::VR_ULONG:       if (!parse_unsigned_long(ptr, &ptr, field.get<unsigned long>()))			   return false; break;
			case variant_type::VR_ULONGLONG:   if (!parse_unsigned_long_long(ptr, &ptr, field.get<unsigned long long>()))  return false; break;

			case variant_type::VR_FLOAT:	  if (!parse_float(ptr, &ptr, field.get<float>()))             return false; break;
			case variant_type::VR_DOUBLE:	  if (!parse_double(ptr, &ptr, field.get<double>()))           return false; break;

			case variant_type::VR_VEC2:		  if (!parse_vec2(line, ptr, field.get<vec2>())) return false; break;
			case variant_type::VR_VEC3:		  if (!parse_vec3(line, ptr, field.get<vec3>())) return false; break;
			case variant_type::VR_VEC4:		  if (!parse_vec4(line, ptr, field.get<vec4>())) return false; break;
			case variant_type::VR_MAT3:		  if (!parse_mat3(line, ptr, field.get<mat3>())) return false; break;
			case variant_type::VR_MAT4:		  if (!parse_mat4(line, ptr, field.get<mat4>())) return false; break;
			case variant_type::VR_QUAT:		  if (!parse_quat(line, ptr, field.get<quat>())) return false; break;

			case variant_type::VR_IVEC2:	  if (!parse_vec2(line, ptr, field.get<ivec2>())) return false; break;
			case variant_type::VR_IVEC3:	  if (!parse_vec3(line, ptr, field.get<ivec3>())) return false; break;
			case variant_type::VR_IVEC4:	  if (!parse_vec4(line, ptr, field.get<ivec4>())) return false; break;

			case variant_type::VR_DVEC2:	  if (!parse_vec2(line, ptr, field.get<dvec2>())) return false; break;
			case variant_type::VR_DVEC3:	  if (!parse_vec3(line, ptr, field.get<dvec3>())) return false; break;
			case variant_type::VR_DVEC4:	  if (!parse_vec4(line, ptr, field.get<dvec4>())) return false; break;
			case variant_type::VR_DMAT3:	  if (!parse_mat3(line, ptr, field.get<dmat3>())) return false; break;
			case variant_type::VR_DMAT4:	  if (!parse_mat4(line, ptr, field.get<dmat4>())) return false; break;
			case variant_type::VR_DQUAT:	  if (!parse_quat(line, ptr, field.get<dquat>())) return false; break;

			case variant_type::VR_C_STRING:
			case variant_type::VR_STD_STRING: if (!parse_cstring(line, ptr, &ptr, field.get<std::string>())) return false; break;

			default: return false; break;
			}
			//jump space
			skeep_space_end_comment(line, ptr);
		}

		static bool parse_variant(size_t& line,const char*& ptr, variant& field,const variant_generic_type& g_types = variant_generic_type())
		{
			//get type
			variant_type type;
			//parse type
			if (!parse_variant_type(ptr, &ptr, type))
			{
				if(!g_types.size() || !parse_variant_generic_type(g_types, ptr, &ptr, type))
					return false;
			}
			//alloc
			field = variant(type);
			//skeep spaces
			skeep_space_end_comment(line, ptr);
			//parse '('
			if (!is_start_arg(*ptr)) return false;
			//jump '('
			++ptr;
			//parse args
			parse_variant_args(line,ptr,field);
			//parse ')'
			if (!is_end_arg(*ptr))  return false;
			//jump ')'
			++ptr;
			//...
			return true;
		}
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, bool& out)
	{
		return parse_bool(inout, &inout,out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line,  const char*& inout, short& out)
	{
		return parse_short(inout, &inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, int& out)
	{
		return parse_int(inout, &inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, long& out)
	{
		return parse_long(inout, &inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, long long& out)
	{
		return parse_long_long(inout, &inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, unsigned short& out)
	{
		return parse_unsigned_short(inout, &inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, unsigned int& out)
	{
		return parse_unsigned_int(inout, &inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, unsigned long& out)
	{
		return parse_unsigned_long(inout, &inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, unsigned long long& out)
	{
		return parse_unsigned_long_long(inout, &inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, std::string& out)
	{
		return parse_cstring(line, inout, &inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, vec2& out)
	{
		return parse_vec2(line, inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, vec3& out)
	{
		return parse_vec3(line, inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, vec4& out)
	{
		return parse_vec4(line, inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, quat& out)
	{
		return parse_quat(line, inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, mat3& out)
	{
		return parse_mat3(line, inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, mat4& out)
	{
		return parse_mat4(line, inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, ivec2& out)
	{
		return parse_vec2(line, inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, ivec3& out)
	{
		return parse_vec3(line, inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, ivec4& out)
	{
		return parse_vec4(line, inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, dvec2& out)
	{
		return parse_vec2(line, inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, dvec3& out)
	{
		return parse_vec3(line, inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, dvec4& out)
	{
		return parse_vec4(line, inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, dquat& out)
	{
		return parse_quat(line, inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, dmat3& out)
	{
		return parse_mat3(line, inout, out);
	}
	template <>
	inline bool utils_parser::parse_type(size_t& line, const char*& inout, dmat4& out)
	{
		return parse_mat4(line, inout, out);
	}

}
}