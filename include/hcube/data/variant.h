//
//  variant.h
//  HCubo
//
//  Created by Gabriele on 23/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <hcube/config.h>
#include <hcube/math/vector_math.h>

namespace hcube
{

	class variant;
	class variant_ref;
	enum variant_type
	{
		VR_NONE,
		VR_CHAR,
		VR_SHORT,
		VR_INT,
		VR_LONG,
		VR_LONGLONG,

		VR_UCHAR,
		VR_USHORT,
		VR_UINT,
		VR_ULONG,
		VR_ULONGLONG,

		VR_FLOAT,
		VR_DOUBLE,

		VR_VEC2,
		VR_VEC3,
		VR_VEC4,
		VR_QUAT,

		VR_IVEC2,
		VR_IVEC3,
		VR_IVEC4,

		VR_DVEC2,
		VR_DVEC3,
		VR_DVEC4,
		VR_DQUAT,

		//HEAP
		VR_MAT3,
		VR_MAT4,

		VR_DMAT3,
		VR_DMAT4,

		VR_STD_VECTOR_INT,
		VR_STD_VECTOR_FLOAT,
		VR_STD_VECTOR_VEC2,
		VR_STD_VECTOR_VEC3,
		VR_STD_VECTOR_VEC4,

		VR_C_STRING,
		VR_STD_STRING,
		VR_STD_VECTOR_STRING,

		//PTR
		VR_PTR
	};

	//get type
	template < class T > inline variant_type static_variant_type() { return VR_NONE; };
	//template specialization 
	template <> inline variant_type static_variant_type<char>() { return VR_CHAR; };
	template <> inline variant_type static_variant_type<short>() { return VR_SHORT; };
	template <> inline variant_type static_variant_type<int>() { return VR_INT; };
	template <> inline variant_type static_variant_type<long>() { return VR_LONG; };
	template <> inline variant_type static_variant_type<long long>() { return VR_LONGLONG; };

	template <> inline variant_type static_variant_type<unsigned char>() { return VR_UCHAR; };
	template <> inline variant_type static_variant_type<unsigned short>() { return VR_USHORT; };
	template <> inline variant_type static_variant_type<unsigned int>() { return VR_UINT; };
	template <> inline variant_type static_variant_type<unsigned long>() { return VR_ULONG; };
	template <> inline variant_type static_variant_type<unsigned long long>() { return VR_ULONGLONG; };

	template <> inline variant_type static_variant_type<float>() { return VR_FLOAT; };
	template <> inline variant_type static_variant_type<double>() { return VR_DOUBLE; };

	template <> inline variant_type static_variant_type<ivec2>() { return VR_IVEC2; };
	template <> inline variant_type static_variant_type<ivec3>() { return VR_IVEC3; };
	template <> inline variant_type static_variant_type<ivec4>() { return VR_IVEC4; };

	template <> inline variant_type static_variant_type<vec2>() { return VR_VEC2; };
	template <> inline variant_type static_variant_type<vec3>() { return VR_VEC3; };
	template <> inline variant_type static_variant_type<vec4>() { return VR_VEC4; };
	template <> inline variant_type static_variant_type<quat>() { return VR_QUAT; };
	template <> inline variant_type static_variant_type<mat3>() { return VR_MAT3; };
	template <> inline variant_type static_variant_type<mat4>() { return VR_MAT4; };

	template <> inline variant_type static_variant_type<dvec2>() { return VR_DVEC2; };
	template <> inline variant_type static_variant_type<dvec3>() { return VR_DVEC3; };
	template <> inline variant_type static_variant_type<dvec4>() { return VR_DVEC4; };
	template <> inline variant_type static_variant_type<dquat>() { return VR_DQUAT; };
	template <> inline variant_type static_variant_type<dmat3>() { return VR_DMAT3; };
	template <> inline variant_type static_variant_type<dmat4>() { return VR_DMAT4; };

	template <> inline variant_type static_variant_type< std::vector< int > >()   { return VR_STD_VECTOR_INT; };
	template <> inline variant_type static_variant_type< std::vector< float > >() { return VR_STD_VECTOR_FLOAT; };
	template <> inline variant_type static_variant_type< std::vector< vec2 > >()  { return VR_STD_VECTOR_VEC2; };
	template <> inline variant_type static_variant_type< std::vector< vec3 > >()  { return VR_STD_VECTOR_VEC3; };
	template <> inline variant_type static_variant_type< std::vector< vec4 > >()  { return VR_STD_VECTOR_VEC4; }

