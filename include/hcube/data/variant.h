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
		VR_MAT3,
		VR_MAT4,
		VR_QUAT,

		VR_IVEC2,
		VR_IVEC3,
		VR_IVEC4,

		VR_DVEC2,
		VR_DVEC3,
		VR_DVEC4,
		VR_DMAT3,
		VR_DMAT4,
		VR_DQUAT,

		VR_PTR
	};
	class HCUBE_API variant
	{
	public:

		variant()
		{
			m_type = VR_NONE;
		}

		variant(char c)
		{
			m_c = c;
			m_type = VR_CHAR;
		}

		variant(short s)
		{
			m_s = s;
			m_type = VR_SHORT;
		}

		variant(int i)
		{
			m_i = i;
			m_type = VR_INT;
		}

		variant(long l)
		{
			m_l = l;
			m_type = VR_LONG;
		}

		variant(long long l)
		{
			m_ll = l;
			m_type = VR_LONGLONG;
		}

		variant(unsigned char uc)
		{
			m_uc = uc;
			m_type = VR_UCHAR;
		}

		variant(unsigned short us)
		{
			m_us = us;
			m_type = VR_USHORT;
		}

		variant(unsigned int ui)
		{
			m_ui = ui;
			m_type = VR_UINT;
		}

		variant(unsigned long ul)
		{
			m_ul = ul;
			m_type = VR_ULONG;
		}

		variant(unsigned long long ull)
		{
			m_ull  = ull;
			m_type = VR_ULONGLONG;
		}

		variant(float f)
		{
			m_f = f;
			m_type = VR_FLOAT;
		}

		variant(double d)
		{
			m_d    = d;
			m_type = VR_DOUBLE;
		}

		variant(const ivec2& iv2)
		{
			m_iv2 = iv2;
			m_type = VR_IVEC2;
		}

		variant(const ivec3& v3)
		{
			m_v3 = v3;
			m_type = VR_IVEC3;
		}

		variant(const ivec4& iv4)
		{
			m_iv4 = iv4;
			m_type = VR_IVEC4;
		}

		variant(const vec2& v2)
		{
			m_v2 = v2;
			m_type = VR_VEC2;
		}

		variant(const vec3& v3)
		{
			m_v3 = v3;
			m_type = VR_VEC3;
		}

		variant(const vec4& v4)
		{
			m_v4 = v4;
			m_type = VR_VEC4;
		}

		variant(const mat3& m3)
		{
			m_m3 = m3;
			m_type = VR_MAT3;
		}

		variant(const mat4& m4)
		{
			m_m4 = m4;
			m_type = VR_MAT4;
		}

		variant(const quat& quat)
		{
			m_dq = quat;
			m_type = VR_QUAT;
		}
		
		variant(const dvec2& dv2)
		{
			m_dv2 = dv2;
			m_type = VR_DVEC2;
		}

		variant(const dvec3& dv3)
		{
			m_dv3 = dv3;
			m_type = VR_DVEC3;
		}

		variant(const dvec4& dv4)
		{
			m_dv4 = dv4;
			m_type = VR_DVEC4;
		}

		variant(const dmat3& dm3)
		{
			m_dm3 = dm3;
			m_type = VR_DMAT3;
		}

		variant(const dmat4& dm4)
		{
			m_dm4 = dm4;
			m_type = VR_DMAT4;
		}

		variant(const dquat& dquat)
		{
			m_dq = dquat;
			m_type = VR_DQUAT;
		}

		variant(void* ptr)
		{
			m_ptr = ptr;
			m_type = VR_PTR;
		}

		template < class T >
		T& get()
		{
			return *((T*)&m_ptr);
		}

		template < class T >
		explicit  operator const T& () const
		{
			return *((T*)&m_ptr);
		}

		variant_type get_type() const
		{
			return m_type;
		}

	private: 

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
			mat3 m_m3;
			mat4 m_m4;
			quat m_q;

			dvec2 m_dv2;
			dvec3 m_dv3;
			dvec4 m_dv4;
			dmat3 m_dm3;
			dmat4 m_dm4;
			dquat m_dq;

			void* m_ptr;
		};

		variant_type m_type;
	};
}