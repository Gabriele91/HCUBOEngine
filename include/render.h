//
//  render.h
//  HCubo
//
//  Created by Gabriele on 14/08/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <vector_math.h>

namespace hcube
{
	class context_texture;
	class context_render_target;
	class context_vertex_buffer;
	class context_index_buffer;
	class context_input_layout;

	using context_texture_ptr = std::shared_ptr< context_texture >;
	using context_render_target_ptr = std::shared_ptr< context_render_target >;
	using context_vertex_buffer_ptr = std::shared_ptr< context_vertex_buffer >;
	using context_index_buffer_ptr = std::shared_ptr< context_index_buffer >;
	using context_input_layout_ptr = std::shared_ptr< context_input_layout >;

	enum render_driver
	{
		DR_OPENGL,
		DR_DIRECTX
	};

	enum cullface_type
	{
		CF_DISABLE = 0,
		CF_FRONT,
		CF_BACK,
		CF_FRONT_AND_BACK
	};

	enum draw_type
	{
		DRAW_POINTS,
		DRAW_LINES,
		DRAW_LINE_LOOP,
		DRAW_TRIANGLES,
		DRAW_TRIANGLE_STRIP
	};

	enum blend_type
	{
		BLEND_ZERO,
		BLEND_ONE,

		BLEND_ONE_MINUS_DST_COLOR,
		BLEND_ONE_MINUS_DST_ALPHA,
		BLEND_ONE_MINUS_SRC_COLOR,
		BLEND_ONE_MINUS_SRC_ALPHA,

		BLEND_DST_COLOR,
		BLEND_DST_ALPHA,

		BLEND_SRC_COLOR,
		BLEND_SRC_ALPHA,
		BLEND_SRC_ALPHA_SATURATE
	};

	enum texture_format
	{
		////////////////////
		//RGBA
		TF_RGBA8,
		//uint
		TF_RGBA16UI,
		TF_RGBA32UI,
		//int
		TF_RGBA16I,
		TF_RGBA32I,
		//float
		TF_RGBA16F,
		TF_RGBA32F,
		////////////////////
		//RGB
		TF_RGB8,
		//uint
		TF_RGB16UI,
		TF_RGB32UI,
		//int
		TF_RGB16I,
		TF_RGB32I,
		//float
		TF_RGB16F,
		TF_RGB32F,
		////////////////////
		//RG
		TF_RG8,
		//uint
		TF_RG16UI,
		TF_RG32UI,
		//int
		TF_RG16I,
		TF_RG32I,
		//float
		TF_RG16F,
		TF_RG32F,
		////////////////////
		//R
		TF_R8,
		//uint
		TF_R16UI,
		TF_R32UI,
		//int
		TF_R16I,
		TF_R32I,
		//float
		TF_R16F,
		TF_R32F,
		////////////////////
		TF_RGB565,
		TF_RGB5A1,
		TF_RGBA4,
		////////////////////
		TF_DEPTH16_STENCIL8,
		TF_DEPTH24_STENCIL8,
		TF_DEPTH32_STENCIL8,
		TF_DEPTH_COMPONENT16,
		TF_DEPTH_COMPONENT24,
		TF_DEPTH_COMPONENT32,
	};

	enum texture_type
	{
		TT_R,
		TT_RG,
		TT_RGB,
		TT_RGBA,
		TT_DEPTH,
		TT_DEPTH_STENCIL
	};

	enum texture_type_format
	{
		TTF_FLOAT,
		TTF_UNSIGNED_BYTE,
		TTF_UNSIGNED_SHORT,
		TTF_UNSIGNED_INT
	};

	enum texture_mag_filter_type
	{
		TMAG_NEAREST,
		TMAG_LINEAR
	};

	enum texture_min_filter_type
	{
		TMIN_NEAREST,
		TMIN_NEAREST_MIPMAP_NEAREST,
		TMIN_NEAREST_MIPMAP_LINEAR,
		TMIN_LINEAR,
		TMIN_LINEAR_MIPMAP_NEAREST,
		TMIN_LINEAR_MIPMAP_LINEAR,
	};


	enum texture_edge_type
	{
		TEDGE_CLAMP,
		TEDGE_REPEAT
	};

	enum render_target_type
	{
		RT_COLOR,
		RT_DEPTH,
		RT_DEPTH_STENCIL,
		RT_STENCIL
	};



	enum attribute_type
	{
		//POSITION TRANSFORM
		ATT_POSITIONT = 0,

