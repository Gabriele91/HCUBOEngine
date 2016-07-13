//
//  rendering_system.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright � 2016 Gabriele. All rights reserved.
//
#include <rendering_system.h>

void rendering_system::set_camera(camera::ptr cam)
{
	m_camera = cam;
}

void rendering_system::add_entity(entity::ptr e)
{
	m_entities.push_back(e);
}

void rendering_system::add_rendering_pass(rendering_pass_ptr pass)
{
	m_rendering_pass.push_back(pass);
}


void rendering_pass_base::draw_pass(camera::ptr camera, std::vector< entity::ptr >& entities)
{
    const glm::vec4& viewport = camera->get_viewport();
    glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    
	for (entity::ptr& entity : entities)
	{
		entity->m_material->bind(camera.get(), entity->m_model);
		entity->m_renderable->draw();
		entity->m_material->unbind();
	}
}

void rendering_system::draw()
{
	for (rendering_pass_ptr& pass : m_rendering_pass) pass->pre_pass (m_camera, m_entities);
	for (rendering_pass_ptr& pass : m_rendering_pass) pass->draw_pass(m_camera, m_entities);
	for (rendering_pass_ptr& pass : m_rendering_pass) pass->post_pass(m_camera, m_entities);
}

camera::ptr rendering_system::get_camera() const
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
