//
//  light.h
//  HCubo
//
//  Created by Gabriele on 21/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <component.h>
#include <smart_pointers.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


class light : public component
{
    
    COMPONENT_DEC(light)
    
public:
    //light type
    enum light_type
    {
        DIRECTION_LIGHT = 0,
        POINT_LIGHT     = 1,
        SPOT_LIGHT      = 2
    };
    //type
    light_type m_type { POINT_LIGHT };
    //light info
    glm::vec3  m_diffuse { 1.0, 1.0, 1.0 };
    glm::vec3  m_specular{ 1.0, 1.0, 1.0 };
    //attenuation
    float      m_constant	  { 1.0 };
    float      m_linear       { 1.0 };
    float      m_quadratic    { 0.0 };
    //spot light info
    float      m_inner_cut_off{ -1.0 };
    float      m_outer_cut_off{ -1.0 };
	//default
	light() {}
	//init params
	light
	(
        light_type       type,
        const glm::vec3& diffuse,
        const glm::vec3& specular,
		float            constant,
		float            linear,
		float            quadratic,
        float            inner_cut_off,
        float            outer_cut_off
	) 
	{
        m_type        = type;
        m_diffuse     = diffuse;
        m_specular    = specular;
        m_constant    = constant;
        m_linear      = linear;
        m_quadratic   = quadratic;
        m_inner_cut_off = inner_cut_off;
        m_outer_cut_off = outer_cut_off;
	}
    //init point type
    void point(const glm::vec3& diffuse,
               const glm::vec3& specular,
               float            constant,
               float            linear,
               float            quadratic)
    {
        m_type        = POINT_LIGHT;
        m_diffuse     = diffuse;
        m_specular    = specular;
        m_constant    = constant;
        m_linear      = linear;
        m_quadratic   = quadratic;
    }
    //init sport type
    void spot(const glm::vec3& diffuse,
              const glm::vec3& specular,
              float            constant,
              float            linear,
              float            quadratic,
              float            inner_cut_off,
              float            outer_cut_off)
    {
        m_type        = SPOT_LIGHT;
        m_diffuse     = diffuse;
        m_specular    = specular;
        m_constant    = constant;
        m_linear      = linear;
        m_quadratic   = quadratic;
        m_inner_cut_off = std::cos(inner_cut_off);
        m_outer_cut_off = std::cos(outer_cut_off);
    }
    //init direction type
    void direction(const glm::vec3& diffuse,
                   const glm::vec3& specular)
    {
        m_type      = DIRECTION_LIGHT;
        m_diffuse   = diffuse;
        m_specular  = specular;
    }
    
    //compute attenuation from radius and linear att
    void set_quadratic_attenuation_from_radius(float radius,float linear = 1.0, float constant = 1.0)
    {
        //formula
        // a = 1./ (c + d*l + d*d*q)
        // if d = r => a=0
        // q = (c+r*l)/(r*r)
        m_quadratic = (constant+radius*linear)/(radius*radius);
        m_linear    = linear;
        m_constant  = constant;
    }
    
    void set_linear_attenuation_from_radius(float radius, float quadratic = 1.0, float constant = 1.0)
    {
        //formula
        // a = 1./ (c + d*l + d*d*q)
        // if d = r => a=0
        // l = (c+r*r*q)/r
        m_linear    = ((constant+radius*radius*quadratic)/radius);
        m_quadratic = quadratic;
        m_constant  = constant;
    }
    
    float get_radius_factor(float brightness=128.0f) const
    {
        float light_max  = std::fmaxf(std::fmaxf(m_diffuse.r, m_diffuse.g), m_diffuse.b);
        float brightness_factor =  256.f / brightness; //256.0f->green max bit value
        return (m_linear +  std::sqrtf(m_linear * m_linear - 4 * m_quadratic * (m_constant - brightness_factor * light_max))) / (2 * m_quadratic);
    }
    
    
	//copy
	virtual component_ptr copy() const;
};

using light_ptr  = std::shared_ptr< light >;
using light_uptr = std::unique_ptr< light >;
using light_wptr = std::weak_ptr< light >;

template < class... Args >
static inline light_ptr light_snew(Args&&... args)
{
    return std::make_shared< light >( args... );
}

inline component_ptr light::copy() const
{
    return light_snew(light{
        m_type,
        m_diffuse,
        m_specular,
        m_constant,
        m_linear,
        m_quadratic ,
        m_inner_cut_off,
        m_outer_cut_off
	});
}