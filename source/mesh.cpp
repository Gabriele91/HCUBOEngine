//
//  mesh.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <mesh.h>
#include <OpenGL4.h>

void mesh::build(const std::vector< glm::vec3 >& points)
{
    destoy();
    build_vertex(points);
}

void mesh::build(const std::vector< glm::vec3 >& points,
                 const std::vector< glm::vec2 >& uvmap)
{
    destoy();
    build_vertex(points);
    build_uvmap(uvmap);
}


void mesh::build(const std::vector< glm::vec3 >& points,
                 const std::vector< glm::vec3 >& colors)
{
    destoy();
    build_vertex(points);
    build_color(colors);
}

void mesh::build(const std::vector< glm::vec3 >& points,
                 const std::vector< glm::vec2 >& uvmap,
                 const std::vector< glm::vec3 >& colors)
{
    destoy();
    build_vertex(points);
    build_uvmap(uvmap);
    build_color(colors);
}

void mesh::build(const std::vector< unsigned int >& indexs,
                 const std::vector< glm::vec3 >& points)
{
    destoy();
    build_index(indexs);
    build_vertex(points);
}

void mesh::build(const std::vector< unsigned int >& indexs,
                 const std::vector< glm::vec3 >& points,
                 const std::vector< glm::vec2 >& uvmap)
{
    destoy();
    build_index(indexs);
    build_vertex(points);
    build_uvmap(uvmap);
}


void mesh::build(const std::vector< unsigned int >& indexs,
                 const std::vector< glm::vec3 >& points,
                 const std::vector< glm::vec3 >& colors)
{
    destoy();
    build_index(indexs);
    build_vertex(points);
    build_color(colors);
}

void mesh::build(const std::vector< unsigned int >& indexs,
                 const std::vector< glm::vec3 >& points,
                 const std::vector< glm::vec2 >& uvmap,
                 const std::vector< glm::vec3 >& colors)
{
    destoy();
    build_index(indexs);
    build_vertex(points);
    build_uvmap(uvmap);
    build_color(colors);
}


void mesh::set_draw_info(const draw_info& info)
{
    m_info = info;
}

void mesh::draw()
{
    if(m_bvertex) set_buffer(m_bvertex,m_info.m_attribute_vertex, 3);
    if(m_buvmap)  set_buffer(m_buvmap, m_info.m_attribute_uvmap,  2);
    if(m_bcolor)  set_buffer(m_bcolor, m_info.m_attribute_color,  3);
    
    if(m_bindex)
    {
        //set index
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_bindex);
        //draw
        glDrawElements(m_info.m_draw_mode, m_bindex_size, GL_UNSIGNED_INT, (void*)0);
        //unbind
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer (GL_ARRAY_BUFFER, 0);
    }
    else
    {
        //unbind index
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
        //draw
        glDrawArrays (m_info.m_draw_mode, 0, m_bvertex_size);
        //unbind buffer
        glBindBuffer (GL_ARRAY_BUFFER, 0);
    }
    
    if(m_bvertex) glDisableVertexAttribArray(m_info.m_attribute_vertex);
    if(m_buvmap)  glDisableVertexAttribArray(m_info.m_attribute_uvmap);
    if(m_bcolor)  glDisableVertexAttribArray(m_info.m_attribute_color);
    
}

void mesh::destoy()
{
    if(m_bindex)  glDeleteBuffers(1, &m_bindex);
    if(m_bvertex) glDeleteBuffers(1, &m_bvertex);
    if(m_buvmap)  glDeleteBuffers(1, &m_buvmap);
    if(m_bcolor)  glDeleteBuffers(1, &m_bcolor);
    m_bvertex_size= 0;
    m_bindex_size = 0;
    m_bindex  = 0;
    m_bvertex = 0;
    m_buvmap  = 0;
    m_bcolor  = 0;
}

void mesh::build_index(const std::vector< unsigned int >& indexs)
{
    //save size
    m_bindex_size = (unsigned int)indexs.size();
    //create buffer
    glGenBuffers (1, &m_bindex);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_bindex);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, indexs.size()* sizeof (unsigned int), indexs.data(), GL_STATIC_DRAW);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}

void mesh::build_vertex(const std::vector< glm::vec3 >& points)
{
    //save size
    m_bvertex_size = (unsigned int)points.size();
    //create buffer
    glGenBuffers (1, &m_bvertex);
    glBindBuffer (GL_ARRAY_BUFFER, m_bvertex);
    glBufferData (GL_ARRAY_BUFFER, points.size()* 3 * sizeof (float), points.data(), GL_STATIC_DRAW);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
}

void mesh::build_uvmap(const std::vector< glm::vec2 >& uvmap)
{
    
    glGenBuffers (1, &m_buvmap);
    glBindBuffer (GL_ARRAY_BUFFER, m_buvmap);
    glBufferData (GL_ARRAY_BUFFER, uvmap.size()* 2 * sizeof (float), uvmap.data(), GL_STATIC_DRAW);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
}

void mesh::build_color(const std::vector< glm::vec3 >& colors)
{
    glGenBuffers (1, &m_bcolor);
    glBindBuffer (GL_ARRAY_BUFFER, m_bcolor);
    glBufferData (GL_ARRAY_BUFFER, colors.size()* 3 * sizeof (float), colors.data(), GL_STATIC_DRAW);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
}

void mesh::set_buffer(unsigned int buffer,int attribute_location, int strip)
{
    glBindBuffer (GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(attribute_location);
    glVertexAttribPointer (attribute_location, strip, GL_FLOAT, GL_FALSE, 0, NULL);
}


mesh::~mesh()
{
    destoy();
}