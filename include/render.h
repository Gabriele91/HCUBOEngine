//
//  render.h
//  HCubo
//
//  Created by Gabriele on 14/08/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

class context_texture;
class context_render_target;
class context_vertex_buffer;
class context_index_buffer;
class context_input_layout;
class context_shader;

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
    RT_DEPTH_STENCIL
};

enum attribute_type
{
    //POSITIONT
    ATT_POSITIONT    = 0,
    //POSITION
    ATT_POSITION0    = 1,
    ATT_POSITION1,
    ATT_POSITION2,
    ATT_POSITION3,
    ATT_POSITION4,
    ATT_POSITION5,
    ATT_POSITION6,
    ATT_POSITION7,
    ATT_POSITION8,
    ATT_POSITION9,
    //COLOR
    ATT_COLOR0 = 20,
    ATT_COLOR1,
    ATT_COLOR2,
    ATT_COLOR3,
    ATT_COLOR4,
    ATT_COLOR5,
    ATT_COLOR6,
    ATT_COLOR7,
    ATT_COLOR8,
    ATT_COLOR9,
    //TEXCOORD
    ATT_TEXCOORD0 = 40,
    ATT_TEXCOORD1,
    ATT_TEXCOORD2,
    ATT_TEXCOORD3,
    ATT_TEXCOORD4,
    ATT_TEXCOORD5,
    ATT_TEXCOORD6,
    ATT_TEXCOORD7,
    ATT_TEXCOORD8,
    ATT_TEXCOORD9,
    //NORMAL
    ATT_NORMAL0 = 60,
    ATT_NORMAL1,
    ATT_NORMAL2,
    ATT_NORMAL3,
    ATT_NORMAL4,
    ATT_NORMAL5,
    ATT_NORMAL6,
    ATT_NORMAL7,
    ATT_NORMAL8,
    ATT_NORMAL9,
    //TANGENT
    ATT_TANGENT0 = 80,
    ATT_TANGENT1,
    ATT_TANGENT2,
    ATT_TANGENT3,
    ATT_TANGENT4,
    ATT_TANGENT5,
    ATT_TANGENT6,
    ATT_TANGENT7,
    ATT_TANGENT8,
    ATT_TANGENT9,
    //BINORMAL
    ATT_BINORMAL0 = 110,
    ATT_BINORMAL1,
    ATT_BINORMAL2,
    ATT_BINORMAL3,
    ATT_BINORMAL4,
    ATT_BINORMAL5,
    ATT_BINORMAL6,
    ATT_BINORMAL7,
    ATT_BINORMAL8,
    ATT_BINORMAL9,
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
    
    bool is_position_transform()
    {
        return m_attribute == ATT_POSITIONT;
    }
    
    size_t components() const
    {
        int i_strip = (int)m_strip;
        return (size_t)((i_strip % 4)+1);
    }
    
    size_t size() const
    {
        return components() * 4;
    }
};

class attribute_list
{
    
public:
    
    using raw_attribute_list = std::vector<attribute> ;
    
    attribute_list(const std::initializer_list<attribute>& atts):m_size(0)
    {
        for (const auto& att : atts)
        {
            push(att);
        }
    }
    
    attribute_list(size_t size,const std::initializer_list<attribute>& atts):m_size(size)
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
    raw_attribute_list::const_iterator begin() const{ return m_list.begin(); }
    raw_attribute_list::const_iterator end() const{ return m_list.end(); }
    
    
protected:
    
    raw_attribute_list m_list;
    size_t             m_size;

};


struct viewport_state
{
    
    glm::vec4 m_viewport;
    //costructors
    viewport_state(){}
    viewport_state(const glm::vec4& vp) : m_viewport(vp){}
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
    blend_state() : m_enable(false){}
    blend_state(blend_type src, blend_type dst) :m_src(src), m_dst(dst), m_enable(true){}
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
    cullface_state(cullface_type cullface = CF_DISABLE) : m_cullface(cullface){}
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
    bool            m_depth;
    depth_func_type m_type;
    //zbuffer
    depth_buffer_state(depth_func_type type,bool depth = true) : m_depth(depth), m_type(type){}
    depth_buffer_state(bool depth = true) : m_depth(depth), m_type(DT_LESS){}
    //operators
    bool operator==(const depth_buffer_state& zb)const
    {
        return m_depth == zb.m_depth && m_type == zb.m_type;
    }
    bool operator!=(const depth_buffer_state& zb)const
    {
        return m_depth != zb.m_depth || m_type != zb.m_type;
    }
    //cast operator
    operator bool() const
    {
        return m_depth;
    }
    
};

