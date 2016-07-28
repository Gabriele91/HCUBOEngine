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
    
    glm::vec4 m_diffuse;
    float     m_intensity	 { 1.0 };
    float     m_min_radius   { 0.0 };
	float     m_max_radius   { 0.0 };
	//default
	light() {}
	//init params
	light
	(
		glm::vec4 diffuse,
		float     intensity,
		float     min_radius,
		float     max_radius
	) 
	{
		m_diffuse   = diffuse;
		m_intensity  = intensity;
		m_min_radius = min_radius;
		m_max_radius = max_radius;
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
		m_diffuse,
		m_intensity,
		m_min_radius,
		m_max_radius
	});
}