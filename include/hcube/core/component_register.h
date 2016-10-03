#pragma once
#include <unordered_map>
#include <hcube/config.h>
#include <hcube/core/component.h>

namespace hcube
{
	namespace component_register
	{
		using make_component_func = component_ptr(*)();
		using component_registry  = std::unordered_map<std::string, make_component_func>;
		
		/* static */
		HCUBE_API component_registry& get_component_registry();
		HCUBE_API component_ptr create(const std::string& name);

		class HCUBE_API add_to_registry
		{
			bool m_success{ true };

		public:

			add_to_registry(const std::string& name, make_component_func func);
			bool get_success() const;

		protected:
			// C++11 (DLL)
			add_to_registry(const add_to_registry&)		       = delete;
			add_to_registry& operator=(const add_to_registry&) = delete;
		};

		template <class T>
		component_ptr make_component()
		{
			return std::make_shared<T>();
		}

		template <class T>
		class add_T_to_registry : public add_to_registry
		{
		public:

			static add_T_to_registry<T>& instance(const std::string& name)
			{
				static add_T_to_registry<T> s_instance(name);
				return s_instance;
			}

		protected:

			add_T_to_registry(const std::string& name)
			:add_to_registry(name, make_component<T>)
			{
			}
			// C++11 (DLL)
			add_T_to_registry<T>(const add_T_to_registry<T>&)            = delete;
			add_T_to_registry<T>& operator=(const add_T_to_registry<T>&) = delete;
		};
	}
}

#define HCUBE_COMPONENT_REGISTER(T)\
	namespace component_register{ namespace detail {\
		static const ::hcube::component_register::add_T_to_registry<T>&  T##_##component_registered =\
		::hcube::component_register::add_T_to_registry<T>::instance(#T);\
	}}