	template <> inline variant_type static_variant_type<const char*>() { return VR_C_STRING; };
	template <> inline variant_type static_variant_type<std::string>() { return VR_STD_STRING; };
	template <> inline variant_type static_variant_type< std::vector< std::string > >() { return VR_STD_VECTOR_STRING; };

	template <> inline variant_type static_variant_type<void*>() { return VR_PTR; };

	class HCUBE_API variant
	{
	public:
		
		variant(variant_type type)
		{
			set_type(type);
		}

		variant()
		{
			set_type(VR_NONE);
		}

		variant(const variant& in)
		{
			copy_from(in);
		}
		
		~variant()
		{
			set_type(VR_NONE);
		}
		
		variant(char c)
		{
			set_type(VR_CHAR);
			m_c = c;
		}

		variant(short s)
		{
			set_type(VR_SHORT);
			m_s = s;
		}

		variant(int i)
		{
			set_type(VR_INT);
			m_i = i;
		}

		variant(long l)
		{
			set_type(VR_LONG);
			m_l = l;
		}

		variant(long long l)
		{
			set_type(VR_LONGLONG);
			m_ll = l;
		}

		variant(unsigned char uc)
		{
			set_type(VR_UCHAR);
			m_uc = uc;
		}

		variant(unsigned short us)
		{
			set_type(VR_USHORT);
			m_us = us;
		}

		variant(unsigned int ui)
		{
			set_type(VR_UINT);
			m_ui = ui;
		}

		variant(unsigned long ul)
		{
			set_type(VR_ULONG);
			m_ul = ul;
		}

		variant(unsigned long long ull)
		{
			set_type(VR_ULONGLONG);
			m_ull = ull;
		}

		variant(float f)
		{
			set_type(VR_FLOAT);
			m_f = f;
		}

		variant(double d)
		{
			set_type(VR_DOUBLE);
			m_d    = d;
		}

		variant(const ivec2& iv2)
		{
			set_type(VR_IVEC2);
			m_iv2 = iv2;
		}

		variant(const ivec3& v3)
		{
			set_type(VR_IVEC3);
			m_v3 = v3;
		}

		variant(const ivec4& iv4)
		{
			set_type(VR_IVEC4);
			m_iv4 = iv4;
		}

		variant(const vec2& v2)
		{
			set_type(VR_VEC2);
			m_v2 = v2;
		}

		variant(const vec3& v3)
		{
			set_type(VR_VEC3);
			m_v3 = v3;
		}

		variant(const vec4& v4)
		{
			set_type(VR_VEC4);
			m_v4 = v4;
		}

		variant(const mat3& m3)
		{
			set_type(VR_MAT3);
			*((mat3*)(m_ptr)) = m3;
		}

		variant(const mat4& m4)
		{
			set_type(VR_MAT4);
			*((mat4*)(m_ptr)) = m4;
		}

		variant(const quat& quat)
		{
			set_type(VR_QUAT);
			m_dq = quat;
		}
		
		variant(const dvec2& dv2)
		{
			set_type(VR_DVEC2);
			m_dv2 = dv2;
		}

		variant(const dvec3& dv3)
		{
			set_type(VR_DVEC3);
			m_dv3 = dv3;
		}

		variant(const dvec4& dv4)
		{
			set_type(VR_DVEC4);
			m_dv4 = dv4;
		}

		variant(const dmat3& dm3)
		{
			set_type(VR_DMAT3);
			*((dmat3*)(m_ptr)) = dm3;
		}

		variant(const dmat4& dm4)
		{
			set_type(VR_DMAT4);
			*((dmat4*)(m_ptr)) = dm4;
		}

		variant(const dquat& dquat)
		{
			set_type(VR_DQUAT);
			m_dq = dquat;
		}

		variant(const std::vector< int > & v_i)
		{
			set_type(VR_STD_VECTOR_INT);
			*((std::vector< int >*)(m_ptr)) = v_i;
		}

