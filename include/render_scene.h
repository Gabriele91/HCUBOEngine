//
//  render_scene.h
//  HCubo
//
//  Created by Gabriele on 13/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <array>
#include <vector>
#include <entity.h>
#include <sphere.h>
#include <frustum.h>
#include <vector_math.h>

namespace hcube
{

#define HCUBE_FOREACH_QUEUE(name,queue)\
        for (::hcube::render_scene_element* name = queue; name; name = name->m_next)


	struct render_scene_element
	{
		render_scene_element() {};
		render_scene_element(entity::wptr ref) : m_ref(ref) {};
		//ref to object
		entity::wptr m_ref;
		//list
		render_scene_element* m_next{ nullptr };
		float                 m_depth{ ~0 };
		//fake lock
		entity::ptr lock()
		{
			return m_ref.lock();
		}
	};

	class render_scene_queue
	{
	public:

		//init
		render_scene_queue(size_t capacity = 512);

		//add element
		void push_front_to_back(entity::wptr entity, float depth);

		void push_back_to_front(entity::wptr entity, float depth);

		//first
		render_scene_element* get_first() const
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
		//static size
		static const size_t page_capacity = 128;
		
		//pool type
		using pool_page  = std::array< render_scene_element, page_capacity >;
		using pool_pages = std::vector< std::unique_ptr<pool_page> >;

		//pool utilities
		void new_page();
		render_scene_element* get_new_element();

		//fields
		size_t                  m_size{ 0 };
		pool_pages              m_pages;
		render_scene_element*	m_first{ nullptr };
	};

	class render_scene_objects
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

	enum render_scene_queue_type
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
		render_scene_objects  m_pool;
		//all queue
		render_scene_queue  m_queues[RQ_MAX];
		//get first element of a queue
		render_scene_element* get_first(render_scene_queue_type queue) const;
		//compue queues
		void compute_lights_queues(const frustum& view_frustum);
		//compue queues
		void compute_no_lights_queues(const frustum& view_frustum);
		void compute_no_lights_queues(const sphere& in_sphere);
		//clear
		void clear();
	};
}