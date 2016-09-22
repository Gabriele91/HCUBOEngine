//
//  mesh.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <mesh.h>
#include <OpenGL4.h>

namespace hcube
{

	bool mesh::mesh_layout::has_a_position() const
	{
		return render::has_a_position_IL(m_input_layout.get());
	}

	size_t mesh::mesh_layout::position_offset() const
	{
		return render::position_offset_IL(m_input_layout.get());
	}


	mesh::~mesh()
	{
		destoy();
	}

	void mesh::build(mesh_layout& layout,
		const std::vector< unsigned int >& indexs,
		const std::vector< byte >& vertex,
		bool compute_obb)
	{
		destoy();
		m_layout = std::move(layout);
		build_index(indexs);
		build_vertex(vertex);
		if (compute_obb) compute_bounding_box(indexs.data(), indexs.size(), vertex.data(), vertex.size());
	}

	void mesh::build(mesh_layout& layout,
		const draw_range&  range,
		const std::vector< byte >& vertex,
		bool compute_obb)
	{
		destoy();
		m_layout = layout;
		m_range = range;
		build_vertex(vertex);
		if (compute_obb) compute_bounding_box(vertex.data(), vertex.size());
	}

	void mesh::build(mesh_layout& layout,
		const  unsigned int* indexs,
		size_t isize,
		const byte* vertex,
		size_t vsize,
		bool compute_obb)
	{
		destoy();
		m_layout = layout;
		build_index(indexs, isize);
		build_vertex(vertex, vsize);
		if (compute_obb) compute_bounding_box(indexs, isize, vertex, vsize);
	}

	void mesh::build(mesh_layout& layout,
		const draw_range & range,
		const byte* vertex,
		size_t size,
		bool compute_obb)
	{
		destoy();
		m_layout = layout;
		m_range = range;
		build_vertex(vertex, size);
		if (compute_obb) compute_bounding_box(vertex, size);
	}

	bool mesh::compute_bounding_box(const unsigned int* indexs,
		size_t isize,
		const byte* points,
		size_t vsize)
	{
		if (!m_layout.has_a_position()) return false;
		//define obb
		obb bounding_box;
		//compute box
		bounding_box.build_from_triangles
		(
			(const unsigned char*)points,
			(size_t)m_layout.position_offset(),
			(size_t)render::size_IL(m_layout.m_input_layout.get()),
			vsize / render::size_IL(m_layout.m_input_layout.get()),
			indexs,
			isize
		);
		//enable culling
		set_bounding_box(bounding_box);
		//end
		return true;
	}

	bool mesh::compute_bounding_box(const byte* points, size_t vsize)
	{
		if (!m_layout.has_a_position()) return false;
		//define obb
		obb bounding_box;
		//compute box
		if (m_layout.m_draw_mode == GL_TRIANGLES)
		{
			bounding_box.build_from_sequenzial_triangles(
				(const unsigned char*)points,
				(size_t)m_layout.position_offset(),
				(size_t)render::size_IL(m_layout.m_input_layout.get()),
				vsize / render::size_IL(m_layout.m_input_layout.get())
			);
		}
		else
		{
			bounding_box.build_from_points(
				(const unsigned char*)points,
				(size_t)m_layout.position_offset(),
				(size_t)render::size_IL(m_layout.m_input_layout.get()),
				vsize / render::size_IL(m_layout.m_input_layout.get())
			);
		}
		//enable culling
		set_bounding_box(bounding_box);
		//end
		return true;
	}

	void mesh::set_bounding_box(const obb& box)
	{
		renderable::set_bounding_box(box);
		renderable::set_support_culling(true);
	}

	void mesh::disable_support_culling()
	{
		renderable::set_support_culling(false);
	}

	void mesh::draw()
	{
		//bind buffer
		render::bind_VBO(m_bvertex);
		render::bind_IL(m_layout.m_input_layout.get());

		if (m_bindex)
		{
			//set index
			render::bind_IBO(m_bindex);
			//draw
			render::draw_elements(m_layout.m_draw_mode, m_bindex_size);
			//unbind
			render::unbind_IBO(m_bindex);
		}
		else
		{
			//draw
			render::draw_arrays(m_layout.m_draw_mode, m_range.m_min, m_range.m_max);
		}

		//disable all attribute
		render::unbind_IL(m_layout.m_input_layout.get());
		render::unbind_VBO(m_bvertex);

	}

	void mesh::destoy()
	{
		if (m_bindex)  render::delete_IBO(m_bindex);
		if (m_bvertex) render::delete_VBO(m_bvertex);
		m_bvertex_size = 0;
		m_bindex_size = 0;
	}

	void mesh::build_index(const std::vector< unsigned int >& indexs)
	{
		//save size
		m_bindex_size = (unsigned int)indexs.size();
		//create buffer
		if (m_layout.m_buffer_mode == STATIC)
		{
			m_bindex = render::create_IBO(indexs.data(), indexs.size());
		}
		else
		{
			m_bindex = render::create_stream_IBO(indexs.data(), indexs.size());
		}
	}

	void mesh::build_vertex(const std::vector< byte >& points)
	{
		//save size
		m_bvertex_size = (unsigned int)points.size();
		//n point
		size_t stride = render::size_IL(m_layout.m_input_layout.get());
		size_t count_points = points.size() / stride;
		//create buffer
		if (m_layout.m_buffer_mode == STATIC)
		{
			m_bvertex = render::create_VBO(points.data(), stride, count_points);
		}
		else
		{
			m_bvertex = render::create_stream_VBO(points.data(), stride, count_points);
		}
	}

	void mesh::build_index(const unsigned int* indexs, size_t size)
	{
		//save size
		m_bindex_size = (unsigned int)size;
		//create buffer
		if (m_layout.m_buffer_mode == STATIC)
		{
			m_bindex = render::create_IBO(indexs, size);
		}
		else
		{
			m_bindex = render::create_stream_IBO(indexs, size);
		}

	}

	void mesh::build_vertex(const byte* points, size_t size)
	{
		//save size
		m_bvertex_size = (unsigned int)size;
		//n point
		size_t stride = render::size_IL(m_layout.m_input_layout.get());
		size_t count_points = size / stride;
		//create buffer
		if (m_layout.m_buffer_mode == STATIC)
		{
			m_bvertex = render::create_VBO(points, stride, count_points);
		}
		else
		{
			m_bvertex = render::create_stream_VBO(points, stride, count_points);
		}
	}

	component_ptr mesh::copy() const
	{
		return (((mesh*)this)->shared_from_this());
	}
}