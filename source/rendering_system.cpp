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


void render_queues::clear()
{
	m_lights.clear();
	m_opaque.clear();
}

void render_queues::remove(entity::ptr e)
{
	//search light and remove
	for (auto it_light  = m_lights.begin();
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
	for (auto it_renderable  = m_opaque.begin();
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
	for (auto it_renderable  = m_translucent.begin();
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

void rendering_system::on_attach( system_manager& sm )
{
    for(auto& e : sm.get_entities()) on_add_entity(e);
}

void rendering_system::on_detach()
{
    m_camera = nullptr;
	m_renderables.clear();
}

void rendering_system::on_add_entity(entity::ptr e)
{
    //copy ref
    if(e->has_component<camera>())     m_camera = e;
	else							   m_renderables.push(e);
}

void rendering_system::on_remove_entity(entity::ptr e)
{
    //remove camera
    if(m_camera == e) m_camera = nullptr;
	//remove obj
	else              m_renderables.remove(e);
}

void rendering_system::on_update(double deltatime)
{
    draw();
}

void rendering_system::set_clear_color(const glm::vec4& clear_color)
{
    m_clear_color = clear_color;
}

void rendering_system::set_ambient_color(const glm::vec4& ambient_color)
{
    m_ambient_color = ambient_color;
}

void rendering_system::add_rendering_pass(rendering_pass_ptr pass)
{
	m_rendering_pass.push_back(pass);
}


void rendering_pass_base::draw_pass(glm::vec4&  clear_color,
                                    glm::vec4&  ambient_color,
                                    entity::ptr e_camera,
									render_queues& queues)
{
    camera::ptr   ccamera = e_camera->get_component<camera>();
    transform_ptr tcamera = e_camera->get_component<transform>();
    const glm::vec4& viewport = ccamera->get_viewport();
    glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
	for (render_queues::element*
		 weak_element = queues.m_cull_opaque; 
		 weak_element;
		 weak_element = weak_element->m_next )
	{
        auto entity   = weak_element->lock();
		auto material = entity->get_component<renderable>()->get_material();
		//material
		if (material)
		{
			material->bind_state();
			material->bind(
				viewport,
				ccamera->get_projection(),
				tcamera->get_matrix_inv(),
				entity->get_component<transform>()->get_matrix()
			);
		}
		//draw
		entity->get_component<renderable>()->draw();
		//material
		if (material)
		{
			material->unbind();
			material->unbind_state();
		}
	}
}


static inline float compute_camera_depth(const frustum& f_camera,const transform_ptr& t_entity)
{
    return f_camera.distance_from_near_plane((glm::vec3)(t_entity->get_matrix()*glm::vec4(0,0,0,1)));
}

void render_queues::add_call_light(element* e)
{
	e->m_next = m_cull_light;
	m_cull_light = e;
}

void render_queues::add_call_opaque(element* e)
{
	element* last    = nullptr;
	element* current = m_cull_opaque;
	//insert sort, front to back
	for (; current;
		   last = current,
		   current = current->m_next)
	{
		//
		auto    entity = current->lock();
		auto  t_entity = entity->get_component<transform>();
		//
		if (current->m_depth < e->m_depth) break;
	}
	
	if (last)
	{
		e->m_next = current;
		last->m_next = e;
	}
	else
	{
		e->m_next     = m_cull_opaque;
		m_cull_opaque = e;
	}
}

void render_queues::add_call_translucent(element* e)
{
	element* last    = nullptr;
	element* current = m_cull_translucent;
	//insert sort, front to back
	for (; current;
		last = current,
		current = current->m_next)
	{
		//
		auto    entity = current->lock();
		auto  t_entity = entity->get_component<transform>();
		//
		if (current->m_depth > e->m_depth) break;
	}

	if (last)
	{
		e->m_next    = current;
		last->m_next = e;
	}
	else
	{
		e->m_next          = m_cull_translucent;
		m_cull_translucent = e;
	}
}

void rendering_system::build_renderables_queue()
{
	//init
	m_renderables.m_cull_light = nullptr;
	m_renderables.m_cull_opaque = nullptr;
	m_renderables.m_cull_translucent = nullptr;
    //camera
    camera::ptr   c_camera = m_camera->get_component<camera>();
    transform_ptr t_camera = m_camera->get_component<transform>();
    auto&         f_camera = c_camera->get_frustum();
    //update view frustum
    if(m_update_frustum)
        c_camera->get_frustum().update_frustum(c_camera->get_projection()*
                                               t_camera->get_matrix_inv());
    //build queue lights
    for (render_queues::element& weak_element : m_renderables.m_lights)
    {
        auto entity     = weak_element.lock();
        auto t_entity   = entity->get_component<transform>();
        auto l_entity   = entity->get_component<light>();
		const auto l_pos    = (glm::vec3)(t_entity->get_matrix()*glm::vec4(0, 0, 0, 1.));
		const auto l_radius	= l_entity->m_quadratic;
        if( l_entity->is_enabled() && f_camera.test_sphere(l_pos, l_radius))
        {
			weak_element.m_next = nullptr;
			m_renderables.add_call_light(&weak_element);
        }
    }
    //build queue opaque
    for (render_queues::element& weak_element : m_renderables.m_opaque)
    {
        auto entity     = weak_element.lock();
        auto t_entity   = entity->get_component<transform>();
        auto r_entity   = entity->get_component<renderable>();
        
        if(   r_entity->is_enabled() && (
                                         ! r_entity->has_support_culling()
                                         ||f_camera.test_obb(r_entity->get_bounding_box(), t_entity->get_matrix())
                                         ))
        {
			weak_element.m_next = nullptr;
			weak_element.m_depth = compute_camera_depth(f_camera, t_entity);
			m_renderables.add_call_opaque(&weak_element);
        }
    }
    //build queue translucent
	for (render_queues::element& weak_element : m_renderables.m_translucent)
    {
        auto entity     = weak_element.lock();
        auto t_entity   = entity->get_component<transform>();
        auto r_entity   = entity->get_component<renderable>();
        
        if(   r_entity->is_enabled() && (
                                         !r_entity->has_support_culling()
                                         || f_camera.test_obb(r_entity->get_bounding_box(), t_entity->get_matrix())
                                         ))
        {
			weak_element.m_next = nullptr;
			weak_element.m_depth = compute_camera_depth(f_camera, t_entity);
			m_renderables.add_call_translucent(&weak_element);
        }
    }
}

void rendering_system::draw()
{
    //culling
    if(!m_stop_frustum_culling) build_renderables_queue();
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

const glm::vec4& rendering_system::get_clear_color() const
{
    return m_clear_color;
}

const glm::vec4& rendering_system::get_ambient_color() const
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
