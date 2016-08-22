#pragma once
#include <smart_pointers.h>
#include <resource.h>
#include <entity.h>

namespace hcube
{
	//class dec
	class prefab;
	//ptrs dec
	using prefab_ptr = std::shared_ptr< prefab >;
	using prefab_uptr = std::unique_ptr< prefab >;
	using prefab_wptr = std::weak_ptr  < prefab >;
	//class defined
	class prefab : public resource
	{
	public:
		virtual entity::ptr instantiate() = 0;
		virtual prefab_ptr copy() const = 0;
	};
}