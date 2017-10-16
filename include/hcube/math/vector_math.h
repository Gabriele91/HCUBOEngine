//
//  vector_math.h
//  HCubo
//
//  Created by Gabriele on 25/08/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <cmath>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/matrix_access.hpp> 

#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>

namespace hcube
{
    using  vec2 = glm::vec2;
    using dvec2 = glm::dvec2;
    using ivec2 = glm::ivec2;
    
    using  vec3 = glm::vec3;
    using dvec3 = glm::dvec3;
    using ivec3 = glm::ivec3;
    
    using  vec4 = glm::vec4;
    using dvec4 = glm::dvec4;
    using ivec4 = glm::ivec4;
    
    using  mat3 = glm::mat3;
    using dmat3 = glm::dmat3;
    
    using  mat4 = glm::mat4;
    using dmat4 = glm::dmat4;
    
    using quat  = glm::quat;
    using dquat = glm::dquat;
    
    namespace constants
    {
        template <class T>
        inline T pi()
        {
            return glm::pi<T>();
        }
		template <class T>
		inline T pi2()
		{
			return glm::pi<T>()*((T)2.0);
		}
    }
    
    template < class T >
    inline T dot(const T& a, const T& b)
    {
        return glm::dot(a, b);
    }
    
    template < class T >
    inline T cross(const T& a, const T& b)
    {
        return glm::cross(a, b);
    }
    
    template < class T >
    inline T normalize(const T& a)
    {
        return glm::normalize(a);
    }
    
    template < class T >
    inline T length(const T& a)
    {
        return glm::length(a);
    }
    
    template < class T >
    inline T ceil(const T& a)
    {
        return glm::ceil(a);
    }
    
    template < class T >
    inline T floor(const T& a)
    {
        return glm::floor(a);
    }
    
    template < class T >
    inline T abs(const T& a)
    {
        return glm::abs(a);
    }
    
    template < class T >
    inline T clamp(const T& a,const T& min, const T&  max)
    {
        return glm::clamp(a,min,max);
    }
    
    template < class T, class F >
    inline T mix(const T& a,const T& b, const F&  f)
    {
        return glm::mix(a,b,f);
    }

	template < class T, class F >
	inline T lerp(const T& a, const T& b, const F&  f)
	{
		return glm::mix(a, b, f);
	}
    
    template < class T >
    inline T traspose(const T& a)
    {
        return glm::transpose(a);
    }
    
    template < class T >
    inline T inverse(const T& a)
    {
        return glm::inverse(a);
    }
    
    template < class T >
    inline T conjugate(const T& a)
    {
        return glm::conjugate(a);
    }
    
    template < class T >
    inline T diagonal3x3(const T& a)
    {
        return glm::diagonal3x3(a);
    }
    
    template < class T >
    inline T diagonal4x4(const T& a)
    {
        return glm::diagonal4x4(a);
    }
    
    template < class T >
    inline T radians(const T& a)
    {
        return glm::radians(a);
    }
    
    template < class T >
    inline T degrees(const T& a)
    {
        return glm::degrees(a);
    }

	template < class T >
	inline auto row(const T& a, size_t i) -> decltype( glm::row(a, i) )
	{
		return glm::row(a, i);
	}

	template < class T >
	inline auto column(const T& a, size_t i) -> decltype( glm::column(a, i) )
	{
		return glm::column(a, i);
	}
    
    template < class T, class V >
    inline T translate(const T& mat,const V& v)
    {
        return glm::translate(mat,v);
    }
    
    template < class T, class V >
    inline T scale(const T& mat,const V& v)
    {
        return glm::scale(mat,v);
    }
    
    template < class T, class A, class V >
    inline T rotate(const T& q,const A& angle,const V& v)
    {
        return glm::rotate(q, angle, v);
    }
    
    template < class T >
    inline mat4 perspective(const T& fovy,const T& aspect,const T& near,const T& far)
    {
        return glm::perspective<T>(fovy, aspect, near, far);
    }
    
