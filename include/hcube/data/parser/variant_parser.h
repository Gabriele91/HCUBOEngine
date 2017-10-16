#pragma once
#include <list>
#include <tuple>
#include <string>
#include <vector>
#include <sstream>
#include <hcube/config.h>
#include <hcube/data/variant.h>
#include <hcube/data/parser/utils_parser.h>

namespace hcube
{
namespace parser
{
		class variant_parser : protected utils_parser
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

			struct context
			{
				variant						m_variant;
				std::list < error_field >   m_errors;
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
				m_context->m_variant = variant();
				//skeep line and comments
				skeep_space_end_comment(m_context->m_line, ptr);
				//ok
				return parse_variant(ptr);
			}

		protected:

			//////////////////////////////////////////////////////
			context* m_context{ nullptr };
			//////////////////////////////////////////////////////
			static inline int  is_a_complex_variant_type(variant_type type)
			{
				switch (type)
				{
				case hcube::VR_CHAR:
				case hcube::VR_SHORT:
				case hcube::VR_INT:
				case hcube::VR_LONG:
				case hcube::VR_LONGLONG:
				case hcube::VR_UCHAR:
				case hcube::VR_USHORT:
				case hcube::VR_UINT:
				case hcube::VR_ULONG:
				case hcube::VR_ULONGLONG:
				case hcube::VR_FLOAT:
				case hcube::VR_DOUBLE:
				case hcube::VR_C_STRING:
				case hcube::VR_STD_STRING:
					return 0;
					break;
				case hcube::VR_VEC2:
				case hcube::VR_VEC3:
				case hcube::VR_VEC4:
				case hcube::VR_QUAT:
				case hcube::VR_IVEC2:
				case hcube::VR_IVEC3:
				case hcube::VR_IVEC4:
				case hcube::VR_DVEC2:
				case hcube::VR_DVEC3:
				case hcube::VR_DVEC4:
				case hcube::VR_DQUAT:
				case hcube::VR_MAT3:
				case hcube::VR_MAT4:
				case hcube::VR_DMAT3:
				case hcube::VR_DMAT4:
					return 1;
					break;
				case hcube::VR_NONE:
				case hcube::VR_PTR:
				case hcube::VR_STD_VECTOR_INT:
				case hcube::VR_STD_VECTOR_FLOAT:
				case hcube::VR_STD_VECTOR_VEC2:
				case hcube::VR_STD_VECTOR_VEC3:
				case hcube::VR_STD_VECTOR_VEC4:
				case hcube::VR_STD_VECTOR_STRING:
				default:
					return -1;
					break;
				}
			}

			static inline bool is_a_value_variant_type(variant_type type)
			{
				switch (type)
				{
				case hcube::VR_NONE:
				case hcube::VR_PTR:
				case hcube::VR_CHAR:
				case hcube::VR_SHORT:
				case hcube::VR_INT:
				case hcube::VR_LONG:
				case hcube::VR_LONGLONG:
				case hcube::VR_UCHAR:
				case hcube::VR_USHORT:
				case hcube::VR_UINT:
				case hcube::VR_ULONG:
				case hcube::VR_ULONGLONG:
				case hcube::VR_FLOAT:
				case hcube::VR_DOUBLE:
				case hcube::VR_C_STRING:
				case hcube::VR_STD_STRING:
				case hcube::VR_VEC2:
				case hcube::VR_VEC3:
				case hcube::VR_VEC4:
				case hcube::VR_QUAT:
				case hcube::VR_IVEC2:
				case hcube::VR_IVEC3:
				case hcube::VR_IVEC4:
				case hcube::VR_DVEC2:
				case hcube::VR_DVEC3:
				case hcube::VR_DVEC4:
				case hcube::VR_DQUAT:
				case hcube::VR_MAT3:
				case hcube::VR_MAT4:
				case hcube::VR_DMAT3:
				case hcube::VR_DMAT4:
					return true;
					//else
				default: return false;
				}
			}