		ATT_NORMAL0 = 1,
		ATT_TEXCOORD0 = 2,
		ATT_TANGENT0 = 3,
		ATT_BINORMAL0 = 4,
		ATT_COLOR0 = 5,

		//POSITION 0
		ATT_POSITION0 = 6,

		ATT_NORMAL1 = 7,
		ATT_TEXCOORD1 = 8,
		ATT_TANGENT1 = 9,
		ATT_BINORMAL1 = 10,
		ATT_COLOR1 = 11,

		//POSITION 1
		ATT_POSITION1 = 12,

		ATT_NORMAL2 = 13,
		ATT_TEXCOORD2 = 14,
		ATT_TANGENT2 = 15,
		ATT_BINORMAL2 = 16,
		ATT_COLOR2 = 17
	};

	enum attribute_strip_type
	{
		AST_FLOAT,
		AST_FLOAT2,
		AST_FLOAT3,
		AST_FLOAT4,

		AST_INT,
		AST_INT2,
		AST_INT3,
		AST_INT4,

		AST_UINT,
		AST_UINT2,
		AST_UINT3,
		AST_UINT4,

		AST_TLESS,
		AST_TLESS2,
		AST_TLESS3,
		AST_TLESS4,
	};

	enum depth_func_type
	{
		DT_NEVER,
		DT_LESS,          // <
		DT_GREATER,       // >
		DT_EQUAL,         // ==
		DT_LESS_EQUAL,    // <=
		DT_GREATER_EQUAL, // >=
		DT_NOT_EQUAL,     // !=
		DT_ALWAYS
	};

	enum depth_mode
	{
		DM_DISABLE,
		DM_ENABLE_AND_WRITE,
		DM_ENABLE_ONLY_READ
	};

	struct target_field
	{
		context_texture*   m_texture;
		render_target_type m_type;
	};

	struct attribute
	{
		attribute_type       m_attribute;
		attribute_strip_type m_strip;
		size_t               m_offset;

		attribute(attribute_type       attribute,
			attribute_strip_type strip,
			size_t               offset)
		{
			m_attribute = attribute;
			m_strip = strip;
			m_offset = offset;
		}

		bool is_position_transform() const
		{
			return m_attribute == ATT_POSITIONT;
		}

		size_t components() const
		{
			int i_strip = (int)m_strip;
			return (size_t)((i_strip % 4) + 1);
		}

		size_t size() const
		{
			return components() * 4;
		}
	};

	class attribute_list
	{

	public:

		using raw_attribute_list = std::vector<attribute>;

		attribute_list(const std::initializer_list<attribute>& atts) :m_size(0)
		{
			for (const auto& att : atts)
			{
				push(att);
			}
		}

		attribute_list(size_t size, const std::initializer_list<attribute>& atts) :m_size(size)
		{
			for (const auto& att : atts)
			{
				m_list.push_back(att);
			}
		}

		void push(const attribute& attr)
		{
			m_list.push_back(attr);
			m_size += attr.size();
		}

		size_t size() const
		{
			return m_size;
		}

		size_t count_attribute() const
		{
			return m_list.size();
		}

		raw_attribute_list::iterator begin() { return m_list.begin(); }
		raw_attribute_list::iterator end() { return m_list.end(); }
		raw_attribute_list::const_iterator begin() const { return m_list.begin(); }
		raw_attribute_list::const_iterator end() const { return m_list.end(); }


	protected:

		raw_attribute_list m_list;
		size_t             m_size;

	};


	struct viewport_state
	{

		vec4 m_viewport;
		//costructors
		viewport_state() {}
		viewport_state(const vec4& vp) : m_viewport(vp) {}
		//operators
		bool operator==(const viewport_state& vs) const
		{
			return m_viewport == vs.m_viewport;
		}
		bool operator!=(const viewport_state& vs) const
		{
			return m_viewport != vs.m_viewport;
		}

	};

	struct blend_state
	{
		bool       m_enable;
		blend_type m_src, m_dst;
		//constructor
		blend_state() : m_enable(false) {}
		blend_state(blend_type src, blend_type dst) :m_src(src), m_dst(dst), m_enable(true) {}
		//operators
		bool operator==(const blend_state& bs) const
		{
			return m_src == bs.m_src && m_dst == bs.m_dst && m_enable == bs.m_enable;
		}
		bool operator!=(const blend_state& bs) const
		{
			return m_src != bs.m_src || m_dst != bs.m_dst || m_enable != bs.m_enable;
		}
	};

