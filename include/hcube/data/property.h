#pragma once
#include <tuple>
#include <vector>
#include <functional>
#include <hcube/data/variant.h>
#include <hcube/data/parser/utils_parser.h>

namespace hcube
{

	struct property
	{
		virtual void		set(void* obj, variant_ref value) = 0;
		virtual variant_ref get(void* obj)					  = 0;
		virtual	const char* get_name() const			      = 0;
	};

	struct runtime_property : public property
	{
		using function_set = std::function < void(void* obj, variant_ref value) >;
		using function_get = std::function < variant_ref(void* obj) >;

		std::string	  m_name;
		function_set  m_set;
		function_get  m_get;

		runtime_property(function_get get,
						 function_set set,
						 const char* name)
			: m_get{ get }
			, m_set{ set }
			, m_name{ name }
		{
		}

		virtual void set(void* obj, variant_ref value) override
		{
			m_set(obj,value);
		}

		virtual variant_ref get(void* obj) override
		{
			return m_get(obj);
		}

		virtual	const char* get_name() const override
		{
			return m_name.c_str();
		}
	};

	template<typename CLASS, typename T, typename I>
	struct property_set_get : public property
	{
		//type
		typedef T    (CLASS::*get_type) ();
		typedef void (CLASS::*set_type) (I);
		using return_type   = typename T;
		using argument_type = typename I;
		//property
	    property_set_get(get_type get,
						 set_type set,
						 const char* name)
			: m_get{ get }
			, m_set{ set }
			, m_name{ name }
		{
		}

		virtual void set(void* obj, variant_ref value)
		{
			(((CLASS*)obj)->*m_set)( value.get< std::remove_reference<argument_type>::type >() );
		}

		virtual variant_ref get(void* obj)
		{
			return variant_ref((((CLASS*)obj)->*m_get)());
		}

		virtual const char* get_name() const
		{
			return m_name;
		}

	protected:
		//attributes
		set_type	m_set;
		get_type	m_get;
		const char* m_name;
	};

	template<typename CLASS, typename T>
	struct property_member : public property
	{
		//type
		using type = T;
		//property
		constexpr property_member(T CLASS::*member,  const char* name)
		: m_member{ member }
		, m_name{ name }
		{
		}	

		virtual void set(void* obj, variant_ref value)
		{
			((CLASS*)obj)->*m_member = value.get<T>();
		}
		virtual variant_ref get(void* obj)
		{
			return variant_ref(((CLASS*)obj)->*m_member);
		}

		virtual const char* get_name() const
		{
			return m_name;
		}

	protected:
		//attributes
		T CLASS::*	m_member;
		const char* m_name;
	};

	inline property* make_runtime_property(runtime_property::function_get get, const char* name)
	{
		return new runtime_property(get, [](void*, variant_ref) {}, name);
	}

	inline property* make_runtime_property(runtime_property::function_get get, runtime_property::function_set set, const char* name)
	{
		return new runtime_property(get, set, name);
	}

	template<typename CLASS, typename T>
	property* make_property_member(T CLASS::*member, const char* name)
	{
		static property_member<CLASS, T> obj{member, name};
		return &obj;
	}

	template<typename CLASS, typename T, typename I>
	property* make_property_get_set(T(CLASS::*get)(void), void(CLASS::*set)(I),  const char* name)
	{
		static property_set_get<CLASS, T, I> obj{get, set, name};
		return &obj;
	}

	inline bool parser_properties(const char* ptr, void* obj, const std::vector< property* >& properties)
	{
		size_t line = 0;
		variant value;
		std::string name;
		while (*ptr)
		{
			//skeep
			parser::utils_parser::skeep_space_end_comment(line, ptr);
			//name
			if (!parser::utils_parser::parse_name(ptr, &ptr, name)) return false;
			//skeep
			parser::utils_parser::skeep_space_end_comment(line, ptr);
			//parse
			if (!parser::utils_parser::parse_variant(line, ptr, value)) return false;
			//search
			for (property* p : properties)
			{
				if (p->get_name() == name)
				{
					p->set(obj, value);
					break;
				}
			}
		}
		//success
		return true;
	}

	template<typename T>
	inline bool parser_properties(const char* ptr, T& obj)
	{
		return parser_properties(ptr,(void*)&obj,T::properties());
	}
}