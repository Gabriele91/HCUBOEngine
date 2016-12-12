#pragma once
#include <ostream>
#include <sstream>
#include <vector>
#include <hcube/data/variant.h>
#include <hcube/data/dump/cstring_dump.h>

namespace hcube
{
	namespace dump
	{
		inline std::string variant_type_dump(const variant_ref& value)
		{
			switch (value.get_type())
			{
			case hcube::VR_NONE: return "none";
			case hcube::VR_CHAR: return "char";
			case hcube::VR_SHORT: return "short";
			case hcube::VR_INT: return "int";
			case hcube::VR_LONG: return "long";
			case hcube::VR_LONGLONG: return "longlong";
			case hcube::VR_UCHAR: return "uchar";
			case hcube::VR_USHORT: return "ushort";
			case hcube::VR_UINT: return "uint";
			case hcube::VR_ULONG: return "ulong";
			case hcube::VR_ULONGLONG: return "ulonglong";
			case hcube::VR_FLOAT: return "float";
			case hcube::VR_DOUBLE: return "double";
			case hcube::VR_VEC2: return "vec2";
			case hcube::VR_VEC3: return "vec3";
			case hcube::VR_VEC4: return "vec4";
			case hcube::VR_QUAT: return "quat";
			case hcube::VR_IVEC2: return "ivec2";
			case hcube::VR_IVEC3: return "ivec3";
			case hcube::VR_IVEC4: return "ivec4";
			case hcube::VR_DVEC2: return "dvec2";
			case hcube::VR_DVEC3: return "dvec3";
			case hcube::VR_DVEC4: return "dvec4";
			case hcube::VR_DQUAT: return "dquat";
			case hcube::VR_MAT3: return "mat3";
			case hcube::VR_MAT4: return "mat4";
			case hcube::VR_DMAT3: return "dmat3";
			case hcube::VR_DMAT4: return "dmat4";
			case hcube::VR_STD_VECTOR_INT: return "int[]";
			case hcube::VR_STD_VECTOR_FLOAT: return "float[]";
			case hcube::VR_STD_VECTOR_VEC2: return "vec2[]";
			case hcube::VR_STD_VECTOR_VEC3: return "vec3[]";
			case hcube::VR_STD_VECTOR_VEC4: return "vec4[]";
			case hcube::VR_C_STRING: return "string";
			case hcube::VR_STD_STRING: return "string";
			case hcube::VR_STD_VECTOR_STRING: return "string[]";
			case hcube::VR_PTR: return "ptr";
			default: return "";
			}
		}
		
