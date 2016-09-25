//
//  entity.h
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <string>
#include <vector>
#include <type_traits>
#include <unordered_map>
#include <hcube/core/smart_pointers.h>
#include <hcube/core/component.h>
//components cache
#include <hcube/components/transform.h>

namespace hcube
{
	//owanner
	class system_manager;
	//entity
	class entity : public smart_pointers< entity >
	{

		friend class system_manager;

	public:

		using components_map = std::unordered_map< component_id, component_ptr >;
		using entities_map = std::unordered_map< entity*, entity::ptr >;

		//default
		entity();

		//component list
		entity(std::initializer_list<component_ptr> components);

		//auto add component list
		template < typename ...Args >
		entity(component_ptr component, Args&& ... args) : entity(args...)
		{
			add_component(component);
		}

		template < class T >
		T& add_component()
		{
			static_assert(std::is_base_of<component, T>::value, "Must to be a component");
			//return add_component(new T());
			return add_component(std::make_shared<T>());
		}
		component_ptr add_component(component_ptr component_t);

		template < class T >
		inline std::shared_ptr< T > get_component()
		{
			static_assert(std::is_base_of<component, T>::value, "Must to be a component");
			//cache
			if (T::get_class_id() == transform::get_class_id()) return  std::static_pointer_cast<T>(transform_cache);
			//pool
			return std::static_pointer_cast<T>(m_components[T::get_class_id()]);
		}

		inline component_ptr get_component(component_id id)
		{
			//cache
			if (id == transform::get_class_id()) return  transform_cache;
			//pool
			return m_components[id];
		}

		template < class T >
		std::shared_ptr< T > remove_component()
		{
			static_assert(std::is_base_of<component, T>::value, "Must to be a component");
			return std::static_pointer_cast<T>(remove_component(T::get_class_id()));
		}
		component_ptr remove_component(component_id id);

		template < class T >
		bool has_component() const
		{
			static_assert(std::is_base_of<component, T>::value, "Must to be a component");
			return has_component(T::get_class_id());
		}
		bool has_component(component_id id);

		bool has_component(component_id id) const;

		entity* get_parent() const;

		system_manager* get_systems() const;

		bool has_child(entity::ptr entity) const;

		void add_child(entity::ptr entity);

		const entities_map& get_childs() const;

		void remove_child(const entity::ptr entity);

		const std::string& get_name() const;

		void set_name(const std::string& name);

		std::vector < entity::ptr > get_childs_by_name(const std::string& name);

		bool on_update(double deltatime);

		void send_message_to_components(const message& message);

		void send_message_to_components_upwards(const message& message,bool to_this_entity=false);

		void send_message_to_components_downwards(const message& message, bool to_this_entity = false);

		void send_message_to_component(component_id id, const message& message);

		void send_message_to_component_upwards(component_id id, const message& message, bool to_this_entity = false);

		void send_message_to_component_downwards(component_id id, const message& message, bool to_this_entity = false);

		entity::ptr copy() const;

	protected:
		//system events
		virtual void on_attach(system_manager&);
		virtual void on_detach();
		//parent
		system_manager* m_systems{ nullptr };
		entity* m_parent{ nullptr };
		//entity name
		std::string m_name;
		//list components
		std::unordered_map< component_id, component_ptr > m_components;
		std::unordered_map< entity*, entity::ptr > m_entities;
		//components cache
		component_ptr transform_cache;
	};
	
}