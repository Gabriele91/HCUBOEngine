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


void rendering_system::set_clear_color(const glm::vec4& clear_color)
{
    m_clear_color = clear_color;
}

void rendering_system::set_ambient_color(const glm::vec4& ambient_color)
{
    m_ambient_color = ambient_color;
}

void rendering_system::set_camera(entity::ptr cam)
{
	m_camera = cam;
}

void rendering_system::add_entity(entity::ptr e)
{
    //add entity
	m_entities.push_back(e);
    //add into queues
    if(e->has_component<light>())      m_lights.push_back(e);
    if(e->has_component<renderable>()) m_renderables.push_back(e);
}

void rendering_system::add_rendering_pass(rendering_pass_ptr pass)
{
	m_rendering_pass.push_back(pass);
}


void rendering_pass_base::draw_pass(glm::vec4&  clear_color,
                                    glm::vec4&  ambient_color,
                                    entity::ptr e_camera,
                                    std::vector< entity::wptr >& lights,
                                    std::vector< entity::wptr >& renderables,
                                    std::vector< entity::ptr >& entities)
{
    camera::ptr   ccamera = e_camera->get_component<camera>();
    transform_ptr tcamera = e_camera->get_component<transform>();
    const glm::vec4& viewport = ccamera->get_viewport();
    glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
	for (entity::wptr& weak_entity : renderables)
	{
        auto entity = weak_entity.lock();
		entity->get_component<renderable>()->draw(viewport,
                                                  ccamera->get_projection(),
                                                  tcamera->get_matrix_inv(),
                                                  entity->get_component<transform>()->get_matrix(),
                                                  entity->get_component<material>());
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
            m_renderables,
            m_entities
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

const std::vector< entity::ptr >& rendering_system::get_entities() const
{
	return m_entities;
}

const std::vector< rendering_pass_ptr >& rendering_system::get_rendering_pass() const
{
	return m_rendering_pass;
}
