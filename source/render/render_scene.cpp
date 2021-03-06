//
//  render_scene.cpp
//  HCubo
//
//  Created by Gabriele on 13/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <hcube/render/render_scene.h>
#include <hcube/geometries/intersection.h>
#include <hcube/components/light.h>
#include <hcube/components/renderable.h>


namespace hcube
{

	render_scene_queue::render_scene_queue(size_t capacity)
	{
		//get page size
		size_t npage = capacity / page_capacity;
		//upper boud
		if (capacity % page_capacity) ++npage;
		//alloc
		for (size_t i = 0; i != npage; ++i) new_page();
	}
	//pages
	void render_scene_queue::new_page()
	{
		m_pages.push_back(std::unique_ptr<pool_page>(new pool_page));
	}
	
	render_scene_element* render_scene_queue::get_new_element()
	{
		size_t page    = m_size / page_capacity;
		size_t element = m_size % page_capacity;
		//compute new size
		++m_size;
		//alloc
		if (page == m_pages.size()) new_page();
		//get
		return &(*m_pages[page])[element];
	}
	
	//add 
	void render_scene_queue::push_front_to_back(entity::wptr entity, float depth)
	{
		//element get
		render_scene_element *e = get_new_element();
		//init
		e->m_ref = entity;
		e->m_depth = depth;
		//next
		e->m_next = nullptr;
		//loop vars
		render_scene_element* last = nullptr;
		render_scene_element* current = m_first;
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
	}
	
	void render_scene_queue::push_back_to_front(entity::wptr entity, float depth)
	{
		//element get
		render_scene_element *e = get_new_element();
		//init
		e->m_ref = entity;
		e->m_depth = depth;
		//link
		e->m_next = nullptr;
		//loop vars
		render_scene_element* last = nullptr;
		render_scene_element* current = m_first;
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
	}
    
	//pool
    void render_scene_objects::remove(entity::ptr e)
    {
        //search light and remove
        for (auto
             it_light  = m_lights.begin();
             it_light != m_lights.end();
             ++it_light)
        {
            if (it_light->lock() == e)
            {
                m_lights.erase(it_light);
                break;
            }
        }
        //search renderable object and remove
        for (auto
             it_renderable  = m_renderable.begin();
             it_renderable != m_renderable.end();
             ++it_renderable)
        {
            if (it_renderable->lock() == e)
            {
                m_renderable.erase(it_renderable);
                break;
            }
        }
    }
    
    void render_scene_objects::push(entity::ptr e)
    {
        if (e->has_component<light>())      m_lights.push_back( e );
        if (e->has_component<renderable>()) m_renderable.push_back( e );
    }
    
    void render_scene_objects::reserve(size_t size)
    {
        m_lights.reserve(size);
        m_renderable.reserve(size);
    }
    
    //clear all
    void render_scene_objects::clear()
    {
        m_lights.clear();
        m_renderable.clear();
    }
    
    //get iterator
    render_scene_element* render_scene::get_first(render_scene_queue_type queue) const
    {
        return m_queues[queue].get_first();
    }
    //clear
    void render_scene::clear()
    {
        m_pool.clear();
        for(auto& queue:m_queues) queue.clear();
    }
    
    
    //compute distance
    static inline float compute_camera_depth(const frustum& f_camera, const transform_ptr& t_entity)
    {
        return f_camera.distance_from_near_plane(t_entity->get_global_position());
    }
    
