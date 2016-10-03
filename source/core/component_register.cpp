#include <hcube/core/component_register.h>

namespace hcube
{
	namespace component_register
	{

		component_registry& get_component_registry()
		{
			static component_registry instance;
			return instance;
		}

		component_ptr create(const std::string& name)
		{
			//get dict
			auto& registry = get_component_registry();
			//find
			auto element = registry.find(name);
			//test
			if (element == registry.end()) return{ nullptr };
			//call
			return (element->second)();
		}

		add_to_registry::add_to_registry(const std::string& name, make_component_func func)
		{
			auto ret = get_component_registry().insert({ name,func });
			m_success = ret.second != false;
		}

		bool add_to_registry::get_success() const
		{
			return m_success;
		}
	}
}