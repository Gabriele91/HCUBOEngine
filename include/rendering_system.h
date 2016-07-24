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
#include <smart_pointers.h>
#include <system_manager.h>

class rendering_pass
{
public:
    virtual void draw_pass(glm::vec4&  clear_color,
                           glm::vec4&  ambient_color,
                           entity::ptr e_camera,
                           std::vector< entity::wptr >& lights,
                           std::vector< entity::wptr >& renderables)= 0;
};
using rendering_pass_ptr  = std::shared_ptr< rendering_pass >;
using rendering_pass_uptr = std::unique_ptr< rendering_pass >;

class rendering_pass_base : public rendering_pass, public smart_pointers< rendering_pass_base >
{
public:
    virtual void draw_pass(glm::vec4&  clear_color,
                           glm::vec4&  ambient_color,
                           entity::ptr e_camera,
                           std::vector< entity::wptr >& lights,
                           std::vector< entity::wptr >& renderables);
};

class rendering_system : public system_component, public smart_pointers< rendering_system >
{
public:
    
    
    virtual void on_attach( system_manager& );
    
    virtual void on_detach();
    
    virtual void on_add_entity(entity::ptr);
    
    virtual void on_remove_entity(entity::ptr);
    
    virtual void on_update(double deltatime);
    
    void set_clear_color(const glm::vec4& clear_color);
    
    void set_ambient_color(const glm::vec4& ambient_color);

	void add_rendering_pass(rendering_pass_ptr pass);

	void draw();
    
    const glm::vec4& get_clear_color() const;
    
    const glm::vec4& get_ambient_color() const;
    
	entity::ptr get_camera() const;

	const std::vector< rendering_pass_ptr >& get_rendering_pass() const;
        
protected:
    
    glm::vec4                         m_clear_color;
    glm::vec4                         m_ambient_color;
    entity::ptr						  m_camera;
    std::vector< entity::wptr >       m_lights;
    std::vector< entity::wptr >       m_renderables;
	std::vector< rendering_pass_ptr > m_rendering_pass;
    
};