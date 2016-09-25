//
//  align16_data.h
//  HCubo
//
//  Created by Gabriele on 23/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <hcube/math/vector_math.h>

namespace hcube
{
    #define HCUBE_STRUCT_ALIGN16(name)\
        struct alignas(16) align16_ ## name
    
    
    template < typename T >
    struct alignas(16) align16
    {
        //value
        T m_data = (T)0;
        //default construction
        align16()
        {
            //void
        }
        align16(const T& in_data)
        {
            m_data = in_data;
        }
        //cast
        operator T& ()
        {
            return m_data;
        }
        //const cast
        operator const T& () const
        {
            return m_data;
        }
    };
    
    //signed
    using align16_bool     = align16<bool>;
    using align16_char     = align16<bool>;
    using align16_short    = align16<short>;
    using align16_int      = align16<int>;
    using align16_long     = align16<long>;
    using align16_longlong = align16<long>;
    using align16_float    = align16<float>;
    using align16_double   = align16<double>;
    
    //unsigned
    using align16_uchar    = align16<unsigned char>;
    using align16_ushort   = align16<unsigned short>;
    using align16_uint     = align16<unsigned int>;
    using align16_ulong    = align16<unsigned long>;
    using align16_ulonlong = align16<unsigned long long>;
    
    //vector
    using align16_vec2     = align16<vec2>;
    using align16_vec3     = align16<vec3>;
    using align16_vec4     = align16<vec4>;
    using align16_mat4     = align16<mat4>;
    
}