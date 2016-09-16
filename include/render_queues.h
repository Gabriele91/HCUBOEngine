//
//  render_queue.h
//  HCubo
//
//  Created by Gabriele on 13/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <array>
#include <vector>
#include <entity.h>
#include <vector_math.h>

namespace hcube
{

#define HCUBE_FOREACH_QUEUE(name,queue)\
        for (::hcube::render_element* name = queue; name; name = name->m_next)


	struct render_element
	{
		render_element() {};
		render_element(entity::wptr ref) : m_ref(ref) {};
		//ref to object
		entity::wptr m_ref;
		//list
		render_element* m_next{ nullptr };
		float           m_depth{ ~0 };
		//fake lock
		entity::ptr lock()
		{
			return m_ref.lock();
		}
	};

	template < size_t pool_size = 128 >
	class render_queue
	{
	public:

		//init
		render_queue()
		{
		}

		//add element
		void push_front_to_back(entity::wptr& entity, float depth)
		{
			//test
			if (size() >= m_elements.size()) return;
			//element get
			render_element *e = &m_elements[size()];
			//init
			e->m_ref = entity;
			e->m_depth = depth;
			//next
			e->m_next = nullptr;
			//loop vars
			render_element* last = nullptr;
			render_element* current = m_first;
			//insert sort, front to back
			for (; current;
				last = current,
				current = current->m_next)
			{
				if (current->m_depth > e->m_depth) break;
			}
			//last iteration
			if (last)
			{
				e->m_next = current;
				last->m_next = e;
			}
			else
			{
				e->m_next = m_first;
				m_first = e;
			}
			//inc count
			++m_size;
		}

		void push_back_to_front(entity::wptr& entity, float depth)
		{
			//test
			if (size() >= m_elements.size()) return;
			//element get
			render_element *e = &m_elements[size()];
			//init
			e->m_ref = entity;
			e->m_depth = depth;
			//link
			e->m_next = nullptr;
			//loop vars
			render_element* last = nullptr;
			render_element* current = m_first;
			//insert sort, back to front
			for (; current;
				last = current,
				current = current->m_next)
			{
				if (current->m_depth < e->m_depth) break;
			}
			//last iteration
			if (last)
			{
				e->m_next = current;
				last->m_next = e;
			}
			else
			{
				e->m_next = m_first;
				m_first = e;
			}
			//inc count
			++m_size;
		}

		//first
		render_element* get_first() const
		{
			return m_first;
		}

		size_t size() const
		{
			return m_size;
		}

		//clear
		void clear()
		{
			m_first = nullptr;
			m_size = 0;
		}

	private:

		//fields
		size_t									m_size{ 0 };
		std::array< render_element, pool_size > m_elements;
		render_element*							m_first{ nullptr };
	};


	class render_objects
	{

	public:

		using pool_objects = std::vector < entity::wptr >;

		pool_objects m_lights;
		pool_objects m_renderable;

		void push(entity::ptr e);
		void remove(entity::ptr e);
		void reserve(size_t size);
		void clear();

	};

	enum render_queue_type
	{
		RQ_SPOT_LIGHT,
		RQ_POINT_LIGHT,
		RQ_DIRECTION_LIGHT,
		RQ_OPAQUE,
		RQ_TRANSLUCENT,
		RQ_UI,
		RQ_BACKGROUND,
		RQ_MAX
	};

	class render_scene
	{
	public:
		//all objects
		render_objects m_pool;
		//all queue
		render_queue<128>   m_queues[RQ_MAX];
		//get first element of a queue
		render_element* get_first(render_queue_type queue) const;
		//compue queues
		void compute_lights_queues(const frustum& view_frustum);
		//compue queues
		void compute_no_lights_queues(const std::string& technique_name, const frustum& view_frustum);
		void compute_no_lights_queues(const std::string& technique_name, const vec3& position, float radius);
		//clear
		void clear();
	};
}