//
//  entity.cpp
//  HCubo
//
//  Created by Gabriele on 23/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//

#include <entity.h>

//default
entity::entity(){}

//component list
entity::entity( std::initializer_list<component_ptr> components )
{
    for(auto& c : components)
    {
        add_component(c);
    }
}

component_ptr entity::add_component(component_ptr component_t)
{
    component_t->on_attach(*this);
    m_components[component_t->get_id()] = component_t;
    return component_t;
}


component_ptr entity::get_component(component_id id)
{
    return m_components[id];
}

component_ptr entity::remove_component(component_id id)
{
    if(has_component(id))
    {
        auto it = m_components.find(id);
        it->second->on_detach();
        return m_components.erase(it)->second;
    }
    return nullptr;
}

bool entity::has_component(component_id id)
{
    return m_components.find(id) != m_components.end();
}


bool entity::has_component(component_id id) const
{
    return m_components.find(id) != m_components.end();
}

entity* entity::parent() const
{
    return m_parent;
}

bool entity::has_child(entity::ptr entity) const
{
    return m_entities.find(entity.get()) != m_entities.end();
}

void entity::add_child(entity::ptr entity)
{
    if(!has_child(entity))
    {
        //ref to this
        entity->m_parent = this;
        //insert
        m_entities[entity.get()]=entity;
    }
}

void entity::remove_child(const entity::ptr entity)
{
    if(has_child(entity))
    {
        //ref to this
        entity->m_parent = nullptr;
        //remove
        m_entities.erase(m_entities.find(entity.get()));
    }
}

bool entity::on_update(double deltatime)
{
    //update status
    bool state = true;
    //all component
    for(auto it_component : m_components)
    {
        state &= it_component.second->on_update(deltatime);
    }
    //update childs
    for(auto it_entity : m_entities)
    {
        state &= it_entity.second->on_update(deltatime);
    }
    //return state
    return state;
}

void entity::send_message_to_component(const message *message)
{
    for(auto it_component : m_components)
    {
        it_component.second->on_message(message);
    }
}

void entity::send_message_to_component_all(const message *message)
{
    send_message_to_component(message);
    
    for(auto it_entity : m_entities)
    {
        it_entity.second->send_message_to_component_all(message);
    }
}