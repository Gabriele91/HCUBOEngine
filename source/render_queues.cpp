//
//  render_queues.cpp
//  HCubo
//
//  Created by Gabriele on 13/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//

#include <render_queues.h>


namespace hcube
{
    
    void render_objects::remove(entity::ptr e)
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
        //search opaque renderable object and remove
        for (auto
             it_renderable  = m_opaque.begin();
             it_renderable != m_opaque.end();
             ++it_renderable)
        {
            if (it_renderable->lock() == e)
            {
                m_opaque.erase(it_renderable);
                break;
            }
        }
        //search translucent renderable object and remove
        for (auto
             it_renderable  = m_translucent.begin();
             it_renderable != m_translucent.end();
             ++it_renderable)
        {
            if (it_renderable->lock() == e)
            {
                m_translucent.erase(it_renderable);
                break;
            }
        }
    }
    
    void render_objects::push(entity::ptr e)
    {
        if (e->has_component<light>())      m_lights.push_back({ e });
        if (e->has_component<renderable>()) m_opaque.push_back({ e });
        //else in m_translucent
    }
    
    void render_objects::reserve(size_t size)
    {
        m_lights.reserve(size);
        m_opaque.reserve(size);
        m_translucent.reserve(size);
    }
    
    //compute distance
    static inline float compute_camera_depth(const frustum& f_camera, const transform_ptr& t_entity)
    {
        return f_camera.distance_from_near_plane((vec3)(t_entity->get_matrix()*vec4(0, 0, 0, 1)));
    }
    
    //by frustum
    void render_objects::compute_light_queue(render_queue& rqueue_spot,
                                             render_queue& rqueue_point,
                                             render_queue& rqueue_direction,
                                             const frustum& view_frustum)
    {
        //clear
        rqueue_spot.clear();
        rqueue_point.clear();
        rqueue_direction.clear();
        
        //build queue lights
        for (render_element& weak_element : m_lights)
        {
            auto entity = weak_element.lock();
            auto t_entity = entity->get_component<transform>();
            auto l_entity = entity->get_component<light>();
            const auto l_pos = (vec3)(t_entity->get_matrix()*vec4(0, 0, 0, 1.));
            const auto l_radius = l_entity->get_radius();
            
            if (l_entity->is_enabled() && view_frustum.test_sphere(l_pos, l_radius))
            {
                weak_element.m_depth = compute_camera_depth(view_frustum, t_entity);
                switch (l_entity->get_type())
                {
                    case light::SPOT_LIGHT:      rqueue_spot.push_front_to_back(&weak_element);      break;
                    case light::POINT_LIGHT:     rqueue_point.push_front_to_back(&weak_element);     break;
                    case light::DIRECTION_LIGHT: rqueue_direction.push_front_to_back(&weak_element); break;
                    default: break;
                };
                
            }
        }
        
    }
    
    void render_objects::compute_opaque_queue(render_queue& rqueue,const frustum& view_frustum)
    {
        //clear
        rqueue.clear();
        //build queue opaque
        for (render_element& weak_element : m_opaque)
        {
            auto entity = weak_element.lock();
            auto t_entity = entity->get_component<transform>();
            auto r_entity = entity->get_component<renderable>();
            
            if (r_entity->is_enabled() &&
                ( !r_entity->has_support_culling() ||
                 view_frustum.test_obb(r_entity->get_bounding_box(), t_entity->get_matrix())) )
            {
                weak_element.m_depth = compute_camera_depth(view_frustum, t_entity);
                rqueue.push_front_to_back(&weak_element);
            }
        }
        
    }
    
    void render_objects::compute_translucent_queue(render_queue& rqueue,const frustum& view_frustum)
    {
        //init
        rqueue.clear();
        //build queue translucent
        for (render_element& weak_element : m_translucent)
        {
            auto entity = weak_element.lock();
            auto t_entity = entity->get_component<transform>();
            auto r_entity = entity->get_component<renderable>();
            
            if (r_entity->is_enabled() &&
                (!r_entity->has_support_culling() ||
                 view_frustum.test_obb(r_entity->get_bounding_box(), t_entity->get_matrix()))
                )
            {
                weak_element.m_depth = compute_camera_depth(view_frustum, t_entity);
                rqueue.push_back_to_front(&weak_element);
            }
        }
        
    }
    
    //by sphere
    void render_objects::compute_opaque_queue(render_queue& rqueue,const vec3& position, float radius)
    {
        rqueue.clear();
        //build queue opaque
        for (render_element& weak_element : m_opaque)
        {
            auto entity = weak_element.lock();
            auto t_entity = entity->get_component<transform>();
            auto r_entity = entity->get_component<renderable>();
            
            if(r_entity->is_enabled())
                if(!r_entity->has_support_culling() ||
                   r_entity->get_bounding_box().is_inside(t_entity->get_matrix(), position, radius))
                {
                    weak_element.m_depth = distance(position, t_entity->get_global_position());
                    rqueue.push_front_to_back(&weak_element);
                }
        }
    }
    
    void render_objects::compute_translucent_queue(render_queue& rqueue,const vec3& position, float radius)
    {
        //init
        rqueue.clear();
        //build queue translucent
        for (render_element& weak_element : m_translucent)
        {
            auto entity = weak_element.lock();
            auto t_entity = entity->get_component<transform>();
            auto r_entity = entity->get_component<renderable>();
            
            if(r_entity->is_enabled())
                if(!r_entity->has_support_culling() ||
                   r_entity->get_bounding_box().is_inside(t_entity->get_matrix(), position, radius))
            {
                weak_element.m_depth = distance(position, t_entity->get_global_position());
                rqueue.push_back_to_front(&weak_element);
            }
        }
        
    }
    
    //push element
    void render_queue::push_front_to_back(render_element* e)
    {
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
    void render_queue::push_back_to_front(render_element* e)
    {
        e->m_next = nullptr;
        //loop vars
        render_element* last = nullptr;
        render_element* current = m_first;
        //insert sort, back to front
        for (;current;
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
    
    //clear all
    void render_objects::clear()
    {
        m_lights.clear();
        m_opaque.clear();
        m_translucent.clear();
    }
    
    //compue queues
    void render_scene::compute_queues(const frustum& view_frustum)
    {
        m_pool.compute_light_queue(m_queues[RQ_SPOT_LIGHT],
                                   m_queues[RQ_POINT_LIGHT],
                                   m_queues[RQ_DIRECTION_LIGHT],
                                   view_frustum);
        m_pool.compute_opaque_queue(m_queues[RQ_OPAQUE],view_frustum);
        m_pool.compute_translucent_queue(m_queues[RQ_TRANSLUCENT],view_frustum);
    }
    //clear
    void render_scene::clear()
    {
        m_pool.clear();
        for(auto& queue:m_queues) queue.clear();
    }
    

}