		variant(const std::vector< float > & v_f)
		{
			set_type(VR_STD_VECTOR_FLOAT);
			*((std::vector< float >*)(m_ptr)) = v_f;
		}

		variant(const std::vector< vec2 > & v_v2)
		{
			set_type(VR_STD_VECTOR_VEC2);
			*((std::vector< vec2 >*)(m_ptr)) = v_v2;
		}

		variant(const std::vector< vec3 > & v_v3)
		{
			set_type(VR_STD_VECTOR_VEC3);
			*((std::vector< vec3 >*)(m_ptr)) = v_v3;
		}

		variant(const std::vector< vec4 > & v_v4)
		{
			set_type(VR_STD_VECTOR_VEC4);
			*((std::vector< vec4 >*)(m_ptr)) = v_v4;
		}

		variant(const char* c_str)
		{
			set_type(VR_C_STRING);
			*((std::string*)(m_ptr)) = c_str;
		}

		variant(const std::string& str)
		{
			set_type(VR_STD_STRING);
			*((std::string*)(m_ptr)) = str;
		}

		variant(const std::vector< std::string > & v_str)
		{
			set_type(VR_STD_VECTOR_STRING);
			*((std::vector< std::string >*)(m_ptr)) = v_str;
		}

		variant(void* ptr)
		{
			set_type(VR_PTR);
			m_ptr = ptr;
		}

		variant(const variant_ref& ref);

		//cast objects
		template < class T >
		T& get()
		{
			if (is_heap_value()) return *((T*)m_ptr);
			else				return *((T*)&m_ptr);
		}
		template < class T >
		const T& get() const
		{
			if (is_heap_value()) return *((T*)m_ptr);
			else				return *((T*)&m_ptr);
		}
		template < class T >
		explicit  operator const T& () const 
		{
			if (is_heap_value()) return *((T*)m_ptr);
			else				 return *((T*)&m_ptr);
		}

		//alloc_cast
		template < class T >
		T& get_alloc() 
		{ 
			if (static_variant_type<T>() != m_type)
			{
				if (!is_heap_value())
				{ 
					std::memset(this, 0, sizeof(variant));
					m_type = VR_NONE;
				}
				set_type(static_variant_type<T>());
			}
			return *((T*)&m_ptr); 
		}


		//type
		variant_type get_type() const
		{
			return m_type;
		}

		//bool
		bool is_heap_value() const
		{
			switch (m_type)
			{
				case VR_MAT3:			  
				case VR_MAT4:			   
				case VR_DMAT3:			   
				case VR_DMAT4:			   
				case VR_C_STRING:          
				case VR_STD_STRING:
				case VR_STD_VECTOR_INT:
				case VR_STD_VECTOR_FLOAT:
				case VR_STD_VECTOR_VEC2:
				case VR_STD_VECTOR_VEC3:
				case VR_STD_VECTOR_VEC4:
				case VR_STD_VECTOR_STRING: return true; break;
				default: return false; break;
			}
		}

		//get reference pointer
		void* get_ref() const
		{
			if(is_heap_value()) return m_ptr;
			else				return (void*)&m_c;
		}

		//copy from other variant
		void copy_from(const variant& in)
		{
			//alloc
			set_type(in.get_type());
			//copy
			switch (m_type)
			{
			//copy from heap
			case VR_MAT3:			   get<mat3>() = (const mat3&)in; break;
			case VR_MAT4:			   get<mat4>() = (const mat4&)in; break;
			case VR_DMAT3:			   get<dmat3>() = (const dmat3&)in; break;
			case VR_DMAT4:			   get<dmat4>() = (const dmat4&)in; break;

			case VR_STD_VECTOR_INT:
				get< std::vector<int> >() = (const std::vector<int>&)in;
			break;
			case VR_STD_VECTOR_FLOAT:
				get< std::vector<float> >() = (const std::vector<float>&)in;
			break;
			case VR_STD_VECTOR_VEC2:
				get< std::vector<vec2> >() = (const std::vector<vec2>&)in;
			break;
			case VR_STD_VECTOR_VEC3:
				get< std::vector<vec3> >() = (const std::vector<vec3>&)in;
			break;
			case VR_STD_VECTOR_VEC4:
				get< std::vector<vec4> >() = (const std::vector<vec4>&)in;
			break;

			case VR_C_STRING:
			case VR_STD_STRING:
				get<std::string>() = (const std::string&)in;
			break;
			case VR_STD_VECTOR_STRING:
				get< std::vector<std::string> >() = (const std::vector<std::string>&)in;
			break;
			//copy stack
			default:
				std::memcpy(this, &in, sizeof(variant));
			break;
			}
		}

