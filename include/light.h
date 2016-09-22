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
#include <sphere.h>


namespace hcube
{

	class light;
	struct uniform_light_spot;
	struct uniform_light_point;
	struct uniform_light_direction;

	class light : public component
	{

		HCUBE_COMPONENT_DEC(light)

	public:
		//light type
		enum light_type
		{
			DIRECTION_LIGHT = 0,
			POINT_LIGHT = 1,
			SPOT_LIGHT = 2
		};
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
            update_projection_matrix();
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
            update_projection_matrix();
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
            update_projection_matrix();
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

		//change color
		void set_color
		(
			const vec3& diffuse,
			const vec3& specular
		)
		{
			m_diffuse = diffuse;
			m_specular = specular;
		}

		//change radius
		void set_radius
		(
			float       inside_radius,
			float       radius
		)
		{
			m_inside_radius = inside_radius;
			m_radius = radius;
			update_projection_matrix();
		}

		void set_radius
		(
			float       constant,
			float       inside_radius,
			float       radius
		)
		{
			m_constant = constant;
			m_inside_radius = inside_radius;
			m_radius = radius;
			update_projection_matrix();
		}
		//change cone
		void set_spot
		(
			float       inner_cut_off,
			float       outer_cut_off
		)
		{
			m_inner_cut_off = std::cos(inner_cut_off);
			m_outer_cut_off = std::cos(outer_cut_off);
			update_projection_matrix();
		}

		//get
		light_type get_type() const
		{
			return m_type;
		}

		float get_radius() const
		{
			return m_radius;
		}

		//sphere culling
		const sphere& get_sphere()
		{
			update_sphere();
			return m_sphere;
		}

		//event
		void on_attach(entity& entity);

		void on_detach();

		bool on_activate();

		void on_message(const message& message);

		//shadow
		bool set_shadow(const ivec2& size);

		void disable_shadow();

		bool is_enable_shadow() const;

		const ivec4& get_viewport() const;
        
        const mat4& get_projection() const;
        
        const mat4& get_view();

		const std::vector<mat4>& get_cube_view();

		const frustum& get_frustum() const;

		const frustum& update_frustum();

        const shadow_buffer& get_shadow_buffer() const;

        //update
		void update_projection_matrix();

		void update_view_matrix();

		void update_sphere();

		//copy
		virtual component_ptr copy() const;

	protected:

		//type
		light_type m_type{ POINT_LIGHT };
		//light info
		vec3	   m_diffuse{ 1.0, 1.0, 1.0 };
		vec3	   m_specular{ 1.0, 1.0, 1.0 };
		//attenuation
		float      m_constant{ 1.0 };
		float      m_inside_radius{ 1.0 };
		float      m_radius{ 1.0 };
		//spot light info
		float      m_inner_cut_off{ -1.0 };
		float      m_outer_cut_off{ -1.0 };
		//frustum spot light
		frustum			    m_frustum;
		mat4			    m_projection;

		mat4				m_view;
		std::vector<mat4>   m_cube_view;
		bool				m_view_is_dirty{ true };

		sphere				m_sphere;
		bool				m_sphere_is_dirty{ true };
		//shadow struct
		struct light_shadow
		{
			bool		  m_enable{ false   };
			ivec4		  m_viewport;
			shadow_buffer m_buffer;
		}
		m_shadow;
		//friend
		friend class light;
		friend struct uniform_light_spot;
		friend struct uniform_light_point;
		friend struct uniform_light_direction;

	};
	using light_ptr = std::shared_ptr< light >;
	using light_uptr = std::unique_ptr< light >;
	using light_wptr = std::weak_ptr< light >;

	template < class... Args >
	static inline light_ptr light_snew(Args&&... args)
	{
		return std::make_shared< light >(args...);
	}
	
	struct uniform_light_spot
	{
		uniform* m_uniform_position { nullptr };
		uniform* m_uniform_direction{ nullptr };

		uniform* m_uniform_diffuse { nullptr };
		uniform* m_uniform_specular{ nullptr };

		uniform* m_uniform_constant     { nullptr };
		uniform* m_uniform_inside_radius{ nullptr };
		uniform* m_uniform_radius       { nullptr };

		uniform* m_uniform_inner_cut_off{ nullptr };
		uniform* m_uniform_outer_cut_off{ nullptr };

		uniform* m_uniform_use_shadow{ nullptr };
		uniform* m_uniform_shadow_projection{ nullptr };
		uniform* m_uniform_shadow_view{ nullptr };
		uniform* m_uniform_shadow_map{ nullptr };

		void get_uniform(shader::ptr shader);
		void get_uniform(const std::string& name,shader::ptr shader);

		void uniform(light_wptr light,
					 const mat4& model);
    
        bool is_valid() const;
        
    protected:
        
        bool m_valid{ false };
        
	};

	struct uniform_light_point
	{
		uniform* m_uniform_position{ nullptr };
		
		uniform* m_uniform_diffuse{ nullptr };
		uniform* m_uniform_specular{ nullptr };

		uniform* m_uniform_constant{ nullptr };
		uniform* m_uniform_inside_radius{ nullptr };
		uniform* m_uniform_radius{ nullptr };

		uniform* m_uniform_use_shadow{ nullptr };
		uniform* m_uniform_shadow_map{ nullptr };

		void get_uniform(shader::ptr shader);
		void get_uniform(const std::string& name, shader::ptr shader);

		void uniform(light_wptr light,
					 const mat4& model);

		bool is_valid() const;

	protected:

		bool m_valid{ false };

	};

	struct uniform_light_direction
	{
		uniform* m_uniform_direction{ nullptr };

		uniform* m_uniform_diffuse{ nullptr };
		uniform* m_uniform_specular{ nullptr };

		void get_uniform(shader::ptr shader);
		void get_uniform(const std::string& name, shader::ptr shader);

		void uniform(light_wptr light,
					 const mat4& model);

		bool is_valid() const;

	protected:

		bool m_valid{ false };

	};

}