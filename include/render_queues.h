//
//  render_queue.h
//  HCubo
//
//  Created by Gabriele on 13/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <queue>
#include <camera.h>
#include <entity.h>
#include <effect.h>
#include <smart_pointers.h>
#include <system_manager.h>
#include <vector_math.h>

namespace hcube
{
    
    #define HCUBE_FOREACH_QUEUE(name,queue)\
        for (::hcube::render_element* name = queue; name; name = name->m_next)

    
    struct render_element
    {
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
    
    class render_queue
    {
    public:
        
        //init
        render_queue(){}
        
        //add element
        void push_front_to_back(render_element* e);
        void push_back_to_front(render_element* e);
        
        //first
        render_element* get_first() const
        {
            return m_first;
        }
        
        size_t get_size() const
        {
            return m_size;
        }
        
        //clear
        void clear()
        {
            m_first = nullptr;
            m_size  = 0;
        }
        
    private:
        
        //fields
        size_t          m_size  { 0        };
        render_element* m_first {  nullptr };
    };
    
    
    class render_objects
    {
        
    public:
        
        using pool_objects = std::vector < render_element >;
        
        pool_objects m_lights;
        pool_objects m_opaque;
        pool_objects m_translucent;
        
        void push(entity::ptr e);
        void remove(entity::ptr e);
        void reserve(size_t size);
        void clear();
        
        //by frustum
        void compute_light_queue(render_queue& rqueue_spot,
                                 render_queue& rqueue_point,
                                 render_queue& rqueue_direction,
                                 const frustum& view_frustum);
        void compute_opaque_queue(render_queue& rqueue,const frustum& view_frustum);
        void compute_translucent_queue(render_queue& rqueue,const frustum& view_frustum);
        
        //by sphere
        void compute_opaque_queue(render_queue& rqueue,const vec3& position, float radius);
        void compute_translucent_queue(render_queue& rqueue,const vec3& position, float radius);
        
    };
    
    enum queue_type
    {
        RQ_SPOT_LIGHT,
        RQ_POINT_LIGHT,
        RQ_DIRECTION_LIGHT,
        RQ_OPAQUE,
        RQ_TRANSLUCENT,
        RQ_MAX
    };
    
    class render_scene
    {
    public:
        //all objects
        render_objects m_pool;
        //all queue
        render_queue   m_queues[RQ_MAX];
        //compue queues
        void compute_queues(const frustum& view_frustum);
        //clear
        void clear();
    };
}