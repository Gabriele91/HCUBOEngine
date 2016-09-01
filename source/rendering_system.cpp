//
//  rendering_system.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright � 2016 Gabriele. All rights reserved.
//
#include <transform.h>
#include <camera.h>
#include <entity.h>
#include <rendering_system.h>
#include <resources_manager.h>


namespace hcube
{
	void render_queues::clear()
	{
		m_lights.clear();
		m_opaque.clear();
	}

	void render_queues::remove(entity::ptr e)
	{
		//search light and remove
		for (auto it_light = m_lights.begin();
			it_light != m_lights.end();
			++it_light)
		{
			if (it_light->lock() == e)
			{
				m_lights.erase(it_light);
				break;
			}
		}
		//search opaque renderable object and remove
		for (auto it_renderable = m_opaque.begin();
			it_renderable != m_opaque.end();
			++it_renderable)
		{
			if (it_renderable->lock() == e)
			{
				m_opaque.erase(it_renderable);
				break;
			}
		}
		//search translucent renderable object and remove
		for (auto it_renderable = m_translucent.begin();
			it_renderable != m_translucent.end();
			++it_renderable)
		{
			if (it_renderable->lock() == e)
			{
				m_translucent.erase(it_renderable);
				break;
			}
		}
	}

	void render_queues::push(entity::ptr e)
	{
		if (e->has_component<light>())      m_lights.push_back({ e });
		if (e->has_component<renderable>()) m_opaque.push_back({ e });
		//else in m_translucent
	}

	void render_queues::reserve(size_t size)
	{
		m_lights.reserve(size);
		m_opaque.reserve(size);
		m_translucent.reserve(size);
	}

	void rendering_system::on_attach(system_manager& sm)
	{
		for (auto& e : sm.get_entities()) on_add_entity(e);
	}

	void rendering_system::on_detach()
	{
		m_camera = nullptr;
		m_renderables.clear();
	}

	void rendering_system::on_add_entity(entity::ptr e)
	{
		//copy ref
		if (e->has_component<camera>())     m_camera = e;
		else							   m_renderables.push(e);
	}

	void rendering_system::on_remove_entity(entity::ptr e)
	{
		//remove camera
		if (m_camera == e) m_camera = nullptr;
		//remove obj
		else              m_renderables.remove(e);
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
		m_rendering_pass.push_back(pass);
	}

	void rendering_system::add_shadow_rendering_pass(rendering_pass_ptr pass)
	{
		m_shadow_pass = pass;
	}

	static inline float compute_camera_depth(const frustum& f_camera, const transform_ptr& t_entity)
	{
		return f_camera.distance_from_near_plane((vec3)(t_entity->get_matrix()*vec4(0, 0, 0, 1)));
	}

	void render_queues::add_call_light_spot(element* e)
	{
		// next to void
		e->m_next = nullptr;
		// loop vars
		element* last = nullptr;
		element* current = m_cull_light_spot;
		// insert sort, front to back
		for (; current;
			   last = current,
			   current = current->m_next)
		{
			if (current->m_depth < e->m_depth) break;
		}
		// last iteration
		if (last)
		{
			e->m_next = current;
			last->m_next = e;
		}
		else
		{
			e->m_next = m_cull_light_spot;
			m_cull_light_spot = e;
		}
	}

	void render_queues::add_call_light_point(element* e)
	{
		// next to void
		e->m_next = nullptr;
		// loop vars
		element* last = nullptr;
		element* current = m_cull_light_point;
		// insert sort, front to back
		for (; current;
			last = current,
			current = current->m_next)
		{
			if (current->m_depth < e->m_depth) break;
		}
		// last iteration
		if (last)
		{
			e->m_next = current;
			last->m_next = e;
		}
		else
		{
			e->m_next = m_cull_light_point;
			m_cull_light_point = e;
		}
	}

