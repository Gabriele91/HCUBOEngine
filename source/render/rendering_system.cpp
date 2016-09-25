//
//  rendering_system.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright � 2016 Gabriele. All rights reserved.
//
#include <iostream>
#include <hcube/core/entity.h>
#include <hcube/core/resources_manager.h>
#include <hcube/render/rendering_system.h>
#include <hcube/components/gameobject.h>
#include <hcube/components/transform.h>
#include <hcube/components/camera.h>

namespace hcube
{

	void rendering_system::on_attach(system_manager& sm)
	{
		for (auto& e : sm.get_entities()) on_add_entity(e);
	}

	void rendering_system::on_detach()
	{
		m_camera = nullptr;
		m_scene.clear();
	}

	void rendering_system::on_add_entity(entity::ptr e)
	{
		//copy ref
		if (e->has_component<camera>())     m_camera = e;
		else							    m_scene.m_pool.push(e);
	}

	void rendering_system::on_remove_entity(entity::ptr e)
	{
		//remove camera
		if (m_camera == e) m_camera = nullptr;
		//remove obj
		else               m_scene.m_pool.remove(e);
	}

	void rendering_system::on_update(double deltatime)
	{
		draw();
	}

	void rendering_system::set_clear_color(const vec4& clear_color)
	{
		m_clear_color = clear_color;
	}

	void rendering_system::set_ambient_color(const vec4& ambient_color)
	{
		m_ambient_color = ambient_color;
	}

	void rendering_system::add_rendering_pass(rendering_pass_ptr pass)
	{
		m_rendering_pass[pass->get_type()].push_back(pass);
	}
    
    rendering_pass_debug_spot_lights::rendering_pass_debug_spot_lights(resources_manager& resources)
    :rendering_pass(RPT_UI)
    {
        m_effect = resources.get_effect("debug_lights");
        ////// SPHERE //////////////////////////////////////////////////////////////////////////////
        //get mesh
        auto root_sphere = resources.get_prefab("sphere")->instantiate();
        //get sphere
        auto e_sphere = root_sphere->get_childs().begin()->second;
        //get component
        m_sphere = std::static_pointer_cast<renderable>( e_sphere->get_component<renderable>()->copy() );
        ////// CONE //////////////////////////////////////////////////////////////////////////////
        //get mesh
        auto root_cone = resources.get_prefab("cone")->instantiate();
        //get sphere
        auto e_cone = root_cone->get_childs().begin()->second;
        //get component
        m_cone = std::static_pointer_cast<renderable>(e_cone->get_component<renderable>()->copy());
    }
    
    void rendering_pass_debug_spot_lights::draw_pass
    (
        int    n_pass,
        vec4&  clear_color,
        vec4&  ambient_color,
        entity::ptr e_camera,
        render_scene& rscene
    )
    {
        
        effect::pass& pass = (*m_effect->get_technique("forward"))[0];

		auto c_camera = e_camera->get_component<camera>();
		auto t_camera = e_camera->get_component<transform>();
        auto state = render::get_render_state();
        //viewport
        render::set_viewport_state({ c_camera->get_viewport() });
        //bind
        pass.bind();
        
        //for all lights
        HCUBE_FOREACH_QUEUE(weak_light, rscene.get_first(RQ_SPOT_LIGHT))
        {
            auto e_light    = weak_light->lock();
            auto l_light    = e_light->get_component<light>();
            auto t_light    = e_light->get_component<transform>();
            
            l_light->update_projection_matrix();
            
            pass.m_shader
                ->get_uniform("viewport")
                ->set_value(c_camera->get_viewport());
            pass.m_shader
				->get_uniform("projection")
				->set_value(c_camera->get_projection());
			pass.m_shader
				->get_uniform("view")
				->set_value(c_camera->get_view());
            
            mat4
            model  = translate(mat4(1), t_light->get_global_position());
            model *= mat4_cast(t_light->get_global_rotation());
            model  = scale(model, { l_light->get_radius(), l_light->get_radius(),l_light->get_radius() });
            
			pass.m_shader->get_uniform("model")->set_value(model);

            m_cone->draw();
        }
        HCUBE_FOREACH_QUEUE(weak_light, rscene.get_first(RQ_POINT_LIGHT))
        {
            auto e_light    = weak_light->lock();
            auto l_light    = e_light->get_component<light>();
            auto t_light    = e_light->get_component<transform>();
            
            l_light->update_projection_matrix();
            
            pass.m_shader
                ->get_uniform("viewport")
                ->set_value(c_camera->get_viewport());
            pass.m_shader
                ->get_uniform("projection")
                ->set_value(c_camera->get_projection());
            pass.m_shader
                ->get_uniform("view")
                ->set_value(c_camera->get_view());
            
            mat4
            model  = translate(mat4(1), t_light->get_global_position());
            model  = scale(model, { l_light->get_radius(), l_light->get_radius(),l_light->get_radius() });
            
            pass.m_shader->get_uniform("model")->set_value(model);
            
            m_sphere->draw();
        }
        
        
        pass.unbind();
        //end
        render::set_render_state(state);
    }

