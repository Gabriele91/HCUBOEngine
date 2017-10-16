//
//  mesh.h
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <hcube/config.h>
#include <hcube/math/vector_math.h>
#include <hcube/core/smart_pointers.h>
#include <hcube/render/render.h>
#include <hcube/render/rendering_system.h>
#include <hcube/components/renderable.h>
#include <hcube/core/component_register.h>

namespace hcube
{
	class HCUBE_API mesh : public smart_pointers< mesh >, public renderable
	{

	public:
		//buffer mode
		enum buffer_type
		{
			STATIC = 0,
			STREAM = 1
		};
		//alias
		using byte = unsigned char;
		//mesh layout
		struct mesh_layout
		{
			context_input_layout_ptr m_input_layout{ nullptr };
			draw_type                m_draw_mode{ DRAW_TRIANGLES };
			buffer_type              m_buffer_mode{ STATIC };

			bool has_a_position() const;
			size_t position_offset() const;

			mesh_layout() {}
			mesh_layout(context_input_layout* layout,
				draw_type draw_mode = DRAW_TRIANGLES,
				buffer_type buffer_mode = STATIC)
			{
				m_input_layout = context_input_layout_ptr(layout, render::delete_IL),
					m_draw_mode = draw_mode;
				m_buffer_mode = buffer_mode;
			}
			mesh_layout(context_input_layout_ptr layout,
				draw_type draw_mode = DRAW_TRIANGLES,
				buffer_type buffer_mode = STATIC)
			{
				m_input_layout = layout,
					m_draw_mode = draw_mode;
				m_buffer_mode = buffer_mode;
			}

		};
		//draw range (no indexed)
		struct draw_range
		{
			unsigned int m_min{ 0 };
			unsigned int m_max{ 0 };

			draw_range() {}

			draw_range(unsigned int max)
			{
				m_max = max;
			}

			draw_range(unsigned int min, unsigned int max)
			{
				m_max = min;
				m_max = max;
			}
		};

		mesh() {}

		virtual ~mesh();

		mesh(mesh_layout& layout,
			const draw_range & draw_range,
			const std::vector< byte >& vertex,
			bool compute_obb = true)
		{
			build(layout, draw_range, vertex, compute_obb);
		}

		mesh(mesh_layout& layout,
			const std::vector< unsigned int >& indexs,
			const std::vector< byte >& vertex,
			bool compute_obb = true)
		{
			build(layout, indexs, vertex, compute_obb);
		}

		void build(mesh_layout& layout,
			const std::vector< unsigned int >& indexs,
			const std::vector< byte >& points,
			bool compute_obb = true);

		void build(mesh_layout& layout,
			const draw_range & draw_range,
			const std::vector< byte >& points,
			bool compute_obb = true);

		void build(mesh_layout& layout,
			const unsigned int* indexs,
			size_t isize,
			const byte* points,
			size_t vsize,
			bool compute_obb = true);

		void build(mesh_layout& layout,
			const draw_range & draw_range,
			const byte* points,
			size_t size,
			bool compute_obb = true);

		void set_bounding_box(const obb& box);

		void disable_support_culling();

		void draw(rendering_system& rsystem, entity::ptr view);

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
		void build_index(const unsigned int* indexs, size_t size);
		void build_vertex(const byte* points, size_t size);

		draw_range   m_range;
		mesh_layout  m_layout;
		unsigned int m_bvertex_size{ 0 };
		unsigned int m_bindex_size{ 0 };
		context_index_buffer*  m_bindex{ nullptr };
		context_vertex_buffer* m_bvertex{ nullptr };

	};
}