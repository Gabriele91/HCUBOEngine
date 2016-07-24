//
//  mesh.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <mesh.h>
#include <OpenGL4.h>

void mesh::build(const mesh_layout& layout,
                 const std::vector< unsigned int >& indexs,
                 const std::vector< byte >& vertex)
{
    destoy();
    m_layout = layout;
    build_index(indexs);
    build_vertex(vertex);
}

void mesh::build(const mesh_layout& layout,
                 const draw_range&  range,
                 const std::vector< byte >& vertex)
{
    destoy();
    m_layout = layout;
    m_range  = range;
    build_vertex(vertex);
}

void mesh::draw(const glm::vec4& viewport,
                const glm::mat4& projection,
                const glm::mat4& view,
                const glm::mat4& model,
                material_ptr material)
{
	if (material)
	{
		material->bind_state();
		material->bind(viewport,
                       projection,
                       view,
                       model);
	}

	mesh::draw();

	if (material)
	{
		material->unbind();
		material->unbind_state();
	}
}

void mesh::draw()
{
    //bind buffer
    glBindBuffer (GL_ARRAY_BUFFER, m_bvertex);
    
    //enable all attribute
    for(const input& data : m_layout.m_input_layout)
    {
        glEnableVertexAttribArray(data.m_attribute);
        glVertexAttribPointer (data.m_attribute,
                               data.m_strip,
                               GL_FLOAT,
                               GL_FALSE,
                               data.m_size,
                               ((char *)NULL + (data.m_offset)));
    }
    
    if(m_bindex)
    {
        //set index
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_bindex);
        //draw
        glDrawElements(m_layout.m_draw_mode, m_bindex_size, GL_UNSIGNED_INT, (void*)0);
        //unbind
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer (GL_ARRAY_BUFFER, 0);
    }
    else
    {
        //unbind index
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
        //draw
        glDrawArrays (m_layout.m_draw_mode, m_range.m_min, m_range.m_max);
        //unbind buffer
        glBindBuffer (GL_ARRAY_BUFFER, 0);
    }
    
    //disable all attribute
    for(const input& data : m_layout.m_input_layout)
    {
        glDisableVertexAttribArray(data.m_attribute);
    }
    
}

void mesh::destoy()
{
    if(m_bindex)  glDeleteBuffers(1, &m_bindex);
    if(m_bvertex) glDeleteBuffers(1, &m_bvertex);
    m_bvertex_size= 0;
    m_bindex_size = 0;
    m_bindex  = 0;
    m_bvertex = 0;
}

void mesh::build_index(const std::vector< unsigned int >& indexs)
{
    //save size
    m_bindex_size = (unsigned int)indexs.size();
    //create buffer
    glGenBuffers (1, &m_bindex);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_bindex);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, indexs.size() * sizeof (unsigned int), indexs.data(), m_layout.m_buffer_mode);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}

void mesh::build_vertex(const std::vector< byte >& points)
{
    //save size
    m_bvertex_size = (unsigned int)points.size();
    //create buffer
    glGenBuffers (1, &m_bvertex);
    glBindBuffer (GL_ARRAY_BUFFER, m_bvertex);
    glBufferData (GL_ARRAY_BUFFER, points.size(), points.data(), m_layout.m_buffer_mode);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
}

mesh::~mesh()
{
    destoy();
}