    //by frustum
    void render_scene::compute_lights_queues(const frustum& view_frustum)
    {
        //ref
        auto& rqueue_spot        = m_queues[RQ_SPOT_LIGHT];
		auto& rqueue_point       = m_queues[RQ_POINT_LIGHT];
		auto& rqueue_direction   = m_queues[RQ_DIRECTION_LIGHT];
        //clear
        rqueue_spot.clear();
        rqueue_point.clear();
        rqueue_direction.clear();
        
        //build queue lights
        for (entity::wptr weak_entity: m_pool.m_lights)
        {
            auto entity   = weak_entity.lock();
            auto t_entity = entity->get_component<transform>();
            auto l_entity = entity->get_component<light>();
            
            if (l_entity->is_enabled() && 
				intersection::check(view_frustum, l_entity->get_sphere()) != intersection::OUTSIDE)
            {
                float depth = compute_camera_depth(view_frustum, t_entity);
                switch (l_entity->get_type())
                {
                    case light::SPOT_LIGHT:      rqueue_spot.push_front_to_back(weak_entity,depth);      break;
                    case light::POINT_LIGHT:     rqueue_point.push_front_to_back(weak_entity,depth);     break;
                    case light::DIRECTION_LIGHT: rqueue_direction.push_front_to_back(weak_entity,depth); break;
                    default: break;
                };
                
            }
        }
        
    }
    
    void render_scene::compute_no_lights_queues(const frustum& view_frustum)
    {
        //clear
        m_queues[RQ_BACKGROUND].clear();
        m_queues[RQ_OPAQUE].clear();
        m_queues[RQ_TRANSLUCENT].clear();
        m_queues[RQ_UI].clear();
        //build queue opaque
        for (entity::wptr weak_entity : m_pool.m_renderable)
        {
            auto entity     = weak_entity.lock();
            auto r_entity   = entity->get_component<renderable>();

			if (r_entity->is_enabled())
			if (material_ptr       r_material = r_entity->get_material())
			if (effect::ptr        r_effect   = r_material->get_effect())
			if (transform_ptr      t_entity   = entity->get_component<transform>())
			if (!r_entity->has_support_culling() ||
				intersection::check(view_frustum, r_entity->get_bounding_box()) != intersection::OUTSIDE
			)
			{
				///queue
				const effect::parameter_queue& queue = r_effect->get_queue();
				//distance
				switch (queue.m_type)
				{
				case RQ_TRANSLUCENT:
					m_queues[RQ_TRANSLUCENT].push_back_to_front(weak_entity, compute_camera_depth(view_frustum, t_entity));
					break;
				case RQ_OPAQUE:
					m_queues[RQ_OPAQUE].push_front_to_back(weak_entity, compute_camera_depth(view_frustum, t_entity));
					break;
				default:
					m_queues[queue.m_type].push_back_to_front(weak_entity, queue.m_order);
					break;
				}
			}
        }
    }
    
    //by sphere
    void render_scene::compute_no_lights_queues(const sphere& in_sphere)
    {
        //clear
        m_queues[RQ_BACKGROUND].clear();
        m_queues[RQ_OPAQUE].clear();
        m_queues[RQ_TRANSLUCENT].clear();
        m_queues[RQ_UI].clear();
        //build queue opaque
        for (entity::wptr weak_entity : m_pool.m_renderable)
        {
            auto entity     = weak_entity.lock();
            auto r_entity   = entity->get_component<renderable>();

			if (r_entity->is_enabled())
			if (material_ptr       r_material = r_entity->get_material())
			if (effect::ptr        r_effect   = r_material->get_effect())
			if (transform_ptr      t_entity   = entity->get_component<transform>())
			if (!r_entity->has_support_culling() ||
				intersection::check(r_entity->get_bounding_box(), in_sphere))
			{
				///queue
				const effect::parameter_queue& queue = r_effect->get_queue();
				//distance
				switch (queue.m_type)
				{
				case RQ_TRANSLUCENT:
					m_queues[RQ_TRANSLUCENT].push_back_to_front(weak_entity, distance(t_entity->get_global_position(), in_sphere.get_position()));
					break;
				case RQ_OPAQUE:
					m_queues[RQ_OPAQUE].push_front_to_back(weak_entity, distance(t_entity->get_global_position(), in_sphere.get_position()));
					break;
				default:
					m_queues[queue.m_type].push_back_to_front(weak_entity, queue.m_order);
					break;
				}
			}
		}
    }

}