	void render_queues::add_call_light_direction(element* e)
	{
		// next to void
		e->m_next = nullptr;
		// loop vars
		element* last = nullptr;
		element* current = m_cull_light_direction;
		// insert sort, front to back
		for (; current;
			last = current,
			current = current->m_next)
		{
			if (current->m_depth < e->m_depth) break;
		}
		// last iteration
		if (last)
		{
			e->m_next = current;
			last->m_next = e;
		}
		else
		{
			e->m_next = m_cull_light_direction;
			m_cull_light_direction = e;
		}
	}
	
	void render_queues::add_call_opaque(element* e)
	{
		e->m_next = nullptr;
		//loop vars
		element* last = nullptr;
		element* current = m_cull_opaque;
		//insert sort, front to back
		for (; current;
			   last = current,
			   current = current->m_next)
		{
			if (current->m_depth < e->m_depth) break;
		}
		//last iteration
		if (last)
		{
			e->m_next = current;
			last->m_next = e;
		}
		else
		{
			e->m_next = m_cull_opaque;
			m_cull_opaque = e;
		}
	}

	void render_queues::add_call_translucent(element* e)
	{
		e->m_next = nullptr;
		//loop vars
		element* last = nullptr;
		element* current = m_cull_translucent;
		//insert sort, back to front
		for (; current;
			last = current,
			current = current->m_next)
		{
			if (current->m_depth > e->m_depth) break;
		}
		//last iteration
		if (last)
		{
			e->m_next = current;
			last->m_next = e;
		}
		else
		{
			e->m_next = m_cull_translucent;
			m_cull_translucent = e;
		}
	}


	void render_queues::compute_light_queue(const frustum& view_frustum)
	{
		m_cull_light_spot	   = nullptr;
		m_cull_light_point	   = nullptr;
		m_cull_light_direction = nullptr;

		//build queue lights
		for (render_queues::element& weak_element : m_lights)
		{
			auto entity = weak_element.lock();
			auto t_entity = entity->get_component<transform>();
			auto l_entity = entity->get_component<light>();
			const auto l_pos = (vec3)(t_entity->get_matrix()*vec4(0, 0, 0, 1.));
			const auto l_radius = l_entity->get_radius();
			
			if (l_entity->is_enabled() && view_frustum.test_sphere(l_pos, l_radius))
			{
				weak_element.m_depth = compute_camera_depth(view_frustum, t_entity);
				switch (l_entity->get_type())
				{
					case light::SPOT_LIGHT:      add_call_light_spot(&weak_element);     break;
					case light::POINT_LIGHT:     add_call_light_point(&weak_element);     break;
					case light::DIRECTION_LIGHT: add_call_light_direction(&weak_element); break;
					default: break;
				};

			}
		}

	}

	void render_queues::compute_opaque_queue(const frustum& view_frustum)
	{
		m_cull_opaque = nullptr;
		//build queue opaque
		for (render_queues::element& weak_element : m_opaque)
		{
			auto entity = weak_element.lock();
			auto t_entity = entity->get_component<transform>();
			auto r_entity = entity->get_component<renderable>();

			if (r_entity->is_enabled() &&
				( !r_entity->has_support_culling() ||
				   view_frustum.test_obb(r_entity->get_bounding_box(), t_entity->get_matrix())) )
			{
				weak_element.m_depth = compute_camera_depth(view_frustum, t_entity);
				add_call_opaque(&weak_element);
			}
		}

	}

	void render_queues::compute_translucent_queue(const frustum& view_frustum)
	{
		//init
		m_cull_translucent = nullptr;
		//build queue translucent
		for (render_queues::element& weak_element : m_translucent)
		{
			auto entity = weak_element.lock();
			auto t_entity = entity->get_component<transform>();
			auto r_entity = entity->get_component<renderable>();

			if (r_entity->is_enabled() &&
				(!r_entity->has_support_culling() ||
					view_frustum.test_obb(r_entity->get_bounding_box(), t_entity->get_matrix()))
				)
			{
				weak_element.m_depth = compute_camera_depth(view_frustum, t_entity);
				add_call_translucent(&weak_element);
			}
		}

	}

