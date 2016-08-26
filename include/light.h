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
#include <vector_math.h>
#include <shader.h>
#include <camera.h>
#include <shadow_buffer.h>


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
		vec3  m_diffuse { 1.0, 1.0, 1.0 };
		vec3  m_specular{ 1.0, 1.0, 1.0 };
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
			light_type  type,
			const vec3& diffuse,
			const vec3& specular,
			float       constant,
			float       inside_radius,
			float       radius,
			float       inner_cut_off,
			float       outer_cut_off
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
			const vec3& diffuse,
			const vec3& specular,
			float       constant,
			float       inside_radius,
			float       radius
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
			const vec3& diffuse,
			const vec3& specular,
			float       constant,
			float       inside_radius,
			float       radius,
			float       inner_cut_off,
			float       outer_cut_off
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
			const vec3& diffuse,
			const vec3& specular
		)
		{
			m_type = DIRECTION_LIGHT;
			m_diffuse = diffuse;
			m_specular = specular;
		}

		//shadow
		bool set_shadow(const ivec2& size);

		void disable_shadow();

		bool is_enable_shadow() const;

		camera::ptr get_shadow_camera() const;

		frustum* get_frustum() const;

		const shadow_buffer& get_shadow_buffer() const;
		//copy
		virtual component_ptr copy() const;

	protected:
		//shadow struct
		struct light_shadow
		{
			bool		  m_enable{ false   };
			camera::ptr   m_camera{ nullptr };
			shadow_buffer m_buffer;
		}
		m_shadow;
		//update
		void update_shadow_projection_matrix();

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

		void uniform(light_wptr light, const mat4& view, const mat4& model);
    
        bool is_valid() const;
        
    protected:
        
        bool m_valid{ false };
        
	};

}