    template < class T >
    inline mat4 look_at(const T& eye,const T& center, const T& up)
    {
        return glm::lookAt(eye, center, up);
    }
    
    
    
    template < class T >
    inline auto value_ptr(const T& q_m_v) -> decltype( glm::value_ptr(q_m_v) )
    {
        return glm::value_ptr(q_m_v);
    }
    
    template < class T >
    inline auto quat_cast(const T& q_m) -> decltype( glm::quat_cast(q_m) )
    {
        return glm::quat_cast(q_m);
    }

	template < class T >
	inline auto mat3_cast(const T& q_v) -> decltype(glm::mat3_cast(q_v))
	{
		return glm::mat3_cast(q_v);
	}

    template < class T >
    inline auto mat4_cast(const T& q_v) -> decltype(  glm::mat4_cast(q_v) )
    {
        return glm::mat4_cast(q_v);
    }
    
	template < class T >
	inline auto to_quat(const T& q_m) -> decltype(glm::toQuat(q_m))
	{
		return glm::toQuat(q_m);
	}

	template < class T >
	inline auto to_mat3(const T& q_v) -> decltype(glm::toMat3(q_v))
	{
		return glm::toMat3(q_v);
	}

	template < class T >
	inline auto to_mat4(const T& q_v) -> decltype(glm::toMat4(q_v))
	{
		return glm::toMat4(q_v);
	}
    /*
    * eigenvalues rotate
    */
    GLM_FUNC_QUALIFIER void eigenvalues_rotate(mat3& mat, double& c, double& s, int i0, int j0, int i1, int j1)
    {
        double a = c * mat[i0][j0] - s * mat[i1][j1];
        double b = s * mat[i0][j0] + c * mat[i1][j1];
        mat[i0][j0] = (float)a;
        mat[i1][j1] = (float)b;
    }
    
    /**
     * Diagonize matrix using Jacobi rotations.
     * @remark Method does not check if matrix is diagonizable.
     * Passing non diagonizable matrix and infinite max_iter (= -1)
     * May result in infinite loop.
     */
    GLM_FUNC_QUALIFIER vec3 eigenvalues_jacobi(mat3& mat, int max_iter, mat3& E)
    {
        vec3 ret;
        
        // initial eigenvalues
        ret[0] = mat[0][0];
        ret[1] = mat[1][1];
        ret[2] = mat[2][2];
        
        mat[0][0] = 1.0;
        mat[1][1] = 1.0;
        mat[2][2] = 1.0;
        
        E = mat3(1.0);
        
        for(int z = 0; z!=max_iter; ++z)
        {
            for (int i = 0; i != 3; ++i)
            {
                for (int j = i + 1; j < 3; j++)
                {
                    double mii = ret[i];
                    double mjj = ret[j];
                    double mij = mat[i][j];
                    double phi = 0.5 * std::atan2(2 * mij, mjj - mii);
                    double c = std::cos(phi);
                    double s = std::sin(phi);
                    double mii1 = c * c * mii - 2 * s * c * mij + s * s * mjj;
                    double mjj1 = s * s * mii + 2 * s * c * mij + c * c * mjj;
                    if (std::abs(mii - mii1) < 0.00001 || (mjj - mjj1) < 0.00001) {
                        ret[i]    = (float)mii1;
                        ret[j]    = (float)mjj1;
                        mat[i][j] = (float)0.0;
                        for (int k = 0;     k < i; ++k) eigenvalues_rotate(mat, c, s, k, i, k, j);
                        for (int k = i + 1; k < j; ++k) eigenvalues_rotate(mat, c, s, i, k, k, j);
                        for (int k = j + 1; k < 3; ++k) eigenvalues_rotate(mat, c, s, i, k, j, k);
                        //rotate eigenvectors
                        for (int k = 0; k < 3; k++) eigenvalues_rotate(E, c, s, k, i, k, j);
                    }
                }
            }
        }
        return ret;
    }
}