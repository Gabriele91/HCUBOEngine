//
//  text_mesh.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <vector>
#include <text_mesh.h>
#include <OpenGL4.h>
#include <algorithm>


text_mesh::text_mesh(size_t text_max_size)
{
    //set max size
    m_text_max_size = text_max_size;
    //gen buffer
    glGenBuffers (1, &m_bpoints);
    glBindBuffer (GL_ARRAY_BUFFER, m_bpoints);
    glBufferData (GL_ARRAY_BUFFER, text_max_size * sizeof (int), NULL, GL_STREAM_DRAW);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
}

text_mesh::~text_mesh()
{
    glDeleteBuffers(1, &m_bpoints);
}

void text_mesh::update_mesh()
{
    //bind
    glBindBuffer(GL_ARRAY_BUFFER, m_bpoints);
    ///get point
    int* points = (int*)glMapBufferRange(GL_ARRAY_BUFFER,
                                         0,
                                         m_text_max_size * sizeof (int),
                                         GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT );
    // stream data
    for(size_t i=0; i!=m_text_max_size && i!=m_text.size(); ++i)
    {
        points[i] = (int)m_text[i];
    }
    // unmap buffer
    glUnmapBuffer(GL_ARRAY_BUFFER);
    //unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void text_mesh::set_text(const std::u32string& text)
{
    if(m_text != text)
    {
        m_text = text;
        update_mesh();
    }
}

void text_mesh::set_text(const std::string& text)
{
    std::u32string u32text;
    for(char c:text) u32text += (int)c;
    set_text(u32text);
}


void text_mesh::draw(const glm::vec4& viewport,
                     const glm::mat4& projection,
                     const glm::mat4& view,
                     const glm::mat4& model,
                     material_ptr material)
{
	if (material)
	{
		material->bind_state();
		material->bind(viewport, projection, view, model);
	}

	text_mesh::draw();

	if (material)
	{
		material->unbind();
		material->unbind_state();
	}
}

void text_mesh::draw()
{
    const int attribute_location = 0;
    ////////////////////////////////////////////////////////
    //unbind index
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    ////////////////////////////////////////////////////////
    glBindBuffer(GL_ARRAY_BUFFER, m_bpoints);
    glEnableVertexAttribArray(attribute_location);
    glVertexAttribIPointer (attribute_location, 1, GL_INT, 0, nullptr);
    ////////////////////////////////////////////////////////
    //draw
    glDrawArrays (GL_POINTS, 0, (GLuint)std::min(m_text.size(),m_text_max_size));
    ////////////////////////////////////////////////////////
    //unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}