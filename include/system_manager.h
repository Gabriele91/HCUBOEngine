//
//  system_manager.h
//  HCubo
//
//  Created by Gabriele on 24/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <entity.h>
#include <smart_pointers.h>
class system_manager;
class system_component;


class system_component
{
public:
    virtual void on_attach( system_manager& ){ }
    virtual void on_detach(){ }
    virtual void on_add_entity(entity::ptr) = 0;
    virtual void on_remove_entity(entity::ptr) = 0;
    virtual void on_update(double deltatime) = 0;
};

using system_component_ptr  = std::shared_ptr< system_component >;
using system_component_uptr = std::unique_ptr< system_component >;
using system_component_wptr = std::weak_ptr< system_component >;

class system_manager
{
public:
    
    using list_entities = std::vector< entity::ptr >;
    using list_systems  = std::vector< system_component_ptr >;
    
    void add_system(system_component_ptr);
    void remove_system(system_component_ptr);
    
    void add_entity(entity::ptr);
    void remove_entity(entity::ptr);
    
    void update(double deltatime);
    
    const list_entities& get_entities() const;
    const list_systems& get_systems() const;
    
protected:
    
    std::vector< entity::ptr >           m_entities;
    std::vector< system_component_ptr >  m_systems;
    
};