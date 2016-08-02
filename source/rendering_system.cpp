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

void rendering_system::on_attach( system_manager& sm )
{
    for(auto& e : sm.get_entities()) on_add_entity(e);
}

void rendering_system::on_detach()
{
    m_camera = nullptr;
    m_lights.clear();
    m_renderables.clear();
}

void rendering_system::on_add_entity(entity::ptr e)
{
    //copy ref
    if(e->has_component<camera>())     m_camera = e;
    if(e->has_component<light>())      m_lights.push_back(e);
    if(e->has_component<renderable>()) m_renderables.push_back(e);
}

void rendering_system::on_remove_entity(entity::ptr e)
{
    //remove camera
    if(m_camera == e) m_camera = nullptr;
    //search light and remove
    for(auto it_light  = m_lights.begin();
             it_light != m_lights.end();
           ++it_light)
    {
        if(it_light->lock() == e)
        {
            m_lights.erase(it_light);
            break;
        }
    }
    //search renderable and remove
    for(auto it_renderable  = m_lights.begin();
             it_renderable != m_lights.end();
           ++it_renderable)
    {
        if(it_renderable->lock() == e)
        {
            m_renderables.erase(it_renderable);
            break;
        }
    }
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
                                    std::vector< entity::wptr >& lights,
                                    std::vector< entity::wptr >& renderables)
{
    camera::ptr   ccamera = e_camera->get_component<camera>();
    transform_ptr tcamera = e_camera->get_component<transform>();
    const glm::vec4& viewport = ccamera->get_viewport();
    glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
	for (entity::wptr& weak_entity : renderables)
	{
        auto entity   = weak_entity.lock();
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

void rendering_system::draw()
{
    for (rendering_pass_ptr& pass : m_rendering_pass)
    {
        pass->draw_pass
        (
            m_clear_color,
            m_ambient_color,
            m_camera,
            m_lights,
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
