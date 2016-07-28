//
//  mesh.h
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <OpenGL4.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <renderable.h>
#include <smart_pointers.h>

class mesh :  public smart_pointers< mesh >,  public renderable
{

public:
    //byte type
    using byte = unsigned char;
    //layout field
    struct input
    {
        int m_attribute;
        int m_size;
        int m_strip;
        size_t m_offset;
    };
    //layout
    using input_layout = std::vector< input >;
    //mesh layout
    struct mesh_layout
    {
        input_layout m_input_layout;
        int m_draw_mode;
        int m_buffer_mode;
    };
    //draw range (no indexed)
    struct draw_range
    {
        unsigned int m_min{ 0 };
        unsigned int m_max{ 0 };
        
        draw_range(){}
        
        draw_range(unsigned int max)
        {
            m_max = max;
        }
        
        draw_range(unsigned int min,unsigned int max)
        {
            m_max = min;
            m_max = max;
        }
    };
    
    mesh(){}
    
    mesh(const mesh_layout& layout,
         const draw_range & draw_range,
         const std::vector< byte >& vertex)
    {
        build(layout, draw_range, vertex);
    }
    
    mesh(const mesh_layout& layout,
         const std::vector< unsigned int >& indexs,
         const std::vector< byte >& vertex)
    {
        build(layout, indexs, vertex);
    }
    
    void build(const mesh_layout& layout,
               const std::vector< unsigned int >& indexs,
               const std::vector< byte >& points);
    
    void build(const mesh_layout& layout,
               const draw_range & draw_range,
               const std::vector< byte >& points);

	void draw();

    void draw(const glm::vec4& viewport,
              const glm::mat4& projection,
              const glm::mat4& view,
              const glm::mat4& model,
              material_ptr material);
    
    void destoy();
    
    virtual ~mesh();

	virtual component_ptr copy() const;
    
protected:
    
    void build_index(const std::vector< unsigned int >& indexs);
    void build_vertex(const std::vector< byte >& points);
    
    draw_range   m_range;
    mesh_layout  m_layout;
    unsigned int m_bvertex_size{ 0 };
    unsigned int m_bindex_size { 0 };
    unsigned int m_bindex      { 0 };
    unsigned int m_bvertex     { 0 };
    
};