	struct cullface_state
	{
		//face cull
		cullface_type m_cullface;
		//cullface
		cullface_state(cullface_type cullface = CF_DISABLE) : m_cullface(cullface) {}
		//operators
		bool operator==(const cullface_state& cl)const
		{
			return m_cullface == cl.m_cullface;
		}
		bool operator!=(const cullface_state& cl)const
		{
			return m_cullface != cl.m_cullface;
		}
		//cast operator
		operator int() const
		{
			return (int)m_cullface;
		}
		operator cullface_type () const
		{
			return m_cullface;
		}

	};


	struct depth_buffer_state
	{
		//value
		depth_mode      m_mode;
		depth_func_type m_type;
		//zbuffer
		depth_buffer_state(depth_func_type type, depth_mode mode = DM_ENABLE_AND_WRITE) : m_mode(mode), m_type(type) {}
		depth_buffer_state(depth_mode mode = DM_ENABLE_AND_WRITE) : m_mode(mode), m_type(DT_LESS) {}
		//operators
		bool operator==(const depth_buffer_state& zb)const
		{
			return m_mode == zb.m_mode && m_type == zb.m_type;
		}
		bool operator!=(const depth_buffer_state& zb)const
		{
			return m_mode != zb.m_mode || m_type != zb.m_type;
		}
		//cast operator
		operator bool() const
		{
			return m_mode;
		}

	};

	struct clear_color_state
	{
		//color
		vec4 m_color;
		//color
		clear_color_state() :m_color(0, 0, 0, 0) {}
		clear_color_state(const vec4& color) :m_color(color) {}
		//operators
		bool operator==(const clear_color_state& c)const
		{
			return m_color == c.m_color;
		}
		bool operator!=(const clear_color_state& c)const
		{
			return m_color != c.m_color;
		}
		//cast operator
		operator vec4& ()
		{
			return m_color;
		}
		operator const vec4& () const
		{
			return m_color;
		}

	};


	struct render_state
	{
		clear_color_state  m_clear_color;
		viewport_state     m_viewport;
		cullface_state     m_cullface;
		depth_buffer_state m_depth;
		blend_state        m_blend;

		bool operator == (const render_state& rs) const
		{
			return
				m_cullface == rs.m_cullface
				&& m_viewport == rs.m_viewport
				&& m_clear_color == rs.m_clear_color
				&& m_depth == rs.m_depth
				&& m_blend == rs.m_blend;
		}
		bool operator != (const render_state& rs) const
		{
			return
				m_cullface != rs.m_cullface
				&& m_viewport != rs.m_viewport
				&& m_clear_color != rs.m_clear_color
				&& m_depth != rs.m_depth
				&& m_blend != rs.m_blend;
		}
	};

	enum mapping_type
	{
		MAP_WRITE,
		MAP_READ,
		MAP_WRITE_AND_READ
	};

	struct texture_raw_data_information
	{
		texture_format		m_format;
		unsigned int		m_width;
		unsigned int		m_height;
		const unsigned char*m_bytes;
		texture_type		m_type;
		texture_type_format m_type_format;
	};

	struct texture_gpu_data_information
	{
		texture_min_filter_type		 m_min_type;
		texture_mag_filter_type		 m_mag_type;
		texture_edge_type			 m_edge_s;
		texture_edge_type			 m_edge_t;
		texture_edge_type			 m_edge_r;
		bool						 m_build_mipmap;
		//cube texture
		texture_gpu_data_information
		(
			texture_min_filter_type		 min_type,
			texture_mag_filter_type		 mag_type,
			texture_edge_type			 edge_s,
			texture_edge_type			 edge_t,
			texture_edge_type			 edge_r,
			bool						 build_mipmap
		)
		{
			m_min_type = min_type;
			m_mag_type = mag_type;
			m_edge_s = edge_s;
			m_edge_t = edge_t;
			m_edge_r = edge_r;
			m_build_mipmap = build_mipmap;
		}
		//2D texture
		texture_gpu_data_information
		(
			texture_min_filter_type		 min_type,
			texture_mag_filter_type		 mag_type,
			texture_edge_type			 edge_s,
			texture_edge_type			 edge_t,
			bool						 build_mipmap
		)
		{
			m_min_type = min_type;
			m_mag_type = mag_type;
			m_edge_s = edge_s;
			m_edge_t = edge_t;
			m_build_mipmap = build_mipmap;
		}
	};

#define LIB_EXPORT
	namespace render
	{
		LIB_EXPORT render_driver get_render_driver();
		LIB_EXPORT void print_info();

		LIB_EXPORT bool init();
		LIB_EXPORT void close();

