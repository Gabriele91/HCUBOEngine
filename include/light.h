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
    glm::vec4  m_diffuse;
    float      m_intensity	  { 1.0 };
    float      m_min_radius   { 0.0 };
    float      m_max_radius   { 0.0 };
    //spot light info
    float      m_spot_cutoff  { 0.0 };
    float      m_spot_exponent{ 1.0 };
    
	//default
	light() {}
	//init params
	light
	(
        light_type       type,
		const glm::vec4& diffuse,
		float            intensity,
		float            min_radius,
		float            max_radius,
        float            spot_cutoff   = 0.0,
        float            spot_exponent = 0.0
	) 
	{
        m_type          = type;
		m_diffuse       = diffuse;
        m_intensity     = intensity;
        m_min_radius    = min_radius;
        m_max_radius    = max_radius;
        m_spot_cutoff   = spot_cutoff;
        m_spot_exponent = spot_exponent;
	}
    //init point type
    void point(const glm::vec4& diffuse,
               float            intensity,
               float            min_radius,
               float            max_radius)
    {
        m_type          = POINT_LIGHT;
        m_diffuse       = diffuse;
        m_intensity     = intensity;
        m_min_radius    = min_radius;
        m_max_radius    = max_radius;
    }
    //init sport type
    void spot(const glm::vec4& diffuse,
              float            intensity,
              float            min_radius,
              float            max_radius,
              float            angle,
              float            exponent = 1.0)
    {
        m_type          = SPOT_LIGHT;
        m_diffuse       = diffuse;
        m_intensity     = intensity;
        m_min_radius    = min_radius;
        m_max_radius    = max_radius;
        m_spot_cutoff   = std::cos(angle*0.5f);
        m_spot_exponent = exponent;
    }
    //init direction type
    void direction(const glm::vec4& diffuse,
                   float            intensity)
    {
        m_type          = DIRECTION_LIGHT;
        m_diffuse       = diffuse;
        m_intensity     = intensity;
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
		m_intensity,
		m_min_radius,
		m_max_radius,
        m_spot_cutoff,
        m_spot_exponent
	});
}