			static inline bool is_a_vector_variant_type(variant_type type)
			{
				switch (type)
				{
					//vector types
				case hcube::VR_STD_VECTOR_INT:
				case hcube::VR_STD_VECTOR_FLOAT:
				case hcube::VR_STD_VECTOR_VEC2:
				case hcube::VR_STD_VECTOR_VEC3:
				case hcube::VR_STD_VECTOR_VEC4:
				case hcube::VR_STD_VECTOR_STRING:
					return true;
					//else
				default: return false;
				}
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
				if (!parse_type<typename T::value_type>(line, ptr, field.x)) return false;

				skeep_space_end_comment(line, ptr);
				if (!is_comm_arg(*ptr)) return false; else ++ptr;
				skeep_space_end_comment(line, ptr);

				if (!parse_type<typename T::value_type>(line, ptr, field.y)) return false;

				return true;
			}

			template < typename T = vec3 >
			static bool parse_vec3(size_t& line, const char*& ptr, T& field)
			{
				if (!parse_type<typename T::value_type>(line, ptr, field.x)) return false;

				skeep_space_end_comment(line, ptr);
				if (!is_comm_arg(*ptr)) return false; else ++ptr;
				skeep_space_end_comment(line, ptr);

				if (!parse_type<typename T::value_type>(line, ptr, field.y)) return false;

				skeep_space_end_comment(line, ptr);
				if (!is_comm_arg(*ptr)) return false; else ++ptr;
				skeep_space_end_comment(line, ptr);

				if (!parse_type<typename T::value_type>(line, ptr, field.z)) return false;

				return true;
			}

			template < typename T = vec4 >
			static bool parse_vec4(size_t& line, const char*& ptr, T& field)
			{
				if (!parse_type<typename T::value_type>(line, ptr, field.x)) return false;

				skeep_space_end_comment(line, ptr);
				if (!is_comm_arg(*ptr)) return false; else ++ptr;
				skeep_space_end_comment(line, ptr);

				if (!parse_type<typename T::value_type>(line, ptr, field.y)) return false;

				skeep_space_end_comment(line, ptr);
				if (!is_comm_arg(*ptr)) return false; else ++ptr;
				skeep_space_end_comment(line, ptr);

				if (!parse_type<typename T::value_type>(line, ptr, field.z)) return false;

				skeep_space_end_comment(line, ptr);
				if (!is_comm_arg(*ptr)) return false; else ++ptr;
				skeep_space_end_comment(line, ptr);

				if (!parse_type<typename T::value_type>(line, ptr, field.w)) return false;

				return true;
			}

			template < typename T = quat >
			static bool parse_quat(size_t& line, const char*& ptr, T& field)
			{
				if (!parse_type<typename T::value_type>(line, ptr, field.x)) return false;

				skeep_space_end_comment(line, ptr);
				if (!is_comm_arg(*ptr)) return false; else ++ptr;
				skeep_space_end_comment(line, ptr);

				if (!parse_type<typename T::value_type>(line, ptr, field.y)) return false;

				skeep_space_end_comment(line, ptr);
				if (!is_comm_arg(*ptr)) return false; else ++ptr;
				skeep_space_end_comment(line, ptr);

				if (!parse_type<typename T::value_type>(line, ptr, field.z)) return false;

				skeep_space_end_comment(line, ptr);
				if (!is_comm_arg(*ptr)) return false; else ++ptr;
				skeep_space_end_comment(line, ptr);

				if (!parse_type<typename T::value_type>(line, ptr, field.w)) return false;

				return true;
			}

			template < typename T = mat3 >
			static bool parse_mat3(size_t& line, const char*& ptr, T& field)
			{
				//first
				if (!parse_type<typename T::value_type>(line, ptr, field[0][0])) return false;
				//skeep spaces
				skeep_space_end_comment(line, ptr);
				// if one param, call mat4 constructor
				if (!is_comm_arg(*ptr))
				{
					// is 'T('<typename T::value_type>')'
					field = T(field[0][0]);
					//success
					return true;
				}
				//for all
				for (int y = 0; y != 3; ++y)
				for (int x = 0; x != 3; ++x)
				{
					//jmp first
					if ((x + y) == 0) continue;
					//parse ','
					skeep_space_end_comment(line, ptr);
					if (!is_comm_arg(*ptr)) return false; else ++ptr;
					skeep_space_end_comment(line, ptr);
					//parse value
					if (!parse_type<typename T::value_type>(line, ptr, field[x][y])) return false;
				}

				return true;
			}

