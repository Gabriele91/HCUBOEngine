#pragma once
#include <tuple>
#include <hcube/data/json.h>
#include <hcube/data/variant.h>

namespace hcube
{

	struct property
	{
		virtual void		set(void* obj, variant_ref value) = 0;
		virtual variant_ref get(void* obj)					  = 0;
		virtual	const char* get_name() const			      = 0;
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

}