	void rendering_system::draw()
	{
		//culling
		camera::ptr     c_camera = m_camera->get_component<camera>();
		const frustum&  f_camera = c_camera->get_frustum();
		//update view frustum
		if (m_update_frustum) c_camera->update_frustum();
        //lights queue
        m_scene.compute_lights_queues(f_camera);
        //n shadow pass
        int n_shadow_pass = 0;
        //build shadow map
        for(rendering_pass_ptr& shadow_pass : m_rendering_pass[RPT_SHADOW])
		{
			//spot lights
			HCUBE_FOREACH_QUEUE(weak_light, m_scene.get_first(RQ_SPOT_LIGHT))
			shadow_pass->draw_pass
			(
                n_shadow_pass,
				m_clear_color,
				m_ambient_color,
                weak_light->lock(),
                m_scene
			);
			//point lights
			HCUBE_FOREACH_QUEUE(weak_light, m_scene.get_first(RQ_POINT_LIGHT))
			shadow_pass->draw_pass
			(
                n_shadow_pass,
				m_clear_color,
				m_ambient_color,
                weak_light->lock(),
                m_scene
			);
            //next
            ++n_shadow_pass;
        }
        //error
        render::print_errors();
		//viewport
		render::set_viewport_state({ c_camera->get_viewport() });
        //color
        render::set_clear_color_state(m_clear_color);
		//clear
        render::clear(CLEAR_COLOR_DEPTH);
        //objects queue
        m_scene.compute_no_lights_queues(f_camera);
        //error
        render::print_errors();
        //int npass
        int n_render_pass = -1;
        //all passes
        for (rendering_pass_ptr& pass : m_rendering_pass[RPT_RENDER])
        {
            pass->draw_pass
            (
              ++n_render_pass,
              m_clear_color,
              m_ambient_color,
              m_camera,
              m_scene
             );
            //error
        }
		//int npass
		int n_ui_pass = -1;
        //all passes
        for (rendering_pass_ptr& pass : m_rendering_pass[RPT_UI])
        {
            pass->draw_pass
            (
             ++n_ui_pass,
             m_clear_color,
             m_ambient_color,
             m_camera,
             m_scene
            );
        }
	}

	const vec4& rendering_system::get_clear_color() const
	{
		return m_clear_color;
	}

	const vec4& rendering_system::get_ambient_color() const
	{
		return m_ambient_color;
	}

	entity::ptr rendering_system::get_camera() const
	{
		return m_camera;
	}

	const std::vector< rendering_pass_ptr >& rendering_system::get_rendering_pass(rendering_pass_type type) const
	{
		return m_rendering_pass[type];
	}
	
	void rendering_system::stop_update_frustum(bool stop_update)
	{
		m_update_frustum = !stop_update;
	}

	void rendering_system::stop_frustum_culling(bool stop_culling)
	{
		m_stop_frustum_culling = stop_culling;
	}
}