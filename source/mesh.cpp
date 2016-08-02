//
//  mesh.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <mesh.h>
#include <OpenGL4.h>


bool mesh::mesh_layout::has_a_position() const
{
	for (auto& in : m_input_layout.m_fields)
		if (in.m_is_position)
			return true;
    return false;
}

size_t mesh::mesh_layout::position_offset() const
{
	for (auto& in : m_input_layout.m_fields)
		if (in.m_is_position)
			return in.m_offset;
	return ~(size_t)0;
}


mesh::~mesh()
{
	destoy();
}

void mesh::build(const mesh_layout& layout,
                 const std::vector< unsigned int >& indexs,
                 const std::vector< byte >& vertex,
	             bool compute_obb)
{
    destoy();
    m_layout = layout;
    build_index(indexs);
    build_vertex(vertex);
	if (compute_obb) compute_bounding_box(indexs.data(), indexs.size(), vertex.data(), vertex.size());
}

void mesh::build(const mesh_layout& layout,
                 const draw_range&  range,
                 const std::vector< byte >& vertex,
	             bool compute_obb)
{
    destoy();
    m_layout = layout;
    m_range  = range;
    build_vertex(vertex);
	if (compute_obb) compute_bounding_box(vertex.data(), vertex.size());
}

void mesh::build(const mesh_layout& layout,
	             const  unsigned int* indexs,
				 size_t isize,
				 const byte* vertex,
				 size_t vsize,
			     bool compute_obb)
{
	destoy();
	m_layout = layout;
	build_index(indexs,isize);
	build_vertex(vertex,vsize);
	if (compute_obb) compute_bounding_box(indexs, isize, vertex, vsize);
}

void mesh::build(const mesh_layout& layout,
				 const draw_range & range,
				 const byte* vertex,
				 size_t size,
				 bool compute_obb)
{
	destoy();
	m_layout = layout;
	m_range = range;
	build_vertex(vertex,size);
	if (compute_obb) compute_bounding_box(vertex, size);
}

bool mesh::compute_bounding_box(const unsigned int* indexs,
						        size_t isize,
								const byte* points,
								size_t vsize)
{
	if (!m_layout.has_a_position()) return false;
	//compute box
	m_bounding_box.build_from_triangles(
		(const unsigned char*)points,
		(size_t)m_layout.position_offset(),
		(size_t)m_layout.m_input_layout.m_size,
		vsize / m_layout.m_input_layout.m_size,
		indexs,
		isize
	);
	//enable culling
	m_support_culling = true;
	//end
	return true;
}

bool mesh::compute_bounding_box(const byte* points, size_t vsize)
{
	if (!m_layout.has_a_position()) return false;
	//compute box
	if (m_layout.m_draw_mode == GL_TRIANGLES)
	{
		m_bounding_box.build_from_sequenzial_triangles(
			(const unsigned char*)points,
			(size_t)m_layout.position_offset(),
			(size_t)m_layout.m_input_layout.m_size,
			vsize / m_layout.m_input_layout.m_size
		);
	}
	else
	{
		m_bounding_box.build_from_points(
			(const unsigned char*)points,
			(size_t)m_layout.position_offset(),
			(size_t)m_layout.m_input_layout.m_size,
			vsize / m_layout.m_input_layout.m_size
		);
	}
	//enable culling
	m_support_culling = true;
	//end
	return true;
}

void mesh::set_bounding_box(const obb& box)
{
	m_bounding_box = box;
	m_support_culling = true;
}

void mesh::disable_support_culling()
{
	m_support_culling = false;
}

void mesh::draw()
{
    //bind buffer
    glBindBuffer (GL_ARRAY_BUFFER, m_bvertex);
    
    //enable all attribute
    for(const input& data : m_layout.m_input_layout.m_fields)
    {
        glEnableVertexAttribArray(data.m_attribute);
        glVertexAttribPointer (data.m_attribute,
                               data.m_strip,
                               GL_FLOAT,
                               GL_FALSE,
							   m_layout.m_input_layout.m_size,
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
    for(const input& data : m_layout.m_input_layout.m_fields)
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

void mesh::build_index(const unsigned int* indexs, size_t size)
{
	//save size
	m_bindex_size = (unsigned int)size;
	//create buffer
	glGenBuffers(1, &m_bindex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bindex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), indexs, m_layout.m_buffer_mode);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void mesh::build_vertex(const byte* points, size_t size)
{
	//save size
	m_bvertex_size = (unsigned int)size;
	//create buffer
	glGenBuffers(1, &m_bvertex);
	glBindBuffer(GL_ARRAY_BUFFER, m_bvertex);
	glBufferData(GL_ARRAY_BUFFER, size, points, m_layout.m_buffer_mode);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void vbo_mode_draw_to_copy(int mode,int& new_mode)
{
	switch (mode)
	{
		case GL_STATIC_DRAW:
		case GL_STATIC_READ:   new_mode = GL_STATIC_COPY; break;
		case GL_STREAM_DRAW:
		case GL_STREAM_READ:   new_mode = GL_STREAM_COPY; break;
		case GL_DYNAMIC_DRAW:
		case GL_DYNAMIC_READ:  new_mode = GL_DYNAMIC_COPY; break;
		default: 
			new_mode = mode; 
		break;
	}
}

static void vbo_copy(unsigned int vbo_in,unsigned int size, int b_mode, unsigned int& vbo_out)
{
	//alloc new buffer
	glGenBuffers(1, &vbo_out);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_out);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, b_mode);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//vbo_in copy into vbo_out
	glBindBuffer(GL_ARRAY_BUFFER, vbo_in);
	glBindBuffer(GL_COPY_WRITE_BUFFER, vbo_out);
	glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

component_ptr mesh::copy() const
{
#if 1
	return (((mesh*)this)->shared_from_this());
#else
	auto omesh = mesh::snew();
	//real copy
	omesh->m_bounding_box = m_bounding_box;
	omesh->m_support_culling = m_support_culling;
	omesh->m_range = m_range;
	omesh->m_layout = m_layout;
	omesh->m_bvertex_size = m_bvertex_size;
	omesh->m_bindex_size = m_bindex_size;
	omesh->m_material = m_material;
	//copy gpu buffers
	if (m_bindex||m_bvertex)
	{
		//DRAW to COPY
		vbo_mode_draw_to_copy(m_layout.m_buffer_mode, omesh->m_layout.m_buffer_mode);
		//copy index buffer
		if (m_bindex)
			vbo_copy(
				m_bindex,
				m_bindex_size * sizeof(unsigned int),
				omesh->m_layout.m_buffer_mode,
				omesh->m_bindex
			);
		//copy vertex buffer
		if (m_bvertex)
			vbo_copy(
				m_bvertex,
				m_bvertex_size,
				omesh->m_layout.m_buffer_mode,
				omesh->m_bvertex
			);
	}
	return omesh;
#endif
}