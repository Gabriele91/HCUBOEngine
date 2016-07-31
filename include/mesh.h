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
        int    m_attribute;
        int    m_strip;
        size_t m_offset;
		bool   m_is_position;

		input(int   attribute,
		  	  int   strip,
			  size_t offset)
		{
			m_attribute = attribute;
			m_strip = strip;
			m_offset = offset;
			m_is_position = false;
		}

		input(int   attribute,
			  int   strip,
			  size_t offset,
			  bool   is_position)
		{
			m_attribute = attribute;
			m_strip = strip;
			m_offset = offset;
			m_is_position = is_position;
		}
    };
    //layout
	struct input_layout
	{
		int m_size;
		std::vector< input > m_fields;
	};
    //mesh layout
    struct mesh_layout
    {
        input_layout m_input_layout;
        int m_draw_mode;
        int m_buffer_mode;

		bool has_a_position() const;
		size_t position_offset() const;

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

	virtual ~mesh();

    mesh(const mesh_layout& layout,
         const draw_range & draw_range,
         const std::vector< byte >& vertex,
		 bool compute_obb = true)
    {
        build(layout, draw_range, vertex, compute_obb);
    }
    
    mesh(const mesh_layout& layout,
         const std::vector< unsigned int >& indexs,
         const std::vector< byte >& vertex,
		 bool compute_obb = true)
    {
        build(layout, indexs, vertex, compute_obb);
    }
    
    void build(const mesh_layout& layout,
               const std::vector< unsigned int >& indexs,
               const std::vector< byte >& points,
		       bool compute_obb = true);

	void build(const mesh_layout& layout,
			   const draw_range & draw_range,
		       const std::vector< byte >& points,
		       bool compute_obb = true);

	void build(const mesh_layout& layout,
		       const unsigned int* indexs,
		       size_t isize,
		       const byte* points,
		       size_t vsize,
		       bool compute_obb = true);

	void build(const mesh_layout& layout,
		       const draw_range & draw_range,
		       const byte* points,
			   size_t size,
		       bool compute_obb = true);
	
	void set_bounding_box(const obb& box);

	void disable_support_culling();

	void draw();

    void draw(const glm::vec4& viewport,
              const glm::mat4& projection,
              const glm::mat4& view,
              const glm::mat4& model,
              material_ptr material);
    
    void destoy();
    
	virtual component_ptr copy() const;
	    
protected:

	bool compute_bounding_box(const unsigned int* indexs,
							  size_t size,
							  const byte* points, 
							  size_t vsize);

	bool compute_bounding_box(const byte* points, 
							  size_t vsize);

	void build_index(const std::vector< unsigned int >& indexs);
	void build_vertex(const std::vector< byte >& points);
	void build_index(const unsigned int* indexs,size_t size);
	void build_vertex(const byte* points,size_t size);
    
    draw_range   m_range;
    mesh_layout  m_layout;
    unsigned int m_bvertex_size{ 0 };
    unsigned int m_bindex_size { 0 };
    unsigned int m_bindex      { 0 };
    unsigned int m_bvertex     { 0 };
    
};