	rendering_pass_shadow::rendering_pass_shadow(resources_manager& resources)
	{
		m_effect		   = resources.get_effect("build_shadow");
		m_technique_shadow_spot      = m_effect->get_technique("shadow_spot");
		m_technique_shadow_point     = m_effect->get_technique("shadow_point");
		m_technique_shadow_direction = m_effect->get_technique("shadow_direction");
        //params
        m_mask        = m_effect->get_parameter("mask");
        m_diffuse_map = m_effect->get_parameter("diffuse_map");
        //get uniform mask
        if(m_technique_shadow_spot && (*m_technique_shadow_spot).size())
        {
            m_shadow_spot_mask = (*m_technique_shadow_spot)[0].m_shader->get_uniform("mask");
        }
        if(m_technique_shadow_point && (*m_technique_shadow_point).size())
        {
            m_shadow_point_mask = (*m_technique_shadow_point)[0].m_shader->get_uniform("mask");
        }
        if(m_technique_shadow_direction && (*m_technique_shadow_direction).size())
        {
            m_shadow_direction_mask = (*m_technique_shadow_direction)[0].m_shader->get_uniform("mask");
        }
	}

	void rendering_pass_shadow::draw_pass(
		vec4&  clear_color,
		vec4&  ambient_color,
		entity::ptr e_light,
		render_queues& queues
	)
	{
		//pass
		effect::pass& shadow_pass = (*m_technique_shadow_spot)[0];
		//shadow map
		auto l_light = e_light->get_component<light>();
		//enable shadow?
		if (!l_light->is_enable_shadow()) return;
		//update view frustum and queue
		queues.compute_opaque_queue(l_light->update_frustum());
		//enable shadow buffer/texture
		l_light->get_shadow_buffer().bind();
		//clear
		render::clear();
		//get transform
		transform_ptr t_light = e_light->get_component<transform>();
		//applay pass
		auto state = shadow_pass.safe_bind
		(
			l_light->get_viewport(),
			l_light->get_projection(),
			t_light->get_matrix_inv(),
			mat4(1)
		);
		//default texture
		texture::ptr default_texture = m_diffuse_map->get_texture();
		//set viewport
		render::set_viewport_state({ l_light->get_viewport() });
		//draw objs
		HCUBE_FOREACH_QUEUE(weak_element, queues.m_cull_opaque)
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
                    m_shadow_spot_mask->set_value(p_mask->get_float());
                    //not bind default
                    do_default_mask = false;
                }
            }
            //diffuse map
            if(do_default_tex)  render::bind_texture(default_texture->get_context_texture(), 0);
            //uniform
            if(do_default_mask) m_shadow_spot_mask->set_value(m_mask->get_float());
			//set transform
			shadow_pass.m_uniform_model->set_value(t_entity->get_matrix());
			//draw
			r_entity->draw();
		}
		//disable
		shadow_pass.safe_unbind(state);
		//disable shadow buffer/texture
		l_light->get_shadow_buffer().unbind();
	}

	void rendering_system::draw()
	{
		//culling
		camera::ptr   c_camera = m_camera->get_component<camera>();
		transform_ptr t_camera = m_camera->get_component<transform>();
		frustum&      f_camera = c_camera->get_frustum();
		//update view frustum
		f_camera.update_frustum(c_camera->get_projection()*t_camera->get_matrix_inv());
		//update queue
		m_renderables.compute_light_queue(f_camera);
		//build shadow map
		if (m_shadow_pass) HCUBE_FOREACH_QUEUE(weak_light, m_renderables.m_cull_light_spot)
		{
			//get light
			auto e_light = weak_light->lock();
			//get pass
			m_shadow_pass->draw_pass
			(
				m_clear_color,
				m_ambient_color,
				e_light,
				m_renderables
			);
		}
		//update queue
		m_renderables.compute_opaque_queue(f_camera);
		m_renderables.compute_translucent_queue(f_camera);
		//all passes
		for (rendering_pass_ptr& pass : m_rendering_pass)
		{
			pass->draw_pass
			(
				m_clear_color,
				m_ambient_color,
				m_camera,
				m_renderables
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

	const std::vector< rendering_pass_ptr >& rendering_system::get_rendering_pass() const
	{
		return m_rendering_pass;
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