		LIB_EXPORT const clear_color_state& get_clear_color_state();
		LIB_EXPORT void set_clear_color_state(const clear_color_state& cf);
		LIB_EXPORT void clear();

		LIB_EXPORT const depth_buffer_state& get_depth_buffer_state();
		LIB_EXPORT void set_depth_buffer_state(const depth_buffer_state& cf);

		LIB_EXPORT const cullface_state& get_cullface_state();
		LIB_EXPORT void set_cullface_state(const cullface_state& cf);

		LIB_EXPORT const viewport_state& get_viewport_state();
		LIB_EXPORT void set_viewport_state(const viewport_state& vs);

		LIB_EXPORT const blend_state& get_blend_state();
		LIB_EXPORT void set_blend_state(const blend_state& bs);

		LIB_EXPORT const render_state& get_render_state();
		LIB_EXPORT void set_render_state(const render_state& rs);

		//BO
		LIB_EXPORT context_vertex_buffer* create_stream_VBO(const unsigned char* vbo, size_t stride, size_t n);
		LIB_EXPORT context_index_buffer* create_stream_IBO(const unsigned int* ibo, size_t size);
		LIB_EXPORT context_vertex_buffer* create_VBO(const unsigned char* vbo, size_t stride, size_t n);
		LIB_EXPORT context_index_buffer* create_IBO(const unsigned int* ibo, size_t size);

		LIB_EXPORT void update_steam_VBO(context_vertex_buffer* vbo, const unsigned char* vb, size_t n);
		LIB_EXPORT void update_steam_IBO(context_index_buffer* vbo, const unsigned int* ib, size_t n);

		LIB_EXPORT void bind_VBO(context_vertex_buffer*);
		LIB_EXPORT void bind_IBO(context_index_buffer*);

		LIB_EXPORT void unbind_VBO(context_vertex_buffer*);
		LIB_EXPORT void unbind_IBO(context_index_buffer*);

		LIB_EXPORT unsigned char* map_VBO(context_vertex_buffer*, size_t start, size_t n, mapping_type type);
		LIB_EXPORT void unmap_VBO(context_vertex_buffer*);

		LIB_EXPORT unsigned int*  map_IBO(context_index_buffer*, size_t start, size_t n, mapping_type type);
		LIB_EXPORT void unmap_IBO(context_index_buffer*);

		LIB_EXPORT void delete_VBO(context_vertex_buffer*&);
		LIB_EXPORT void delete_IBO(context_index_buffer*&);

		//draw
		LIB_EXPORT void draw_arrays(draw_type type, unsigned int n);
		LIB_EXPORT void draw_arrays(draw_type type, unsigned int start, unsigned int size);
		LIB_EXPORT void draw_elements(draw_type type, unsigned int n);

		//IL=Input Layaut
		LIB_EXPORT context_input_layout* create_IL(const attribute_list& atl);
		LIB_EXPORT size_t size_IL(const context_input_layout* layout);
		LIB_EXPORT bool   has_a_position_IL(const context_input_layout* layout);
		LIB_EXPORT size_t position_offset_IL(const context_input_layout* layout);
		LIB_EXPORT void delete_IL(context_input_layout*&);
		LIB_EXPORT void bind_IL(context_input_layout*);
		LIB_EXPORT void unbind_IL(context_input_layout* il);

		//depth
		LIB_EXPORT float get_depth(const vec2& pixel);
		//color
		LIB_EXPORT vec4 get_color(const vec2& pixel);

		//texture
		LIB_EXPORT context_texture* create_texture
		(
			const texture_raw_data_information& data,
			const texture_gpu_data_information& info
		);
		LIB_EXPORT context_texture* create_cube_texture
		(
			const texture_raw_data_information  data[6],
			const texture_gpu_data_information& info
		);
		LIB_EXPORT void bind_texture(context_texture*, int n);
		LIB_EXPORT void unbind_texture(context_texture*);
        LIB_EXPORT void unbind_texture(int n);
		LIB_EXPORT void delete_texture(context_texture*&);


		//target
		LIB_EXPORT context_render_target* create_render_target(const std::vector< target_field >& textures);
		LIB_EXPORT void enable_render_target(context_render_target*);
		LIB_EXPORT void disable_render_target(context_render_target*);
		LIB_EXPORT void delete_render_target(context_render_target*&);
		//copy target
		LIB_EXPORT void copy_target_to_target(
			const vec4& from_area,
			context_render_target* from,
			const vec4& to_area,
			context_render_target* to,
			render_target_type	mask
		);

		//debug
		LIB_EXPORT bool print_errors();

	};
}