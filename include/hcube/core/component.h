//
//  component.h
//  HCubo
//
//  Created by Gabriele on 23/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <typeindex>
#include <hcube/core/smart_pointers.h>

namespace hcube
{

	//class
	class  entity;
	class  component;
	struct message;
	//type
	using message_id = unsigned;
	using component_id = std::type_index;
	using entity_id = unsigned;

	struct message
	{
		message_id m_id{ 0 };
		void*      m_data{ nullptr };

		//default
		message() { }

		//message init
		message(message_id id, void* data = nullptr)
		{
			m_id = id;
			m_data = data;
		}
	};

#define HCUBE_COMPONENT_DEC(T)\
    public: static const char* get_class_name(){ return #T ; }\
    public: static component_id get_class_id(){ static component_id type=typeid(T); return type; }\
    public: component_id get_component_id()   const { return get_class_id(); }\
    public: const char*  get_component_name() const { return get_class_name(); }\
    private:

	using component_ptr = std::shared_ptr< component >;
	using component_uptr = std::unique_ptr< component >;
	using component_wptr = std::weak_ptr  < component >;

	class component
	{

		friend class entity;

	public:

		component() {}
		virtual ~component() {}

		/* interface */
		virtual bool on_update(double deltaTime) { return true; }
		virtual void on_attach(entity&) { }
		virtual void on_detach() { }
		virtual void on_message(const message& message) { }
		virtual component_ptr copy() const = 0;

		/* methods */
		virtual bool on_activate() { return false; }
		virtual bool on_deactivate() { return false; }

		virtual component_id get_component_id() const = 0;
		virtual const char*  get_component_name() const = 0;

		bool is_active() const { return m_is_active; }
		bool is_enabled() const { return m_is_enabled; }

		void set_enabled(bool enabled) { m_is_enabled = enabled; }

		entity* get_entity() const { return m_entity; }


	private:

		entity* m_entity{ nullptr };
		bool m_is_active{ true };   //is active from the on_activate
		bool m_is_enabled{ true };  //is enabled, more for debug purpose or if we need to disable just one component

	};

}
