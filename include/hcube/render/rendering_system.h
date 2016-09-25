//
//  rendering_system.h
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <hcube/math/vector_math.h>
#include <hcube/core/entity.h>
#include <hcube/core/smart_pointers.h>
#include <hcube/core/system_manager.h>
#include <hcube/render/render_scene.h>
#include <hcube/render/effect.h>
#include <hcube/components/camera.h>
#include <hcube/components/renderable.h>

namespace hcube
{

    enum rendering_pass_type
    {
        RPT_SHADOW,
        RPT_RENDER,
        RPT_UI,
        RPT_MAX
    };
    
	class rendering_pass
	{
        
	public:
        
        rendering_pass(rendering_pass_type type)
        {
            m_type = type;
        }
        
		virtual void draw_pass(
            int    n_pass,
			vec4&  clear_color,
			vec4&  ambient_color,
			entity::ptr e_camera,
			render_scene& rscene
		) = 0;
        
        rendering_pass_type get_type()
        {
            return m_type;
        }
        
    private:
        
        rendering_pass_type m_type;
        
	};
	using rendering_pass_ptr = std::shared_ptr< rendering_pass >;
	using rendering_pass_uptr = std::unique_ptr< rendering_pass >;

    
    class rendering_pass_debug_spot_lights : public rendering_pass, public smart_pointers<rendering_pass_debug_spot_lights>
    {
        
        effect::ptr     m_effect;
        renderable_ptr  m_cone;
        renderable_ptr  m_sphere;
        
    public:
        
        rendering_pass_debug_spot_lights(resources_manager& resources);
        
        virtual void draw_pass(
                               int    n_pass,
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

		void draw();

		const vec4& get_clear_color() const;

		const vec4& get_ambient_color() const;

		entity::ptr get_camera() const;

		const std::vector< rendering_pass_ptr >& get_rendering_pass(rendering_pass_type type = RPT_RENDER) const;

		void stop_update_frustum(bool stop_update);

		void stop_frustum_culling(bool stop_culling);

	protected:
		
		bool m_update_frustum{ true };
		bool m_stop_frustum_culling{ false };
        //global into
		vec4                              m_clear_color;
		vec4                              m_ambient_color;
		entity::ptr						  m_camera;
		render_scene					  m_scene;
        //all paths
		std::vector< rendering_pass_ptr > m_rendering_pass[RPT_MAX];
	};
}