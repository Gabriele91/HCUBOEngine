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
#include <shader.h>


namespace hcube
{
	class light : public component
	{

		COMPONENT_DEC(light)

	public:
		//light type
		enum light_type
		{
			DIRECTION_LIGHT = 0,
			POINT_LIGHT = 1,
			SPOT_LIGHT = 2
		};
		//type
		light_type m_type{ POINT_LIGHT };
		//light info
		glm::vec3  m_diffuse { 1.0, 1.0, 1.0 };
		glm::vec3  m_specular{ 1.0, 1.0, 1.0 };
		//attenuation
		float      m_constant     { 1.0 };
		float      m_inside_radius{ 1.0 };
		float      m_radius       { 1.0 };
		//spot light info
		float      m_inner_cut_off{ -1.0 };
		float      m_outer_cut_off{ -1.0 };
		//default
		light() {}

		//init all params
		light
		(
			light_type       type,
			const glm::vec3& diffuse,
			const glm::vec3& specular,
			float            constant,
			float            inside_radius,
			float            radius,
			float            inner_cut_off,
			float            outer_cut_off
		)
		{
			m_type = type;
			m_diffuse = diffuse;
			m_specular = specular;
			m_constant = constant;
			m_inside_radius = inside_radius;
			m_radius        = radius;
			m_inner_cut_off = inner_cut_off;
			m_outer_cut_off = outer_cut_off;
		}

		//init point type
		void point
		(
			const glm::vec3& diffuse,
			const glm::vec3& specular,
			float            constant,
			float            inside_radius,
			float            radius
		)
		{
			m_type = POINT_LIGHT;
			m_diffuse = diffuse;
			m_specular = specular;
			m_constant = constant;
			m_inside_radius = inside_radius;
			m_radius = radius;
		}

		//init sport type
		void spot
		(
			const glm::vec3& diffuse,
			const glm::vec3& specular,
			float            constant,
			float            inside_radius,
			float            radius,
			float            inner_cut_off,
			float            outer_cut_off
		)
		{
			m_type = SPOT_LIGHT;
			m_diffuse = diffuse;
			m_specular = specular;
			m_constant = constant;
			m_inside_radius = inside_radius;
			m_radius = radius;
			m_inner_cut_off = std::cos(inner_cut_off);
			m_outer_cut_off = std::cos(outer_cut_off);
		}

		//init direction type
		void direction
		(
			const glm::vec3& diffuse,
			const glm::vec3& specular
		)
		{
			m_type = DIRECTION_LIGHT;
			m_diffuse = diffuse;
			m_specular = specular;
		}
		//copy
		virtual component_ptr copy() const;
	};
	using light_ptr = std::shared_ptr< light >;
	using light_uptr = std::unique_ptr< light >;
	using light_wptr = std::weak_ptr< light >;

	template < class... Args >
	static inline light_ptr light_snew(Args&&... args)
	{
		return std::make_shared< light >(args...);
	}
	
	struct uniform_light
	{
		uniform* m_uniform_type;

		uniform* m_uniform_position;
		uniform* m_uniform_direction;

		uniform* m_uniform_diffuse;
		uniform* m_uniform_specular;

		uniform* m_uniform_inv_constant;
		uniform* m_uniform_inv_quad_inside_radius;
		uniform* m_uniform_inv_quad_radius;

		uniform* m_uniform_inner_cut_off;
		uniform* m_uniform_outer_cut_off;

		void get_uniform(int i, shader::ptr shader);

		void uniform(light_wptr light, const glm::mat4& view, const glm::mat4& model);
	};

}