	private: 

		//native type
		union alignas(16)
		{
			char       m_c;
			short      m_s;
			int        m_i;
			long       m_l;
			long long  m_ll;

			unsigned char       m_uc;
			unsigned short      m_us;
			unsigned int        m_ui;
			unsigned long       m_ul;
			unsigned long long  m_ull;

			float  m_f;
			double m_d;

			ivec2 m_iv2;
			ivec3 m_iv3;
			ivec4 m_iv4;

			vec2 m_v2;
			vec3 m_v3;
			vec4 m_v4;
			quat m_q;

			dvec2 m_dv2;
			dvec3 m_dv3;
			dvec4 m_dv4;
			dquat m_dq;

			void* m_ptr;
		};
		//save type
		variant_type m_type{ VR_NONE };
		//set type
		void set_type(variant_type type)
		{
			//alloc
			switch (m_type)
			{
			case VR_MAT3:			   delete (mat3*)(m_ptr);  break;
			case VR_MAT4:			   delete (mat4*)(m_ptr);  break;
			case VR_DMAT3:			   delete (dmat4*)(m_ptr); break;
			case VR_DMAT4:			   delete (dmat4*)(m_ptr); break;

			case VR_STD_VECTOR_INT:    delete (std::vector<int>*)m_ptr;    break;
			case VR_STD_VECTOR_FLOAT:  delete (std::vector<float>*)m_ptr;  break;
			case VR_STD_VECTOR_VEC2:   delete (std::vector<vec2>*)m_ptr;   break;
			case VR_STD_VECTOR_VEC3:   delete (std::vector<vec3>*)m_ptr;   break;
			case VR_STD_VECTOR_VEC4:   delete (std::vector<vec4>*)m_ptr;   break;

			case VR_C_STRING:          delete (std::string*)m_ptr;			     break;
			case VR_STD_STRING:        delete (std::string*)m_ptr;			     break;
			case VR_STD_VECTOR_STRING: delete (std::vector<std::string>*)m_ptr;  break;
			default: break;
			}
			//change type
			m_type = type;
			//dealloc
			switch (m_type)
			{
			case VR_MAT3:			   m_ptr = new mat3; break;
			case VR_MAT4:			   m_ptr = new mat4; break;
			case VR_DMAT3:			   m_ptr = new dmat3; break;
			case VR_DMAT4:			   m_ptr = new dmat4; break;

			case VR_STD_VECTOR_INT:    m_ptr = new std::vector<int>;	break;
			case VR_STD_VECTOR_FLOAT:  m_ptr = new std::vector<float>;	break;
			case VR_STD_VECTOR_VEC2:   m_ptr = new std::vector<vec2>;   break;
			case VR_STD_VECTOR_VEC3:   m_ptr = new std::vector<vec3>;	break;
			case VR_STD_VECTOR_VEC4:   m_ptr = new std::vector<vec4>;	break;

			case VR_C_STRING:          m_ptr = new std::string;				 break;
			case VR_STD_STRING:        m_ptr = new std::string;				 break;
			case VR_STD_VECTOR_STRING: m_ptr = new std::vector<std::string>; break;
			default: break;
			}
		}
	};

	class HCUBE_API variant_ref
	{
	public:

		variant_ref()
		{
			m_type = VR_NONE;
		}

		variant_ref(variant& var)
		{
			m_ptr = (void*)var.get_ref();
			m_type = var.get_type();
		}

		variant_ref(const variant& var)
		{
			m_ptr = (void*)var.get_ref();
			m_type = var.get_type();
		}

		variant_ref(char& c)
		{
			m_ptr  = (void*)&c;
			m_type = VR_CHAR;
		}

		variant_ref(short& s)
		{
			m_ptr  = (void*)&s;
			m_type = VR_SHORT;
		}

