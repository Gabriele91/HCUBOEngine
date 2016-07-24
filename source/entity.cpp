//
//  entity.cpp
//  HCubo
//
//  Created by Gabriele on 23/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//

#include <entity.h>
#include <system_manager.h>

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
    component_t->m_entity = this;
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
        it->second->m_entity = nullptr;
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

entity* entity::get_parent() const
{
    return m_parent;
}

system_manager* entity::get_system() const
{
    return m_system;
}

bool entity::has_child(entity::ptr entity) const
{
    return m_entities.find(entity.get()) != m_entities.end();
}

void entity::add_child(entity::ptr new_child)
{
    if(!has_child(new_child))
    {
        //ref to this
        new_child->m_parent = this;
        //insert
        m_entities[new_child.get()] = new_child;
        //add to sys
        if(get_system()) get_system()->add_entity(new_child);
    }
}

void entity::remove_child(const entity::ptr old_child)
{
    if(has_child(old_child))
    {
        //ref to this
        old_child->m_parent = nullptr;
        //remove
        m_entities.erase(m_entities.find(old_child.get()));
        //remove from sys
        if(get_system()) get_system()->add_entity(old_child);
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

//system events
void entity::on_attach( system_manager& sys )
{
    //attach
    m_system = &sys;
    //add child
    for(auto child:m_entities) sys.add_entity(child.second);
}

void entity::on_detach()
{
    //remove childs
    for(auto child:m_entities) m_system->remove_entity(child.second);
    //remove
    m_system = nullptr;
}

void entity::send_message_to_component(const message& message)
{
    for(auto it_component : m_components)
    {
        it_component.second->on_message(message);
    }
}

void entity::send_message_to_component_upwards(const message& message)
{
    send_message_to_component(message);
    
    if(m_parent)
    {
        m_parent->send_message_to_component_upwards(message);
    }
}

void entity::send_message_to_component_downwards(const message& message)
{
    send_message_to_component(message);
    
    for(auto it_entity : m_entities)
    {
        it_entity.second->send_message_to_component_downwards(message);
    }
}

void entity::send_message_to_component(component_id id, const message& message)
{
    if(has_component(id)) m_components[id]->on_message(message);
}

void entity::send_message_to_component_upwards(component_id id,const message& message)
{
    send_message_to_component_upwards(id,message);
    
    if(m_parent)
    {
        m_parent->send_message_to_component_upwards(id,message);
    }
}

void entity::send_message_to_component_downwards(component_id id,const message& message)
{
    send_message_to_component(id,message);
    
    for(auto it_entity : m_entities)
    {
        it_entity.second->send_message_to_component_downwards(id,message);
    }
}