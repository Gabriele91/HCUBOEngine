//
//  rendering_pass_shadow.cpp
//  HCubo
//
//  Created by Gabriele on 15/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <rendering_pass_shadow.h>

namespace hcube
{
    rendering_pass_shadow::rendering_pass_shadow(resources_manager& resources)
    :rendering_pass(RPT_SHADOW)
    {
        m_effect					 = resources.get_effect("build_shadow");
        m_technique_shadow_spot      = m_effect->get_technique("shadow_spot");
        m_technique_shadow_point     = m_effect->get_technique("shadow_point");
        m_technique_shadow_direction = m_effect->get_technique("shadow_direction");
        //params
        m_mask        = m_effect->get_parameter("mask");
        m_diffuse_map = m_effect->get_parameter("diffuse_map");
        //get uniform mask
        if(m_technique_shadow_spot && (*m_technique_shadow_spot).size())
        {
            m_shadow_spot_mask       = (*m_technique_shadow_spot)[0].m_shader->get_uniform("mask");
            m_shadow_spot_projection = (*m_technique_shadow_spot)[0].m_shader->get_uniform("projection");
            m_shadow_spot_view       = (*m_technique_shadow_spot)[0].m_shader->get_uniform("view");
            m_shadow_spot_model		 = (*m_technique_shadow_spot)[0].m_shader->get_uniform("model");
        }
        if(m_technique_shadow_point && (*m_technique_shadow_point).size())
        {
            m_shadow_point_mask			  = (*m_technique_shadow_point)[0].m_shader->get_uniform("mask");
            m_shadow_point_light_position = (*m_technique_shadow_point)[0].m_shader->get_uniform("light_position");
            m_shadow_point_far_plane	  = (*m_technique_shadow_point)[0].m_shader->get_uniform("far_plane");
            m_shadow_point_projection     = (*m_technique_shadow_point)[0].m_shader->get_uniform("projection");
            m_shadow_point_view			  = (*m_technique_shadow_point)[0].m_shader->get_uniform("view[0]");
            m_shadow_point_model	      = (*m_technique_shadow_point)[0].m_shader->get_uniform("model");
        }
        if(m_technique_shadow_direction && (*m_technique_shadow_direction).size())
        {
            m_shadow_direction_mask		    = (*m_technique_shadow_direction)[0].m_shader->get_uniform("mask");
            m_shadow_direction_projection	= (*m_technique_shadow_direction)[0].m_shader->get_uniform("projection");
            m_shadow_direction_view			= (*m_technique_shadow_direction)[0].m_shader->get_uniform("view[0]");
            m_shadow_direction_model		= (*m_technique_shadow_direction)[0].m_shader->get_uniform("model");
        }
    }
    
    void rendering_pass_shadow::draw_pass
    (
     int    n_pass,
     vec4&  clear_color,
     vec4&  ambient_color,
     entity::ptr e_light,
     render_scene& rscene
     )
    {
        //shadow map
        auto l_light = e_light->get_component<light>();
        //enable shadow?
        if (!l_light->is_enable_shadow()) return;
        //current technique
        effect::technique* current_technique = nullptr;
        //current mask uniform
        uniform* u_shadow_mask = nullptr;
        //current u
        uniform* u_shadow_projection = nullptr;
        uniform* u_shadow_view	     = nullptr;
        uniform* u_shadow_model		 = nullptr;
        //type
        switch (l_light->get_type())
        {
            case light::SPOT_LIGHT:
                current_technique   = m_technique_shadow_spot;
                u_shadow_mask	    = m_shadow_spot_mask;
                u_shadow_projection = m_shadow_spot_projection;
                u_shadow_view	    = m_shadow_spot_view;
                u_shadow_model		= m_shadow_spot_model;
                break;
            case light::POINT_LIGHT:
                current_technique = m_technique_shadow_point;
                u_shadow_mask	  = m_shadow_point_mask;
                u_shadow_projection = m_shadow_point_projection;
                u_shadow_view       = m_shadow_point_view;
                u_shadow_model      = m_shadow_point_model;
                break;
            default: return;
        }
        //pass
        effect::pass& shadow_pass = (*current_technique)[0];
        //enable shadow buffer/texture
        l_light->get_shadow_buffer().bind();
        //clear
        render::clear(CLEAR_DEPTH);
        //applay pass
        auto state = shadow_pass.safe_bind();
        //default uniform
        u_shadow_projection->set_value(l_light->get_projection());
        //////////////////////////////////////////////////////////////////////////////
        //spot light vs point lights
        switch (l_light->get_type())
        {
            case light::SPOT_LIGHT:
                //update view frustum and queue
                rscene.compute_no_lights_queues (l_light->update_frustum());
                //set view
                u_shadow_view->set_value(l_light->get_view());
                break;
            case light::POINT_LIGHT:
                //build queue by sphere
                rscene.compute_no_lights_queues(
					e_light->get_component<transform>()->get_global_position(), 
					l_light->get_radius()
                );
                //uniform
                u_shadow_view->set_value(l_light->get_cube_view());
                //position
                if (m_shadow_point_light_position)
                    m_shadow_point_light_position->set_value(
                            e_light->get_component<transform>()->get_global_position()
                    );
                //radius
                if (m_shadow_point_far_plane)
                    m_shadow_point_far_plane->set_value(l_light->get_radius());
                break;
            default: break;
        }
        //////////////////////////////////////////////////////////////////////////////
        //default texture
        texture::ptr default_texture = m_diffuse_map->get_texture();
        //set viewport
        render::set_viewport_state({ l_light->get_viewport() });
        //draw objs
        render_queue_type queue_shadow[]{ RQ_OPAQUE, RQ_TRANSLUCENT };
        //for all types
        for(render_queue_type queue_type : queue_shadow)
        {
            HCUBE_FOREACH_QUEUE(weak_element, rscene.get_first(queue_type))
            {
                auto entity     = weak_element->lock();
                auto t_entity   = entity->get_component<transform>();
                auto r_entity   = entity->get_component<renderable>();
                //events
                bool do_default_tex = true;
                bool do_default_mask= true;
                //test
                if (auto e_material = r_entity->get_material())
                {
                    if (auto p_texture  = e_material->get_default_parameter(material::MAT_DEFAULT_DIFFUSE_MAP))
                        if (auto t_texture  = p_texture->get_texture())
                        {
                            //diffuse map
                            render::bind_texture(t_texture->get_context_texture(), 0);
                            //not bind default
                            do_default_tex = false;
                        }
                    if (auto p_mask  = e_material->get_default_parameter(material::MAT_DEFAULT_MASK))
                    {
                        //uniform
                        u_shadow_mask->set_value(p_mask->get_float());
                        //not bind default
                        do_default_mask = false;
                    }
                }
                //diffuse map
                if(do_default_tex)  render::bind_texture(default_texture->get_context_texture(), 0);
                //uniform
                if(do_default_mask) u_shadow_mask->set_value(m_mask->get_float());
                //set transform
                u_shadow_model->set_value(t_entity->get_matrix());
                //draw
                r_entity->draw();
            }
        }
        //disable shadow buffer/texture
        l_light->get_shadow_buffer().unbind();
        //disable
        shadow_pass.safe_unbind(state);
    }
}