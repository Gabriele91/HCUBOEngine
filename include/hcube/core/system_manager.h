//
//  system_manager.h
//  HCubo
//
//  Created by Gabriele on 24/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <hcube/core/entity.h>
#include <hcube/core/smart_pointers.h>

namespace hcube
{
	class system_manager;
	class system_component;
	using system_component_id = std::type_index;

	class system_component
	{
	public:
		virtual void on_attach(system_manager&) { }
		virtual void on_detach() { }
		virtual void on_message(const message& message) { }
		virtual void on_add_entity(entity::ptr) { };
		virtual void on_remove_entity(entity::ptr) { };
		virtual void on_update(double deltatime) = 0;
		virtual system_component_id get_id() const = 0;
	};

	using system_component_ptr = std::shared_ptr< system_component >;
	using system_component_uptr = std::unique_ptr< system_component >;
	using system_component_wptr = std::weak_ptr< system_component >;

#define HCUBE_SYSTEM_COMPONENT_DEC(T)\
    public: static system_component_id type(){ static system_component_id type=typeid(T); return type; }\
    public: system_component_id get_id() const { return type(); }\
    private:

	class system_manager
	{
	public:

		using list_entities = std::vector< entity::ptr >;
		using map_systems = std::unordered_map< system_component_id, system_component_ptr >;

		void add_system(system_component_ptr);

		void remove_system(system_component_ptr);

		system_component* get_system(system_component_id id) const;

		template< class T >
		T* get_system()
		{
			return (T*)get_system(T::type());
		}

		void send_message_to_systems(const message& message);

		void send_message_to_system(system_component_id id, const message& message);

		void add_entity(entity::ptr);

		void remove_entity(entity::ptr);

		void send_message_to_entities(const message& message);

		void update(double deltatime);

		std::vector < entity::ptr > get_entities_by_name(const std::string& name);

		const list_entities& get_entities() const;

		const map_systems& get_systems() const;

	protected:

		list_entities m_entities;
		map_systems   m_systems;

	};
}