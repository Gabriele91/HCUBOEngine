//
//  entity.h
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <unordered_map>
#include <material.h>
#include <glm/mat4x4.hpp>
#include <light.h>
#include <renderable.h>
#include <smart_pointers.h>
#include <component.h>
#include <type_traits>


class entity : public smart_pointers< entity >
{
public:
    //default
    entity(){}
    //component list
    entity( std::initializer_list<component_ptr> components )
    {
        for(auto& c : components)
        {
            add_component(c);
        }
    }
    //auto add component list
    template < typename ...Args >
    entity( component_ptr component, Args&& ... args ) : entity(args...)
    {
        add_component(component);
    }
    
    template < class T >
    T& add_component()
    {
        static_assert(std::is_base_of<component,T>::value,"Must to be a component");
        return add_component(new T());
    }
    
    component_ptr add_component(component_ptr component_t)
    {
        component_t->on_attach(*this);
        m_components[component_t->get_id()] = component_t;
        return component_t;
    }
    
    template < class T >
    std::shared_ptr< T > get_component()
    {
        static_assert(std::is_base_of<component,T>::value,"Must to be a component");
        return std::static_pointer_cast< T >( m_components[T::type()] );
    }
    
    component_ptr get_component(component_id id)
    {
        return m_components[id];
    }
    
    template < class T >
    std::shared_ptr< T > remove_component()
    {
        return std::static_pointer_cast<T>(remove_component(T::type()));
    }
    
    component_ptr remove_component(component_id id)
    {
        if(has_component(id))
        {
            auto it = m_components.find(id);
            it->second->on_detach();
            return m_components.erase(it)->second;
        }
        return nullptr;
    }
    
    bool has_component(component_id id)
    {
        return m_components.find(id) != m_components.end();
    }
    
    
    template < class T >
    bool has_component() const
    {
        return m_components.find(T::type()) != m_components.end();
    }
    
    bool has_component(component_id id) const
    {
        return m_components.find(id) != m_components.end();
    }
    
    entity* parent() const
    {
        return m_parent;
    }
    
    bool has_child(entity::ptr entity) const
    {
        return m_entities.find(entity.get()) != m_entities.end();
    }
    
    void add_child(entity::ptr entity)
    {
        if(!has_child(entity))
        {
            //ref to this
            entity->m_parent = this;
            //insert
            m_entities[entity.get()]=entity;
        }
    }
    
    void remove_child(const entity::ptr entity)
    {
        if(has_child(entity))
        {
            //ref to this
            entity->m_parent = nullptr;
            //remove
            m_entities.erase(m_entities.find(entity.get()));
        }
    }
    
    bool on_update(double deltatime)
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
    
    void send_message_to_component(const message *message)
    {
        for(auto it_component : m_components)
        {
            it_component.second->on_message(message);
        }
    }
    
    void send_message_to_component_all(const message *message)
    {
        send_message_to_component(message);
        
        for(auto it_entity : m_entities)
        {
            it_entity.second->send_message_to_component_all(message);
        }
    }
    
protected:
    
    //parent
    entity* m_parent;
    //list components
    std::unordered_map< component_id,component_ptr > m_components;
    std::unordered_map< entity*,entity::ptr > m_entities;
    

    
};
