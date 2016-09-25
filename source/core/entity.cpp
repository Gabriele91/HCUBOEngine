//
//  entity.cpp
//  HCubo
//
//  Created by Gabriele on 23/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//

#include <hcube/core/entity.h>
#include <hcube/core/system_manager.h>

namespace hcube
{
	//default
	entity::entity() {}

	//component list
	entity::entity(std::initializer_list<component_ptr> components)
	{
		for (auto& c : components)
		{
			add_component(c);
		}
	}

	component_ptr entity::add_component(component_ptr component_t)
	{
		//cache
		if (component_t->get_id() == transform::type())	transform_cache = component_t;
		//add
		component_t->m_entity = this;
		m_components[component_t->get_id()] = component_t;
		component_t->on_attach(*this);
		return component_t;
	}

	component_ptr entity::remove_component(component_id id)
	{
		if (has_component(id))
		{
			//cache
			if (id == transform::type()) transform_cache = nullptr;
			//find
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

	system_manager* entity::get_systems() const
	{
		return m_systems;
	}

	bool entity::has_child(entity::ptr entity) const
	{
		return m_entities.find(entity.get()) != m_entities.end();
	}

	void entity::add_child(entity::ptr new_child)
	{
		if (!has_child(new_child))
		{
			//ref to this
			new_child->m_parent = this;
			//insert
			m_entities[new_child.get()] = new_child;
			//add to sys
			if (get_systems()) get_systems()->add_entity(new_child);
		}
	}

	const entity::entities_map& entity::get_childs() const
	{
		return m_entities;
	}

	void entity::remove_child(const entity::ptr old_child)
	{
		if (has_child(old_child))
		{
			//ref to this
			old_child->m_parent = nullptr;
			//remove
			m_entities.erase(m_entities.find(old_child.get()));
			//remove from sys
			if (get_systems()) get_systems()->add_entity(old_child);
		}
	}

	bool entity::on_update(double deltatime)
	{
		//update status
		bool state = true;
		//all component
		for (auto it_component : m_components)
		{
			state &= it_component.second->on_update(deltatime);
		}
		//update childs
		for (auto it_entity : m_entities)
		{
			state &= it_entity.second->on_update(deltatime);
		}
		//return state
		return state;
	}

	const std::string& entity::get_name() const
	{
		return m_name;
	}

	void entity::set_name(const std::string& name)
	{
		m_name = name;
	}

	std::vector < entity::ptr > entity::get_childs_by_name(const std::string& name)
	{
		std::vector < entity::ptr > m_select_es;
		for (auto it_entity : m_entities)
		{
			if (it_entity.second->get_name() == name) m_select_es.push_back(it_entity.second);
		}
		return m_select_es;
	}

	//system events
	void entity::on_attach(system_manager& sys)
	{
		//attach
		m_systems = &sys;
		//add child
		for (auto child : m_entities) sys.add_entity(child.second);
	}

	void entity::on_detach()
	{
		//remove childs
		for (auto child : m_entities) m_systems->remove_entity(child.second);
		//remove
		m_systems = nullptr;
	}

	void entity::send_message_to_components(const message& message)
	{
		for (auto it_component : m_components)
		{
			it_component.second->on_message(message);
		}
	}

	void entity::send_message_to_components_upwards(const message& message, bool to_this_entity)
	{
		if(to_this_entity) send_message_to_components(message);

		if (m_parent)
		{
			m_parent->send_message_to_components_upwards(message,true);
		}
	}

	void entity::send_message_to_components_downwards(const message& message, bool to_this_entity)
	{
		if (to_this_entity) send_message_to_components(message);

		for (auto it_entity : m_entities)
		{
			it_entity.second->send_message_to_components_downwards(message, true);
		}
	}

	void entity::send_message_to_component(component_id id, const message& message)
	{
		if (has_component(id)) m_components[id]->on_message(message);
	}

	void entity::send_message_to_component_upwards(component_id id, const message& message, bool to_this_entity)
	{
		if (to_this_entity) send_message_to_component(id, message);

		if (m_parent)
		{
			m_parent->send_message_to_component_upwards(id, message, true);
		}
	}

	void entity::send_message_to_component_downwards(component_id id, const message& message, bool to_this_entity)
	{
		if (to_this_entity) send_message_to_component(id, message);

		for (auto it_entity : m_entities)
		{
			it_entity.second->send_message_to_component_downwards(id, message, true);
		}
	}

	//copy
	entity::ptr entity::copy() const
	{
		auto oentity = entity::snew();
		//copy all component
		for (auto it_component : m_components)
		{
			auto cpycomponent = it_component.second->copy();
			oentity->add_component(cpycomponent);
		}
		//copy childs
		for (auto it_entity : m_entities)
		{
			auto cpyentity = it_entity.second->copy();
			oentity->add_child(cpyentity);
		}
		return oentity;
	}
}