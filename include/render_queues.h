//
//  render_queue.h
//  HCubo
//
//  Created by Gabriele on 13/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
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
    
    class render_queue
    {
    public:
        
        //init
        render_queue(size_t capacity=256)
        {
            m_elements.resize(capacity);
        }
        
        //add element
        void push_front_to_back(entity::wptr& e,float depth);
        void push_back_to_front(entity::wptr& e,float depth);
        
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
            m_size  = 0;
        }
        
    private:
        
        //fields
        size_t                      m_size  { 0 };
        std::vector<render_element> m_elements;
        render_element*             m_first { nullptr };
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
        render_queue   m_queues[RQ_MAX];
        //get first element of a queue
        render_element* get_first(render_queue_type queue) const;
        //compue queues
        void compute_queues(const frustum& view_frustum);
        //compue queues
        void compute_lights_queues(const frustum& view_frustum);
        //compue queues
        void compute_no_lights_queues(const frustum& view_frustum);
        void compute_no_lights_queues(const vec3& position, float radius);
        //clear
        void clear();
    };
}