//
//  rendering_system.h
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <queue>
#include <camera.h>
#include <entity.h>
#include <effect.h>
#include <smart_pointers.h>
#include <system_manager.h>
#include <vector_math.h>
#include <render_queues.h>

namespace hcube
{

	class rendering_pass
	{
	public:
		virtual void draw_pass(
			vec4&  clear_color,
			vec4&  ambient_color,
			entity::ptr e_camera,
			render_scene& rscene
		) = 0;
	};
	using rendering_pass_ptr = std::shared_ptr< rendering_pass >;
	using rendering_pass_uptr = std::unique_ptr< rendering_pass >;

	class rendering_pass_shadow : public rendering_pass, public smart_pointers<rendering_pass_shadow>
	{
		
        effect::ptr		   m_effect;
        effect::parameter* m_mask;
        effect::parameter* m_diffuse_map;
		effect::technique* m_technique_shadow_spot;
		effect::technique* m_technique_shadow_point;
        effect::technique* m_technique_shadow_direction;
		//spot light
        uniform*           m_shadow_spot_mask		    { nullptr };
		//point light
		uniform*           m_shadow_point_mask			{ nullptr };
		uniform*           m_shadow_point_light_position{ nullptr };
		uniform*           m_shadow_point_far_plane		{ nullptr };
		//direction light
        uniform*           m_shadow_direction_mask		{ nullptr };
		//view / projection
		uniform*           m_shadow_spot_model      { nullptr };
		uniform*           m_shadow_spot_view		{ nullptr };
		uniform*           m_shadow_spot_projection { nullptr };

		uniform*           m_shadow_point_model     { nullptr };
		uniform*           m_shadow_point_view      { nullptr };
		uniform*           m_shadow_point_projection{ nullptr };

		uniform*           m_shadow_direction_model	    { nullptr };
		uniform*           m_shadow_direction_view	    { nullptr };
		uniform*           m_shadow_direction_projection{ nullptr };

	public:

		rendering_pass_shadow(resources_manager& resources);

		virtual void draw_pass(
			vec4&  clear_color,
			vec4&  ambient_color,
            entity::ptr e_camera,
            render_scene& rscene
		);
	};
    
    class rendering_pass_debug_spot_lights : public rendering_pass, public smart_pointers<rendering_pass_debug_spot_lights>
    {
        
        effect::ptr     m_effect;
        renderable_ptr  m_cone;
        renderable_ptr  m_sphere;
        
    public:
        
        rendering_pass_debug_spot_lights(resources_manager& resources);
        
        virtual void draw_pass(
                               vec4&  clear_color,
                               vec4&  ambient_color,
                               entity::ptr e_camera,
                               render_scene& rscene
                               );
    };

	class rendering_system : public system_component, public smart_pointers< rendering_system >
	{

		HCUBE_SYSTEM_COMPONENT_DEC(rendering_system);

	public:


		virtual void on_attach(system_manager&);

		virtual void on_detach();

		virtual void on_add_entity(entity::ptr);

		virtual void on_remove_entity(entity::ptr);

		virtual void on_update(double deltatime);

		void set_clear_color(const vec4& clear_color);

		void set_ambient_color(const vec4& ambient_color);

		void add_rendering_pass(rendering_pass_ptr pass);

		void add_shadow_rendering_pass(rendering_pass_ptr pass);

		void draw();

		const vec4& get_clear_color() const;

		const vec4& get_ambient_color() const;

		entity::ptr get_camera() const;

		const std::vector< rendering_pass_ptr >& get_rendering_pass() const;

		void stop_update_frustum(bool stop_update);

		void stop_frustum_culling(bool stop_culling);

	protected:
		
		bool m_update_frustum{ true };
		bool m_stop_frustum_culling{ false };

		vec4                              m_clear_color;
		vec4                              m_ambient_color;
		entity::ptr						  m_camera;
		render_scene					  m_scene;
		std::vector< rendering_pass_ptr > m_rendering_pass;
		//light
		rendering_pass_ptr				  m_shadow_pass;
	};
}