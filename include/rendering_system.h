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

class render_queues
{
public:
    
    using queue = std::vector < entity::wptr >;
    
	queue m_lights;
	queue m_opaque;
	queue m_translucent;

	void clear();
	void push(entity::ptr e);
    void remove(entity::ptr e);
    void reserve(size_t size);
};

class rendering_pass
{
public:
    virtual void draw_pass(glm::vec4&  clear_color,
                           glm::vec4&  ambient_color,
                           entity::ptr e_camera,
                           render_queues& queues)= 0;
};
using rendering_pass_ptr  = std::shared_ptr< rendering_pass >;
using rendering_pass_uptr = std::unique_ptr< rendering_pass >;

class rendering_pass_base : public rendering_pass, public smart_pointers< rendering_pass_base >
{
public:
    virtual void draw_pass(glm::vec4&  clear_color,
                           glm::vec4&  ambient_color,
                           entity::ptr e_camera,
						   render_queues& queues);
};


class rendering_system : public system_component, public smart_pointers< rendering_system >
{

	SYSTEM_COMPONENT_DEC(rendering_system);

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
    
    void stop_update_frustum(bool stop_update);
    
    void stop_frustum_culling(bool stop_culling);
        
protected:
    
    void build_renderables_queue();
    
    bool m_update_frustum{ true };
    bool m_stop_frustum_culling{ false };

    glm::vec4                         m_clear_color;
    glm::vec4                         m_ambient_color;
    entity::ptr						  m_camera;
    render_queues					  m_renderables;
    render_queues					  m_queue_renderables;
	std::vector< rendering_pass_ptr > m_rendering_pass;
    
};