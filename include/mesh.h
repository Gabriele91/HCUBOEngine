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
    
    mesh(){}
    mesh(const std::vector< glm::vec3 >& points){ build(points); };
    
    mesh(const std::vector< glm::vec3 >& points,
         const std::vector< glm::vec2 >& uvmap)
    {
        build(points,uvmap);
    }
    
    mesh(const std::vector< glm::vec3 >& points,
         const std::vector< glm::vec3 >& colors)
    {
        build(points,colors);
    }
    
    mesh(const std::vector< glm::vec3 >& points,
         const std::vector< glm::vec2 >& uvmap,
         const std::vector< glm::vec3 >& colors)
    {
        build(points,uvmap,colors);
    }
    
    mesh(const std::vector< unsigned int >& indexs,
         const std::vector< glm::vec3 >& points)
    {
        build(indexs,points);
    }

    
    mesh(const std::vector< unsigned int >& indexs,
         const std::vector< glm::vec3 >& points,
         const std::vector< glm::vec2 >& uvmap)
    
    {
        build(indexs,points,uvmap);
    }
    
    mesh(const std::vector< unsigned int >& indexs,
         const std::vector< glm::vec3 >& points,
         const std::vector< glm::vec3 >& colors)
    {
        build(indexs,points,colors);
    }
    
    mesh(const std::vector< unsigned int >& indexs,
         const std::vector< glm::vec3 >& points,
         const std::vector< glm::vec2 >& uvmap,
         const std::vector< glm::vec3 >& colors)
    {
        build(indexs,points,uvmap,colors);
    }
    
    void build(const std::vector< glm::vec3 >& points);
    
    void build(const std::vector< glm::vec3 >& points,
               const std::vector< glm::vec2 >& uvmap);
    
    void build(const std::vector< glm::vec3 >& points,
               const std::vector< glm::vec3 >& colors);
    
    void build(const std::vector< glm::vec3 >& points,
               const std::vector< glm::vec2 >& uvmap,
               const std::vector< glm::vec3 >& colors);
    
    void build(const std::vector< unsigned int >& indexs,
               const std::vector< glm::vec3 >& points);
    
    void build(const std::vector< unsigned int >& indexs,
               const std::vector< glm::vec3 >& points,
               const std::vector< glm::vec2 >& uvmap);
    
    void build(const std::vector< unsigned int >& indexs,
               const std::vector< glm::vec3 >& points,
               const std::vector< glm::vec3 >& colors);
    
    void build(const std::vector< unsigned int >& indexs,
               const std::vector< glm::vec3 >& points,
               const std::vector< glm::vec2 >& uvmap,
               const std::vector< glm::vec3 >& colors);
    
    struct draw_info
    {
        int m_draw_mode       { GL_TRIANGLES };
        int m_attribute_vertex{ 0 };
        int m_attribute_uvmap { 1 };
        int m_attribute_color { 2 };
        
        draw_info()
        {
        }
        
        draw_info(int draw_mode)
        {
            m_draw_mode = draw_mode;
        }
        
        draw_info(int draw_mode,int attribute_vertex)
        {
            m_draw_mode = draw_mode;
            m_attribute_vertex = attribute_vertex;
        }
        
        draw_info(int draw_mode,int attribute_vertex,int attribute_uvmap)
        {
            m_draw_mode = draw_mode;
            m_attribute_vertex = attribute_vertex;
            m_attribute_uvmap  = attribute_uvmap;
        }
        
        draw_info(int draw_mode,int attribute_vertex,int attribute_uvmap,int attribute_color)
        {
            m_draw_mode = draw_mode;
            m_attribute_vertex = attribute_vertex;
            m_attribute_uvmap  = attribute_uvmap;
            m_attribute_color  = attribute_color;
        }
        
    };
    
    void set_draw_info(const draw_info& info);
    
    void draw();
    
    void destoy();
    
    virtual ~mesh();
    
protected:
    
    void build_index(const std::vector< unsigned int >& indexs);
    void build_vertex(const std::vector< glm::vec3 >& points);
    void build_uvmap(const std::vector< glm::vec2 >& points);
    void build_color(const std::vector< glm::vec3 >& points);
    
    void set_buffer(unsigned int buffer,int attribute_location, int strip);
    
    draw_info    m_info;
    unsigned int m_bvertex_size{ 0 };
    unsigned int m_bindex_size { 0 };
    unsigned int m_bindex      { 0 };
    unsigned int m_bvertex     { 0 };
    unsigned int m_buvmap      { 0 };
    unsigned int m_bcolor      { 0 };
    
};