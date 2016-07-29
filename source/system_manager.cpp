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
    m_systems[system->get_id()] = system;
    //call event
    system->on_attach(*this);
}

void system_manager::remove_system(system_component_ptr system)
{
    //search
    auto it_system = m_systems.find(system->get_id());
    //remove
    if(it_system!=m_systems.end())
    {
        //call event
        it_system->second->on_detach();
        //remove
        m_systems.erase(it_system);
    }
}

system_component* system_manager::get_system(system_component_id id) const
{
	auto sys = m_systems.find(id);
	if (sys != m_systems.end()) return sys->second.get();
	return nullptr;
}

void system_manager::send_message_to_systems(const message& message)
{
	for (auto sys : m_systems) sys.second->on_message(message);
}

void system_manager::send_message_to_system(system_component_id id, const message& message)
{
	auto sys = m_systems.find(id);
	if (sys != m_systems.end()) sys->second->on_message(message);
}

void system_manager::add_entity(entity::ptr entity)
{
    //add
    m_entities.push_back(entity);
    //call event system
    for(auto sys : m_systems) sys.second->on_add_entity(entity);
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
        for(auto sys : m_systems) sys.second->on_remove_entity(entity);
        //remove
        m_entities.erase(it_entity);
    }
}

void system_manager::send_message_to_entities(const message& message)
{
	for (auto entity : m_entities) entity->send_message_to_components(message);
}

void system_manager::update(double deltatime)
{
    for(auto sys : m_systems) sys.second->on_update(deltatime);
}

std::vector < entity::ptr > system_manager::get_entities_by_name(const std::string& name)
{
	std::vector < entity::ptr > m_select_es;
	for (auto it_entity : m_entities)
	{
		if (it_entity->get_name() == name) m_select_es.push_back(it_entity);
	}
	return m_select_es;
}


const system_manager::list_entities& system_manager::get_entities() const
{
    return m_entities;
}

const system_manager::map_systems& system_manager::get_systems() const
{
    return m_systems;
}