		variant_ref(int& i)
		{
			m_ptr = (void*)&i;
			m_type = VR_INT;
		}

		variant_ref(long& l)
		{
			m_ptr = (void*)&l;
			m_type = VR_LONG;
		}

		variant_ref(long long& l)
		{
			m_ptr = (void*)&l;
			m_type = VR_LONGLONG;
		}

		variant_ref(unsigned char& uc)
		{
			m_ptr = (void*)&uc;
			m_type = VR_UCHAR;
		}

		variant_ref(unsigned short& us)
		{
			m_ptr = (void*)&us;
			m_type = VR_USHORT;
		}

		variant_ref(unsigned int& ui)
		{
			m_ptr = (void*)&ui;
			m_type = VR_UINT;
		}

		variant_ref(unsigned long& ul)
		{
			m_ptr = (void*)&ul;
			m_type = VR_ULONG;
		}

		variant_ref(unsigned long long& ull)
		{
			m_ptr = (void*)&ull;
			m_type = VR_ULONGLONG;
		}

		variant_ref(float& f)
		{
			m_ptr = (void*)&f;
			m_type = VR_FLOAT;
		}

		variant_ref(double& d)
		{
			m_ptr = (void*)&d;
			m_type = VR_DOUBLE;
		}

		variant_ref(ivec2& iv2)
		{
			m_ptr = (void*)&iv2;
			m_type = VR_IVEC2;
		}

		variant_ref(ivec3& v3)
		{
			m_ptr = (void*)&v3;
			m_type = VR_IVEC3;
		}

		variant_ref(ivec4& iv4)
		{
			m_ptr = (void*)&iv4;
			m_type = VR_IVEC4;
		}

		variant_ref(vec2& v2)
		{
			m_ptr = (void*)&v2;
			m_type = VR_VEC2;
		}

		variant_ref(vec3& v3)
		{
			m_ptr = (void*)&v3;
			m_type = VR_VEC3;
		}

		variant_ref(vec4& v4)
		{
			m_ptr = (void*)&v4;
			m_type = VR_VEC4;
		}

		variant_ref(mat3& m3)
		{
			m_ptr = (void*)&m3;
			m_type = VR_MAT3;
		}

		variant_ref(mat4& m4)
		{
			m_ptr = (void*)&m4;
			m_type = VR_MAT4;
		}

		variant_ref(quat& quat)
		{
			m_ptr = (void*)&quat;
			m_type = VR_QUAT;
		}

		variant_ref(dvec2& dv2)
		{
			m_ptr = (void*)&dv2;
			m_type = VR_DVEC2;
		}

		variant_ref(dvec3& dv3)
		{
			m_ptr = (void*)&dv3;
			m_type = VR_DVEC3;
		}

		variant_ref(dvec4& dv4)
		{
			m_ptr = (void*)&dv4;
			m_type = VR_DVEC4;
		}

		variant_ref(dmat3& dm3)
		{
			m_ptr = (void*)&dm3;
			m_type = VR_DMAT3;
		}

		variant_ref(dmat4& dm4)
		{
			m_ptr = (void*)&dm4;
			m_type = VR_DMAT4;
		}

		variant_ref(dquat& dquat)
		{
			m_ptr = (void*)&dquat;
			m_type = VR_DQUAT;
		}

		variant_ref(std::vector< int > & v_i)
		{
			m_ptr = (void*)&v_i;
			m_type = VR_STD_VECTOR_INT;
		}

		variant_ref(std::vector< float > & v_f)
		{
			m_ptr = (void*)&v_f;
			m_type = VR_STD_VECTOR_FLOAT;
		}

		variant_ref(std::vector< vec2 > & v_v2)
		{
			m_ptr = (void*)&v_v2;
			m_type = VR_STD_VECTOR_VEC2;
		}

		variant_ref(std::vector< vec3 > & v_v3)
		{
			m_ptr = (void*)&v_v3;
			m_type = VR_STD_VECTOR_VEC3;
		}

		variant_ref(std::vector< vec4 > & v_v4)
		{
			m_ptr = (void*)&v_v4;
			m_type = VR_STD_VECTOR_VEC4;
		}
		
		variant_ref(const char*& c_str)
		{
			m_ptr = (void*)&c_str;
			m_type = VR_C_STRING;
		}

