#pragma once
#include <tuple>
#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <unordered_map>
#include <hcube/data/variant.h>

namespace hcube
{

	struct property
	{
		virtual void		set(void* obj, variant_ref value) = 0;
		virtual variant_ref get(void* obj)					  = 0;
		virtual variant     get_value(void* obj)			  = 0;
		virtual	const char* get_name() const			      = 0;
	};

	template<typename CLASS>
	struct property_function : public property
	{
		using function_set = typename std::function < void(CLASS* obj, variant_ref value) >;
		using function_get = typename std::function < variant_ref(CLASS* obj) >;

		std::string	  m_name;
		function_get  m_get;
		function_set  m_set;

		property_function(function_get get,
						  function_set set,
						  const char* name)
			: m_get{ get }
			, m_set{ set }
			, m_name{ name }
		{
		}

		virtual void set(void* obj, variant_ref value) override
		{
			m_set((CLASS*)obj,value);
		}

		virtual variant_ref get(void* obj) override
		{
			return m_get((CLASS*)obj);
		}

		virtual variant get_value(void* obj) override
		{
			return variant(m_get((CLASS*)obj));
		}

		virtual	const char* get_name() const override
		{
			return m_name.c_str();
		}
	};

	template<typename CLASS, typename ENUM>
	struct property_enum_str : public property
	{
		using map_str_to_enum   = std::unordered_map < std::string, ENUM >;
		using map_enum_to_str   = std::unordered_map < ENUM, std::string >;
		using map_argument_pair = std::pair< std::string, ENUM >;


		property_enum_str(
			ENUM CLASS::*member,
			std::initializer_list<map_argument_pair> enum_map,
			const char* name
		)
		: m_name{ name }
		, m_member{ member }
		{
			for (const map_argument_pair& enum_map_pair : enum_map)
			{
				m_str_to_enum[enum_map_pair.first] = enum_map_pair.second;
				m_enum_to_str[enum_map_pair.second] = enum_map_pair.first;
			}
		}

		virtual void set(void* obj, variant_ref value) override
		{
			//get iterator
			auto it = m_str_to_enum.find(value.get<std::string>());
			//set value from iterator
			if (it != m_str_to_enum.end())
			{
				((CLASS*)obj)->*m_member = it->second;
			}
		}

		virtual variant_ref get(void* obj) override
		{
			//get iterator
			auto it = m_enum_to_str.find(((CLASS*)obj)->*m_member);
			//set value from iterator
			if (it != m_enum_to_str.end())
			{
				return variant_ref(it->second);
			}
			return variant_ref();
		}

		virtual variant get_value(void* obj) override
		{
			//get iterator
			auto it = m_enum_to_str.find(((CLASS*)obj)->*m_member);
			//set value from iterator
			if (it != m_enum_to_str.end())
			{
				return variant(it->second);
			}
			return variant();
		}


		virtual	const char* get_name() const override
		{
			return m_name.c_str();
		}

	private:

		map_str_to_enum m_str_to_enum;
		map_enum_to_str m_enum_to_str;
		std::string	    m_name;
		ENUM CLASS::*   m_member;

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

		virtual variant get_value(void* obj)
		{
			return variant((((CLASS*)obj)->*m_get)());
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
		virtual variant get_value(void* obj)
		{
			return variant(((CLASS*)obj)->*m_member);
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

	class HCUBE_API properties_vector
	{

	public:
		using property_list			 = std::vector < hcube::property* >;
		using property_list_const_it = property_list::const_iterator;

		//init vector
		properties_vector(std::initializer_list< property* > properties);

		//init vector
		properties_vector(const std::vector< const properties_vector* >& extends,const std::vector< property* >& properties);

		//dealloc vector
		~properties_vector();

		//remove copy
		properties_vector(const properties_vector&) = delete;

		properties_vector& operator=(const properties_vector&) = delete;

		//iterators
		property_list_const_it begin() const;
		property_list_const_it end() const;

	private:

		property_list m_properties;

	};

	template < typename CLASS >
	inline property* make_property_function(std::function< variant_ref (CLASS* obj) > get, const char* name )
	{
		return (property*)new property_function<CLASS>(get, [](CLASS*, variant_ref) {}, name) ;
	}

	template < typename CLASS >
	inline property* make_property_function(std::function < variant_ref (CLASS* obj) > get, 
											std::function < void(CLASS* obj, variant_ref value) > set,
										    const char* name)
	{
		return (property*)new property_function<CLASS>(get, set, name);
	}

	template<typename CLASS, typename T>
	inline property* make_property_member(T CLASS::*member, const char* name)
	{
		return (property*)new property_member<CLASS, T> {member, name};
	}

	template<typename CLASS, typename ENUM>
	inline property* make_property_enum_str(ENUM CLASS::*member, std::initializer_list< std::pair< std::string, ENUM > > map_enum, const char* name)
	{
		return (property*)new property_enum_str<CLASS, ENUM>{ member, map_enum, name };
	}

	template<typename CLASS, typename T, typename I>
	inline property* make_property_get_set(T(CLASS::*get)(void), void(CLASS::*set)(I),  const char* name)
	{
		return (property*)new property_set_get<CLASS, T, I>{get, set, name};
	}

	template<typename CLASS, typename T, typename I>
	inline property* make_property_const_get_set(T(CLASS::*get)(void) const, void(CLASS::*set)(I), const char* name)
	{
		return (property*)new property_set_get<CLASS, T, I>{ (T(CLASS::*)(void))get, set, name };
	}

	#define HCUBE_ENABLE_PROPERTIES\
		friend struct hcube::property;\
		template<typename CLASS, typename T>\
		friend struct hcube::property_member;\
		template<typename CLASS, typename ENUM>\
		friend struct hcube::property_enum_str;\
		template<typename CLASS, typename T, typename I>\
		friend struct hcube::property_set_get;

	#define HCUBE_DEFINE_PROPERTIES(...)\
		HCUBE_ENABLE_PROPERTIES\
		static const properties_vector& properties()\
		{\
			static properties_vector properties\
			({\
				__VA_ARGS__\
			});\
			return properties;\
		}

	#define HCUBE_EXTENDS_PROPERTIES(extends, ...)\
		HCUBE_ENABLE_PROPERTIES\
		static const properties_vector& properties()\
		{\
			static properties_vector properties\
			(\
				{ &extends::properties() },\
				{ __VA_ARGS__ }\
			);\
			return properties;\
		}
}