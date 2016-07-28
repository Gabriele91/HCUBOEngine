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
    float     m_const	 { 1.0 };
	float     m_linear   { 0.0 };
	float     m_quadratic{ 0.0 };
	//default
	light() {}
	//init params
	light
	(
		glm::vec4 diffuse,
		float     constant,
		float     linear,
		float     quadratic
	) 
	{
		m_diffuse   = diffuse;
		m_const     = constant;
		m_linear    = linear;
		m_quadratic = quadratic;
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
		m_const,
		m_linear,
		m_quadratic
	});
}