		inline std::string variant_value_dump(const variant_ref& value)
		{
			switch (value.get_type())
			{
			case hcube::VR_NONE: return "";
			case hcube::VR_CHAR: return ""; //todo
			case hcube::VR_SHORT: return std::to_string(value.get<short>());
			case hcube::VR_INT: return std::to_string(value.get<int>());
			case hcube::VR_LONG: return std::to_string(value.get<long>());
			case hcube::VR_LONGLONG: return std::to_string(value.get<long long>());
			case hcube::VR_UCHAR: return  ""; //todo
			case hcube::VR_USHORT: return std::to_string(value.get<unsigned short>());
			case hcube::VR_UINT: return std::to_string(value.get<unsigned int>());
			case hcube::VR_ULONG: return std::to_string(value.get<unsigned long>());
			case hcube::VR_ULONGLONG: return std::to_string(value.get<unsigned long long>());
			case hcube::VR_FLOAT: return std::to_string(value.get<float>());
			case hcube::VR_DOUBLE: return std::to_string(value.get<double>());
			case hcube::VR_VEC2: 
				return std::to_string(value.get<vec2>().x)+", "+
					   std::to_string(value.get<vec2>().y);
			case hcube::VR_VEC3:
				return std::to_string(value.get<vec3>().x) + ", " +
					   std::to_string(value.get<vec3>().y) + ", " +
                       std::to_string(value.get<vec3>().z);
			case hcube::VR_VEC4:
				return std::to_string(value.get<vec4>().x) + ", " +
					   std::to_string(value.get<vec4>().y) + ", " +
					   std::to_string(value.get<vec4>().z) + ", " +
					   std::to_string(value.get<vec4>().w);
			case hcube::VR_QUAT:
				return std::to_string(value.get<quat>().x) + ", " +
					   std::to_string(value.get<quat>().y) + ", " +
					   std::to_string(value.get<quat>().z) + ", " +
					   std::to_string(value.get<quat>().w);
			case hcube::VR_IVEC2:
				return std::to_string(value.get<ivec2>().x) + ", " +
					   std::to_string(value.get<ivec2>().y);
			case hcube::VR_IVEC3:
				return std::to_string(value.get<ivec3>().x) + ", " +
					   std::to_string(value.get<ivec3>().y) + ", " +
					   std::to_string(value.get<ivec3>().z);
			case hcube::VR_IVEC4:
				return std::to_string(value.get<ivec4>().x) + ", " +
					   std::to_string(value.get<ivec4>().y) + ", " +
					   std::to_string(value.get<ivec4>().z) + ", " +
					   std::to_string(value.get<ivec4>().w);
			case hcube::VR_DVEC2:
				return std::to_string(value.get<dvec2>().x) + ", " +
					   std::to_string(value.get<dvec2>().y);
			case hcube::VR_DVEC3:
				return std::to_string(value.get<dvec3>().x) + ", " +
					   std::to_string(value.get<dvec3>().y) + ", " +
					   std::to_string(value.get<dvec3>().z);
			case hcube::VR_DVEC4:
				return std::to_string(value.get<dvec4>().x) + ", " +
					   std::to_string(value.get<dvec4>().y) + ", " +
					   std::to_string(value.get<dvec4>().z) + ", " +
					   std::to_string(value.get<dvec4>().w);
			case hcube::VR_DQUAT:
				return std::to_string(value.get<dquat>().x) + ", " +
					   std::to_string(value.get<dquat>().y) + ", " +
					   std::to_string(value.get<dquat>().z) + ", " +
					   std::to_string(value.get<dquat>().w);
			case hcube::VR_MAT3:
			{
				const size_t matrix_size = 3;
				std::string out;
				const auto& matrix = value.get<mat3>();
				for (size_t y = 0; y != matrix_size; ++y)
				for (size_t x = 0; x != matrix_size; ++x)
				{
					if (y == (matrix_size - 1) && x == (matrix_size - 1))
						out += std::to_string(matrix[x][y]);
					else
						out += std::to_string(matrix[x][y]) + ", ";
				}
				return out;
			}
			case hcube::VR_MAT4:
			{
				const size_t matrix_size = 4;
				std::string out;
				const auto& matrix = value.get<mat4>();
				for (size_t y = 0; y != matrix_size; ++y)
				for (size_t x = 0; x != matrix_size; ++x)
				{
					if (y == (matrix_size-1) && x == (matrix_size-1))
						out += std::to_string(matrix[x][y]);
					else
						out += std::to_string(matrix[x][y]) + ", ";
				}
				return out;
			}
			case hcube::VR_DMAT3:
			{
				const size_t matrix_size = 3;
				std::string out;
				const auto& matrix = value.get<dmat3>();
				for (size_t y = 0; y != matrix_size; ++y)
				for (size_t x = 0; x != matrix_size; ++x)
				{
					if (y == (matrix_size - 1) && x == (matrix_size - 1))
						out += std::to_string(matrix[x][y]);
					else
						out += std::to_string(matrix[x][y]) + ", ";
				}
				return out;
			}
			case hcube::VR_DMAT4:
			{
				const size_t matrix_size = 4;
				std::string out;
				const auto& matrix = value.get<dmat4>();
				for (size_t y = 0; y != matrix_size; ++y)
				for (size_t x = 0; x != matrix_size; ++x)
				{
					if (y == (matrix_size - 1) && x == (matrix_size - 1))
						out += std::to_string(matrix[x][y]);
					else
						out += std::to_string(matrix[x][y]) + ", ";
				}
				return out;
			}
			case hcube::VR_STD_VECTOR_INT:
			{
				std::string out;
				const auto& vector = value.get< std::vector< int > >();
				for (size_t i= 0; i != vector.size(); ++i)
				{
					if (i != (vector.size() - 1))
						out += std::to_string(vector[i]) + ", ";
					else
						out += std::to_string(vector[i]);
				}
				return out;
			}
			case hcube::VR_STD_VECTOR_FLOAT:
			{
				std::string out;
				const auto& vector = value.get< std::vector< float > >();
				for (size_t i = 0; i != vector.size(); ++i)
				{
					if (i != (vector.size() - 1))
						out += std::to_string(vector[i]) + ", ";
					else
						out += std::to_string(vector[i]);
				}
				return out;
			}
			case hcube::VR_STD_VECTOR_VEC2:
			{
				std::string out;
				const auto& vector = value.get< std::vector< vec2 > >();
				for (size_t i = 0; i != vector.size(); ++i)
				{
					if (i != (vector.size() - 1))
						out += "{ " + std::to_string(vector[i].x) + ", "
						+ std::to_string(vector[i].y) + "} , ";
					else
						out += "{ " + std::to_string(vector[i].x) + ", "
						+ std::to_string(vector[i].y) + "} ";
				}
				return out;
			}
			case hcube::VR_STD_VECTOR_VEC3:
			{
				std::string out;
				const auto& vector = value.get< std::vector< vec3 > >();
				for (size_t i = 0; i != vector.size(); ++i)
				{
					if (i != (vector.size() - 1))
						out += "{ " + std::to_string(vector[i].x) + ", "
									+ std::to_string(vector[i].y) + ", "
									+ std::to_string(vector[i].z) + "} , ";
					else
						out += "{ " + std::to_string(vector[i].x) + ", "
									+ std::to_string(vector[i].y) + ", "
									+ std::to_string(vector[i].z) + "} ";
				}
				return out;
			}
			case hcube::VR_STD_VECTOR_VEC4:
			{
				std::string out;
				const auto& vector = value.get< std::vector< vec4 > >();
				for (size_t i = 0; i != vector.size(); ++i)
				{
					if (i != (vector.size() - 1))
						out += "{ " + std::to_string(vector[i].x) + ", "
									+ std::to_string(vector[i].y) + ", "
									+ std::to_string(vector[i].z) + ", "
									+ std::to_string(vector[i].w) + "} , ";
					else
						out += "{ " + std::to_string(vector[i].x) + ", "
									+ std::to_string(vector[i].y) + ", "
									+ std::to_string(vector[i].z) + "} ";
				}
				return out;
			}
			case hcube::VR_C_STRING: 
			case hcube::VR_STD_STRING:
				return hcube::dump::cstring_dump(value.get< std::string >());

			case hcube::VR_STD_VECTOR_STRING: 
			{
				std::string out;
				const auto& vector = value.get< std::vector< std::string > >();
				for (size_t i = 0; i != vector.size(); ++i)
				{
					if (i != (vector.size() - 1))
						out += hcube::dump::cstring_dump(vector[i]) +  ", ";
					else
						out += hcube::dump::cstring_dump(vector[i]);
				}
				return out;
			}
			case hcube::VR_PTR:
				return std::to_string((unsigned long long)value.get< void* >());

			default: return "";
			}

		}
		
		inline std::string variant_dump(const variant& v)
		{
			variant_ref ref_v(v);
			return variant_type_dump(ref_v) + "(" + variant_value_dump(ref_v) + ")";
		}		

		inline std::string variant_dump(const variant_ref& v)
		{
			return variant_type_dump(v) + "(" + variant_value_dump(v) + ")";
		}

		inline std::ostream& operator << (std::ostream& sstr, const variant& v)
		{
			return (sstr << variant_dump(v));
		}

		inline std::ostream& operator << (std::ostream& sstr, const variant_ref& v)
		{
			return (sstr << variant_dump(v));
		}
	}
}