			template < typename T = mat4 >
			static bool parse_mat4(size_t& line, const char*& ptr, T& field)
			{
				//first
				if (!parse_type<typename T::value_type>(line, ptr, field[0][0])) return false;
				//skeep spaces
				skeep_space_end_comment(line, ptr);
				// if one param, call mat4 constructor
				if (!is_comm_arg(*ptr))
				{
					// is 'T('<typename T::value_type>')'
					field = T(field[0][0]);
					//success
					return true;
				}
				//for all
				for (int y = 0; y != 4; ++y)
				for (int x = 0; x != 4; ++x)
				{
					//jmp first
					if ((x + y) == 0) continue;
					//parse ','
					skeep_space_end_comment(line, ptr);
					if (!is_comm_arg(*ptr)) return false; else ++ptr;
					skeep_space_end_comment(line, ptr);
					//parse value
					if (!parse_type<typename T::value_type>(line, ptr, field[x][y])) return false;
				}

				return true;
			}

			template < typename T = std::vector<int> >
			static bool parse_vector(size_t& line, const char*& ptr, T& field)
			{
				//not jump ',' at start
				bool jmp_comm = false;
				//loop
				do
				{
					//jmp ','
					if (jmp_comm) ++ptr; else jmp_comm = true;
					//alloc
					typename T::value_type value;
					//is a complex type
					int is_complex = is_a_complex_variant_type(static_variant_type<typename T::value_type>());
					//is a vector type (fail)
					if (is_complex == -1) return false;
					//type
					if (is_complex)
					{
						//get type
						variant_type type;
						//skeep spaces
						skeep_space_end_comment(line, ptr);
						//parse value
						if (is_start_table(*ptr))
						{
							//skeep '{'
							++ptr;
							//skeep spaces
							skeep_space_end_comment(line, ptr);
							//parse
							if (parse_type<typename T::value_type>(line, ptr, value)) field.push_back(value); else return false;
							//skeep spaces
							skeep_space_end_comment(line, ptr);
							//test and skeep  '}'
							if (is_end_table(*ptr)) ++ptr; else return false;
							//skeep spaces
							skeep_space_end_comment(line, ptr);
						}
						else if (parse_variant_type(ptr, &ptr, type) && type == static_variant_type<typename T::value_type>())
						{
							//skeep spaces
							skeep_space_end_comment(line, ptr);
							//test and skeep '('
							if (is_start_arg(*ptr)) ++ptr; else return false;
							//skeep spaces
							skeep_space_end_comment(line, ptr);
							//parse
							if (parse_type<typename T::value_type>(line, ptr, value)) field.push_back(value); else return false;
							//skeep spaces
							skeep_space_end_comment(line, ptr);
							//test and skeep ')'
							if (is_end_arg(*ptr)) ++ptr; else return false;
							//skeep spaces
							skeep_space_end_comment(line, ptr);
						}
						else return false;
					}
					else
					{
						//skeep spaces
						skeep_space_end_comment(line, ptr);
						//parse
						if (parse_type<typename T::value_type>(line, ptr, value)) field.push_back(value); else return false;
						//skeep spaces
						skeep_space_end_comment(line, ptr);
					}
				} while (is_comm_arg(*ptr));
				//ok
				return true;
			}
			//////////////////////////////////////////////////////
			// VARIANT UTILS
			using variant_generic_type = std::vector < std::tuple< std::string, variant_type  > >;

			static bool parse_variant_generic_type(const variant_generic_type& g_types, const char* in, const char** cout, variant_type& type)
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

				case variant_type::VR_STD_VECTOR_INT:    if (!parse_vector(line, ptr, field.get<std::vector<int>>())) return false; break;
				case variant_type::VR_STD_VECTOR_FLOAT:  if (!parse_vector(line, ptr, field.get<std::vector<float>>())) return false; break;
				case variant_type::VR_STD_VECTOR_VEC2:   if (!parse_vector(line, ptr, field.get<std::vector<vec2>>())) return false; break;
				case variant_type::VR_STD_VECTOR_VEC3:   if (!parse_vector(line, ptr, field.get<std::vector<vec3>>())) return false; break;
				case variant_type::VR_STD_VECTOR_VEC4:   if (!parse_vector(line, ptr, field.get<std::vector<vec4>>())) return false; break;
				case variant_type::VR_STD_VECTOR_STRING: if (!parse_vector(line, ptr, field.get<std::vector<std::string>>())) return false; break;

