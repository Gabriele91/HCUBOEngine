//
//  rendering_system.h
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <camera.h>
#include <entity.h>
#include <smart_pointers.h>

class rendering_pass
{
public:
	virtual void draw_pass(camera::ptr camera, std::vector< entity::ptr >& entities)= 0;
};
using rendering_pass_ptr  = std::shared_ptr< rendering_pass >;
using rendering_pass_uptr = std::unique_ptr< rendering_pass >;

class rendering_pass_base : public rendering_pass, public smart_pointers< rendering_pass_base >
{
public:
	virtual void draw_pass(camera::ptr camera, std::vector< entity::ptr >& entities);
};

class rendering_system
{
public:
    
	void set_camera(camera::ptr cam);

	void add_entity(entity::ptr e);

	void add_rendering_pass(rendering_pass_ptr pass);

	void draw();
    
	camera::ptr get_camera() const;

	const std::vector< entity::ptr >& get_entities() const;

	const std::vector< rendering_pass_ptr >& get_rendering_pass() const;
        
protected:
    
    camera::ptr						  m_camera;
    std::vector< entity::ptr >	      m_entities;
	std::vector< rendering_pass_ptr > m_rendering_pass;
    
};