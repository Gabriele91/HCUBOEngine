//
//  system_manager.cpp
//  HCubo
//
//  Created by Gabriele on 24/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <system_manager.h>


void system_manager::add_system(system_component_ptr system)
{
    //add
    m_systems.push_back(system);
    //call event
    system->on_attach(*this);
}

void system_manager::remove_system(system_component_ptr system)
{
    //search
    auto it_system = std::find(m_systems.begin(), m_systems.end(), system);
    //remove
    if(it_system!=m_systems.end())
    {
        //call event
        (*it_system)->on_detach();
        //remove
        m_systems.erase(it_system);
    }
}

void system_manager::add_entity(entity::ptr entity)
{
    //add
    m_entities.push_back(entity);
    //call event system
    for(auto sys : m_systems) sys->on_add_entity(entity);
    //call event entity
    entity->on_attach(*this);
}

void system_manager::remove_entity(entity::ptr entity)
{
    //search
    auto it_entity = std::find(m_entities.begin(), m_entities.end(), entity);
    //remove
    if(it_entity!=m_entities.end())
    {
        //call event entity
        entity->on_detach();
        //call event system
        for(auto sys : m_systems) sys->on_remove_entity(entity);
        //remove
        m_entities.erase(it_entity);
    }
}

void system_manager::update(double deltatime)
{
    for(auto sys : m_systems) sys->on_update(deltatime);
}

const system_manager::list_entities& system_manager::get_entities() const
{
    return m_entities;
}

const system_manager::list_systems& system_manager::get_systems() const
{
    return m_systems;
}