				default: return false; break;
				}
				//jump space
				skeep_space_end_comment(line, ptr);
				return true;
			}

			bool parse_variant(const char*& ptr, const variant_generic_type& g_types = variant_generic_type())
			{
				//get type
				variant_type type;
				//parse type
				if (!parse_variant_type(ptr, &ptr, type))
				{
					if (!g_types.size() || !parse_variant_generic_type(g_types, ptr, &ptr, type))
					{
						push_error("Not valid type");
						return false;
					}
				}
				//alloc
				m_context->m_variant.copy_from(variant(type));
				//skeep spaces
				skeep_space_end_comment(m_context->m_line, ptr);
				//parse '(' or '{'
				if (is_start_table(*ptr) && is_a_vector_variant_type(type))
				{
					//jump '{'
					++ptr;
					//parse args
					if(!parse_variant_args(m_context->m_line, ptr, m_context->m_variant))
					{
						push_error("Fail to parse arguments");
						return false;
					}
					//parse '}'
					if (!is_end_table(*ptr))
					{
						push_error("Not find character:/\'}\'");
						return false;
					}
					//jump '}'
					++ptr;
					//...
					return true;
				}
				else if (is_start_arg(*ptr))
				{
					//jump '('
					++ptr;
					//parse args
					if(!parse_variant_args(m_context->m_line, ptr, m_context->m_variant))
					{
						push_error("Fail to parse arguments");
						return false;
					}
					//parse ')'
					if (!is_end_arg(*ptr))
					{
						push_error("Not find character:/\')\'");
						return false;
					}
					//jump ')'
					++ptr;
					//...
					return true;
				}
				else
				{
					push_error("Not find character:\'(\'/\'{\'");
					return false;
				}
			}

			//////////////////////////////////////////////////////
			void push_error(const std::string& error)
			{
				m_context->m_errors.push_front(error_field{ m_context->m_line,error });
			}
	};		
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, float& out)
	{
		return parse_float(inout, &inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, double& out)
	{
		return parse_double(inout, &inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, bool& out)
	{
		return parse_bool(inout, &inout,out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line,  const char*& inout, short& out)
	{
		return parse_short(inout, &inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, int& out)
	{
		return parse_int(inout, &inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, long& out)
	{
		return parse_long(inout, &inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, long long& out)
	{
		return parse_long_long(inout, &inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, unsigned short& out)
	{
		return parse_unsigned_short(inout, &inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, unsigned int& out)
	{
		return parse_unsigned_int(inout, &inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, unsigned long& out)
	{
		return parse_unsigned_long(inout, &inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, unsigned long long& out)
	{
		return parse_unsigned_long_long(inout, &inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, std::string& out)
	{
		return parse_cstring(line, inout, &inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, vec2& out)
	{
		return parse_vec2(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, vec3& out)
	{
		return parse_vec3(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, vec4& out)
	{
		return parse_vec4(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, quat& out)
	{
		return parse_quat(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, mat3& out)
	{
		return parse_mat3(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, mat4& out)
	{
		return parse_mat4(line, inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, ivec2& out)
	{
		return parse_vec2(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, ivec3& out)
	{
		return parse_vec3(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, ivec4& out)
	{
		return parse_vec4(line, inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, dvec2& out)
	{
		return parse_vec2(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, dvec3& out)
	{
		return parse_vec3(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, dvec4& out)
	{
		return parse_vec4(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, dquat& out)
	{
		return parse_quat(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, dmat3& out)
	{
		return parse_mat3(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, dmat4& out)
	{
		return parse_mat4(line, inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, std::vector< int >& out)
	{
		return parse_vector(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, std::vector< float >& out)
	{
		return parse_vector(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, std::vector< vec2 >& out)
	{
		return parse_vector(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, std::vector< vec3 >& out)
	{
		return parse_vector(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, std::vector< vec4 >& out)
	{
		return parse_vector(line, inout, out);
	}
	template <>
	inline bool variant_parser::parse_type(size_t& line, const char*& inout, std::vector< std::string >& out)
	{
		return parse_vector(line, inout, out);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
}
}