		variant_ref(std::string& str)
		{
			m_ptr = (void*)&str;
			m_type = VR_STD_STRING;
		}

		variant_ref(std::vector< std::string >& v_str)
		{
			m_ptr = (void*)&v_str;
			m_type = VR_STD_VECTOR_STRING;
		}

		variant_ref(void*& ptr)
		{
			m_ptr = (void*)&ptr;
			m_type = VR_PTR;
		}

		template < class T >
		T& get()
		{
			return *((T*)m_ptr);
		}

		template < class T >
		const T& get() const
		{
			return *((T*)m_ptr);
		}


		template < class T >
		explicit  operator const T& () const
		{
			return *((T*)m_ptr);
		}

		variant_type get_type() const
		{
			return m_type;
		}

		void* get_ref() const
		{
			return m_ptr;
		}

	private:

		void*		 m_ptr;
		variant_type m_type;

	};
	
	inline variant::variant(const variant_ref& ref)
	{
		switch (ref.get_type())
		{
		case hcube::VR_NONE:  break;
		case hcube::VR_CHAR:     (*this) = (const char)ref; break;
		case hcube::VR_SHORT:    (*this) = (const short)ref; break;
		case hcube::VR_INT:      (*this) = (const int)ref; break;
		case hcube::VR_LONG:     (*this) = (const long)ref; break;
		case hcube::VR_LONGLONG: (*this) = (const long long)ref; break;
		case hcube::VR_UCHAR:    (*this) = (const unsigned char)ref; break;
		case hcube::VR_USHORT:   (*this) = (const unsigned short)ref; break;
		case hcube::VR_UINT:     (*this) = (const unsigned int)ref; break;
		case hcube::VR_ULONG:    (*this) = (const unsigned long)ref; break;
		case hcube::VR_ULONGLONG:(*this) = (const unsigned long long)ref; break;
		case hcube::VR_FLOAT:    (*this) = (const float)ref; break;
		case hcube::VR_DOUBLE:   (*this) = (const double)ref; break;
		case hcube::VR_VEC2:     (*this) = (const vec2)ref; break;
		case hcube::VR_VEC3:     (*this) = (const vec3)ref; break;
		case hcube::VR_VEC4:     (*this) = (const vec4)ref; break;
		case hcube::VR_QUAT:     (*this) = (const quat)ref; break;
		case hcube::VR_IVEC2:    (*this) = (const ivec2)ref; break;
		case hcube::VR_IVEC3:    (*this) = (const ivec3)ref; break;
		case hcube::VR_IVEC4:    (*this) = (const ivec4)ref; break;
		case hcube::VR_DVEC2:    (*this) = (const dvec2)ref; break;
		case hcube::VR_DVEC3:    (*this) = (const dvec3)ref; break;
		case hcube::VR_DVEC4:    (*this) = (const dvec4)ref; break;
		case hcube::VR_DQUAT:    (*this) = (const dquat)ref; break;
		case hcube::VR_MAT3:     (*this) = (const mat3)ref; break;
		case hcube::VR_MAT4:     (*this) = (const mat4)ref; break;
		case hcube::VR_DMAT3:    (*this) = (const dmat3)ref; break;
		case hcube::VR_DMAT4:    (*this) = (const dmat4)ref; break;
		case hcube::VR_STD_VECTOR_INT:     (*this) = (const std::vector<int>)ref; break;
		case hcube::VR_STD_VECTOR_FLOAT:   (*this) = (const std::vector<float>)ref; break;
		case hcube::VR_STD_VECTOR_VEC2:    (*this) = (const std::vector<vec2>)ref; break;
		case hcube::VR_STD_VECTOR_VEC3:    (*this) = (const std::vector<vec3>)ref; break;
		case hcube::VR_STD_VECTOR_VEC4:    (*this) = (const std::vector<vec4>)ref; break;
		case hcube::VR_C_STRING:
		case hcube::VR_STD_STRING:         (*this) = (const std::string)ref; break;
		case hcube::VR_STD_VECTOR_STRING:  (*this) = (const std::vector<std::string>)ref; break;
		case hcube::VR_PTR:				   (*this) = (void*)(const void*)ref; break;
		default: break;
		}
	}
}