struct clear_color_state
{
    //color
    glm::vec4 m_color;
    //color
    clear_color_state():m_color(0,0,0,0){}
    clear_color_state(const glm::vec4& color):m_color(color){}
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
    operator glm::vec4& ()
    {
        return m_color;
    }
    operator const glm::vec4& () const
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
           m_cullface    == rs.m_cullface
        && m_viewport    == rs.m_viewport
        && m_clear_color == rs.m_clear_color
        && m_depth       == rs.m_depth
        && m_blend       == rs.m_blend;
    }
    bool operator != (const render_state& rs) const
    {
        return
           m_cullface    != rs.m_cullface
        && m_viewport    != rs.m_viewport
        && m_clear_color != rs.m_clear_color
        && m_depth       != rs.m_depth
        && m_blend       != rs.m_blend;
    }
};


namespace render
{
    static render_driver get_render_driver();
    
    static void init();
    static void close();
    
    static const clear_color_state& get_clear_color_state();
    static void set_clear_color_state(const clear_color_state& cf);
    static void clear();
    
    static const depth_buffer_state& get_depth_buffer_state();
    static void set_depth_buffer_state(const depth_buffer_state& cf);
    
    static const cullface_state& get_cullface_state();
    static void set_cullface_state(const cullface_state& cf);
    
    static const viewport_state& get_viewport_state();
    static void set_viewport_state(const viewport_state& vs);
    
    static const blend_state& get_blend_state();
    static void set_blend_state(const blend_state& bs);
    
    static const render_state& get_render_state();
    static void set_render_state(const render_state& rs);
    
    //BO
    static context_vertex_buffer* create_stream_VBO(const unsigned char* vbo, size_t stride, size_t n);
    static context_index_buffer* create_stream_IBO(const unsigned int* ibo, size_t size);
    static context_vertex_buffer* create_VBO(const unsigned char* vbo, size_t stride, size_t n);
    static context_index_buffer* create_IBO(const unsigned int* ibo, size_t size);
    
    static void update_steam_VBO(context_vertex_buffer* vbo, const unsigned char* vb, size_t n);
    static void update_steam_IBO(context_index_buffer* vbo, const unsigned int* ib, size_t n);
    
    static void bind_VBO(context_vertex_buffer*);
    static void bind_IBO(context_index_buffer*);
    
    static void delete_VBO(context_vertex_buffer*&);
    static void delete_IBO(context_index_buffer*&);
    
    //draw
    static void draw_arrays(draw_type type,unsigned int n);
    static void draw_arrays(draw_type type, unsigned int start,unsigned int size);
    static void draw_elements(draw_type type, unsigned int n);
    
    //IL=Input Layaut
    static context_input_layout* create_IL(const attribute_list& atl);
    static void delete_IL(context_input_layout*&);
    static void bind_IL(context_input_layout*);
    static void unbind_IL(context_input_layout* il);
    
    //depth
    static float get_depth(const glm::vec2& pixel);
    //color
    static glm::vec4 get_color(const glm::vec2& pixel);
    
    //texture
    static context_texture* create_texture(texture_type   type,
                                           unsigned int w,
                                           unsigned int h,
                                           void* byte,
                                           texture_format format,
                                           texture_min_filter_type min_type,
                                           texture_mag_filter_type mag_type,
                                           texture_edge_type       edge_s,
                                           texture_edge_type       edge_t);
    static void bind_texture(context_texture*, int n);
    static void unbind_texture(context_texture*);
    static void delete_texture(context_texture*&);
    
    //target
    static context_render_target* create_render_target(const std::vector< target_field >& textures);
    static void enable_render_target(context_render_target*);
    static void disable_render_target(context_render_target*);
    static void delete_render_target(context_render_target*&);
    
    //debug
    void print_errors();
    
};