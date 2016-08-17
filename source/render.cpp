//
//  render.cpp
//  HCubo
//
//  Created by Gabriele on 15/08/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <render.h>
#include <OpenGL4.h>
#include <iostream>
//TEXTURE
class context_texture
{
public:
    int          m_last_bind{ -1 };
    unsigned int m_width { 0 };
    unsigned int m_height{ 0 };
    unsigned int m_tbo   { 0 };
    
    context_texture(unsigned int width,unsigned int height):m_width(width), m_height(height){}
    
    virtual ~context_texture()
    {
        if (m_tbo) glDeleteTextures(1, &m_tbo);
    }
    
    inline void create_TBO()
    {
        glGenTextures(1, &m_tbo);
    }
    
    inline void enable_TBO(int n)
    {
        m_last_bind = n;
        glActiveTexture((GLenum)(GL_TEXTURE0+m_last_bind));
        glBindTexture(GL_TEXTURE_2D, m_tbo);
    }
    
    inline void disable_TBO()
    {
        if(m_last_bind >= 0)
        {
            glActiveTexture((GLenum)(GL_TEXTURE0+m_last_bind));
            glBindTexture(GL_TEXTURE_2D, (GLuint)0);
            m_last_bind = -1;
        }
    }
    
};

//BUFFER
class context_buffer
{
public:
    
    GLuint m_id_buffer;
    
    context_buffer(GLuint id=0):m_id_buffer(id){}
    
    inline operator GLuint() const
    {
        return m_id_buffer;
    }
    
    inline operator GLuint*()
    {
        return &m_id_buffer;
    }
    
    void gen_buffer()
    {
        glGenBuffers(1, &m_id_buffer);
    }
};

class context_vertex_buffer : public context_buffer
{
public:
    context_vertex_buffer(GLuint id=0):context_buffer(id){}
};

class context_index_buffer : public context_buffer
{
public:
    context_index_buffer(GLuint id=0):context_buffer(id){}
};

//INPUT LAYOUT
class context_input_layout
{
public:
    attribute_list m_list;
};

//FRAMES TARGET
class context_render_target
{
public:
    
    GLuint m_fbo{ 0 };
    
    virtual ~context_render_target()
    {
        if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
    }
    //fbo
    void gen_FBO()
    {
        //vbo
        glGenFramebuffers(1, &m_fbo);
    }
    
    void enable_FBO()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }
    
    void disable_FBO()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};
////////////////////
//     RENDER     //
////////////////////
namespace render
{
///////////////////////
//globals
render_state s_render_state;
GLuint       s_vao_attributes;
///////////////////////
    
bool init()
{
    
#ifdef _WIN32
    //enable glew experimental (OpenGL3/4)
    glewExperimental = GL_TRUE;
    //try to init glew (get OpenGL calls)
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Glew init fail" << std::endl;
        return false;
    }
    //clear OpenGL error by Glew init
    else while ((glGetError()) != GL_NO_ERROR);
#endif
    
#if defined(DIRECTX_MODE)
    //DirectX like topology
    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
    //DirectX like z buffer
    glDepthRange(0.0f,1.0f);
#endif
    //test
    render::print_errors();
    //attributes vao
    glGenVertexArrays(1, &s_vao_attributes);
    glBindVertexArray(s_vao_attributes);
    //set default state
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glViewport(0,0,0,0);
    glClearColor(0.1, 0.5, 1.0, 1.0);
    //save cullface state
    s_render_state.m_cullface.m_cullface = CF_BACK;
    //save depth state
    s_render_state.m_depth.m_depth = true;
    s_render_state.m_depth.m_type  = DT_LESS;
    //save blend state
    s_render_state.m_blend.m_enable = false;
    s_render_state.m_blend.m_src    = BLEND_ONE;
    s_render_state.m_blend.m_dst    = BLEND_ZERO;
    //save viewport state
    s_render_state.m_viewport.m_viewport = glm::vec4(0,0,0,0);
    //save clear color state
    s_render_state.m_clear_color.m_color = glm::vec4(0.1, 0.5, 1.0, 1.0);
    //clear all errors
    render::print_errors();
	//return
	return true;
}

void print_info()
{
    std::string renderer = (const char*)glGetString (GL_RENDERER);
    std::string version  = (const char*)glGetString (GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;
}

void close()
{
    if (s_vao_attributes) glDeleteVertexArrays(1, &s_vao_attributes);
}

render_driver get_render_driver()
{
    return DR_OPENGL;
}

const clear_color_state& get_clear_color_state()
{
    return s_render_state.m_clear_color;
}

void set_clear_color_state(const clear_color_state& clear_color)
{
    if(s_render_state.m_clear_color != clear_color)
    {
        s_render_state.m_clear_color = clear_color;
        glClearColor(clear_color.m_color.r,clear_color.m_color.g,clear_color.m_color.b,clear_color.m_color.a);
    }
}

void clear()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}
    
const depth_buffer_state& get_depth_buffer_state()
{
    return s_render_state.m_depth;
}

static GLenum get_depth_func(depth_func_type type)
{
    switch (type)
    {
        default:
        case DT_LESS:         return GL_LESS;
        case DT_NEVER:        return GL_NEVER;
        case DT_EQUAL:        return GL_EQUAL;
        case DT_GREATER:      return GL_GREATER;
        case DT_NOT_EQUAL:    return GL_NOTEQUAL;
        case DT_LESS_EQUAL:   return GL_LEQUAL;
        case DT_GREATER_EQUAL:return GL_GEQUAL;
        case DT_ALWAYS:       return GL_ALWAYS;
    }
}

void set_depth_buffer_state(const depth_buffer_state& depth)
{

    if(s_render_state.m_depth != depth)
    {
        s_render_state.m_depth = depth;
        if(depth.m_depth)  glEnable(GL_DEPTH_TEST);
        else               glDisable(GL_DEPTH_TEST);
        glDepthFunc(get_depth_func(depth.m_type));
    }
}

const cullface_state& get_cullface_state()
{
    return s_render_state.m_cullface;
}

void set_cullface_state(const cullface_state& cfs)
{
    
    if(s_render_state.m_cullface != cfs)
    {
        s_render_state.m_cullface = cfs;
        
        if(cfs.m_cullface == CF_DISABLE)
        {
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
            switch (cfs.m_cullface)
            {
                case CF_BACK:           glCullFace(GL_BACK);           break;
                case CF_FRONT:          glCullFace(GL_FRONT);          break;
                case CF_FRONT_AND_BACK: glCullFace(GL_FRONT_AND_BACK); break;
                default: break;
            }
        }
    }
}
    
const viewport_state& get_viewport_state() 
{
    //return
    return s_render_state.m_viewport;
}

void  set_viewport_state(const viewport_state& vs)
{
    if(s_render_state.m_viewport != vs)
    {
        s_render_state.m_viewport = vs;
        glViewport(vs.m_viewport.x,vs.m_viewport.y,vs.m_viewport.z,vs.m_viewport.w);
    }
}

const blend_state& get_blend_state()
{
    return s_render_state.m_blend;
}

static GLenum get_blend_type(blend_type type)
{
    switch (type)
    {
        default:
        case BLEND_ZERO: return GL_ZERO;
        case BLEND_ONE:  return GL_ONE;
        
        case BLEND_ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
        case BLEND_ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
        case BLEND_ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
        case BLEND_ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_COLOR;
        
        case BLEND_DST_COLOR: return GL_DST_COLOR;
        case BLEND_DST_ALPHA: return GL_DST_ALPHA;
        
        case BLEND_SRC_COLOR: return GL_SRC_COLOR;
        case BLEND_SRC_ALPHA: return GL_SRC_ALPHA;
        case BLEND_SRC_ALPHA_SATURATE: return GL_SRC_ALPHA_SATURATE;
    }
}

void  set_blend_state(const blend_state& bs)
{
    if(s_render_state.m_blend != bs)
    {
        //save blend
        s_render_state.m_blend = bs;
        //set state
        if (bs.m_enable)
        {
            glEnable(GL_BLEND);
            glBlendFunc(get_blend_type(bs.m_src), get_blend_type(bs.m_dst));
        }
        else
        {
            glBlendFunc(GL_ONE, GL_ZERO);
            glDisable(GL_BLEND);
            //save blend
            s_render_state.m_blend.m_src    = BLEND_ONE;
            s_render_state.m_blend.m_dst    = BLEND_ZERO;
            s_render_state.m_blend.m_enable = false;
        }
    }
}

const render_state& get_render_state()
{
    return s_render_state;
}

void set_render_state(const render_state& rs)
{
    set_clear_color_state(rs.m_clear_color);
    set_cullface_state(rs.m_cullface);
    set_viewport_state(rs.m_viewport);
    set_depth_buffer_state(rs.m_depth);
    set_blend_state(rs.m_blend);
}

//projection matrix
/*
VBO & IBO
*/
context_vertex_buffer* create_stream_VBO(const unsigned char* vbo, size_t stride, size_t n)
{
    auto ptr=new context_vertex_buffer();
    ptr->gen_buffer();
    glBindBuffer(GL_ARRAY_BUFFER, *ptr);
    glBufferData(GL_ARRAY_BUFFER, stride*n, vbo, GL_STREAM_DRAW);
    return ptr;
}

context_index_buffer* create_stream_IBO(const unsigned int* ibo, size_t size)
{
    auto ptr=new context_index_buffer();
    ptr->gen_buffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ptr);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*size, ibo, GL_STREAM_DRAW);
    return ptr;
}


context_vertex_buffer* create_VBO(const unsigned char* vbo, size_t stride, size_t n)
{
    auto ptr=new context_vertex_buffer();
    ptr->gen_buffer();
    glBindBuffer(GL_ARRAY_BUFFER, *ptr);
    glBufferData(GL_ARRAY_BUFFER, stride*n, vbo, GL_STATIC_DRAW);
    return ptr;
}

context_index_buffer* create_IBO(const unsigned int* ibo, size_t size)
{
    auto ptr=new context_index_buffer();
    ptr->gen_buffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ptr);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*size, ibo, GL_STATIC_DRAW);
    return ptr;
}

void update_steam_VBO(context_vertex_buffer* vbo, const unsigned char* data, size_t size)
{
    //get state
    GLint lastbind=0;
    glGetIntegerv(GL_ARRAY_BUFFER, &lastbind);
    //change
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    //restore
    glBindBuffer(GL_ARRAY_BUFFER, lastbind);
}

void update_steam_IBO(context_index_buffer* ibo, const unsigned int* data, size_t size){
    //get state
    GLint lastbind;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER, &lastbind);
    //change
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size*sizeof(unsigned int), data);
    //restore
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lastbind);
}

void bind_VBO(context_vertex_buffer* vbo)
{
    glBindBuffer(GL_ARRAY_BUFFER,*vbo);
}

void bind_IBO(context_index_buffer* ibo)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,*ibo);
}

void unbind_VBO(context_vertex_buffer* vbo)
{
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void unbind_IBO(context_index_buffer* ibo)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

GLbitfield get_mapping_type(mapping_type type)
{
	switch (type)
	{
		default:
		case MAP_WRITE_AND_READ: return GL_MAP_WRITE_BIT | GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT; break;
		case MAP_WRITE: return GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;
		case MAP_READ:  return GL_MAP_READ_BIT  | GL_MAP_UNSYNCHRONIZED_BIT;
	}
}

unsigned char* map_VBO(context_vertex_buffer* vbo, size_t start, size_t n, mapping_type type)
{
	bind_VBO(vbo);
	return (unsigned char*)glMapBufferRange(GL_ARRAY_BUFFER, start, n, get_mapping_type(type));
}

void unmap_VBO(context_vertex_buffer* vbo)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
	unbind_VBO(vbo);
}

unsigned int*  map_IBO(context_index_buffer* ibo, size_t start, size_t n, mapping_type type)
{
	bind_IBO(ibo);
	return (unsigned int*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, start * sizeof(unsigned int), n * sizeof(unsigned int), get_mapping_type(type));
}

void unmap_IBO(context_index_buffer* ibo)
{
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	unbind_IBO(ibo);
}


void delete_VBO(context_vertex_buffer*& vbo)
{
    glDeleteBuffers(1, *vbo);
    delete vbo;
    vbo = nullptr;
}

void delete_IBO(context_index_buffer*& ibo)
{
    glDeleteBuffers(1, *ibo);
    delete ibo;
    ibo = nullptr;
}
/*
 Draw
 */
inline static GLuint get_draw_type(draw_type type)
{
    switch (type)
    {
        case DRAW_POINTS:         return GL_POINTS; break;
        case DRAW_LINES:          return GL_LINES;  break;
        case DRAW_LINE_LOOP:      return GL_LINE_LOOP;  break;
        case DRAW_TRIANGLES:      return GL_TRIANGLES; break;
        case DRAW_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;  break;
        default: return 0; break;
    }
}

void draw_arrays(draw_type type, unsigned int n)
{
    glDrawArrays(get_draw_type(type), 0, n);
}

void draw_arrays(draw_type type, unsigned int start, unsigned int size)
{
    glDrawArrays(get_draw_type(type), start, size);
}

void draw_elements(draw_type type, unsigned int n)
{
    glDrawElements(get_draw_type(type), n, GL_UNSIGNED_INT, (void*)NULL);
}

/*
 InputLayout
*/

context_input_layout* create_IL(const attribute_list& atl)
{
    return new context_input_layout{ atl };
}

size_t size_IL(const context_input_layout* layout)
{
    return layout->m_list.size();
}

bool has_a_position_IL(const context_input_layout* layout)
{
    
    for (auto& in : layout->m_list)
        if (in.is_position_transform())
            return true;
    return false;
}

size_t position_offset_IL(const context_input_layout* layout)
{
    for (auto& in : layout->m_list)
        if (in.is_position_transform())
            return in.m_offset;
    return ~((size_t)0);
}

static GLenum type_component(attribute_strip_type type)
{
    
    switch (type)
    {
        default:
        case AST_FLOAT:  return GL_FLOAT;
        case AST_FLOAT2: return GL_FLOAT;
        case AST_FLOAT3: return GL_FLOAT;
        case AST_FLOAT4: return GL_FLOAT;
            
        case AST_INT:  return GL_INT;
        case AST_INT2: return GL_INT;
        case AST_INT3: return GL_INT;
        case AST_INT4: return GL_INT;
            
        case AST_UINT:  return GL_UNSIGNED_INT;
        case AST_UINT2: return GL_UNSIGNED_INT;
        case AST_UINT3: return GL_UNSIGNED_INT;
        case AST_UINT4: return GL_UNSIGNED_INT;
            
        case AST_TLESS:  return GL_LESS;
        case AST_TLESS2: return GL_LESS;
        case AST_TLESS3: return GL_LESS;
        case AST_TLESS4: return GL_LESS;
            
    }
}

void bind_IL(context_input_layout* layout)
{
    for(const attribute& data : layout->m_list)
    {
        glEnableVertexAttribArray(data.m_attribute);
        glVertexAttribPointer (data.m_attribute,
                               (GLint)data.components(),
                               type_component(data.m_strip),
                               GL_FALSE,
                               (GLuint)layout->m_list.size(),
                               ((char *)NULL + (data.m_offset)));
    }
}

void unbind_IL(context_input_layout* layout)
{
    for(const attribute& data : layout->m_list)
    {
        glDisableVertexAttribArray(data.m_attribute);
    }
}

void delete_IL(context_input_layout*& il)
{
    delete  il;
    il = nullptr;
}
    
//DEPTH
float get_depth(const glm::vec2& pixel)
{
    float depth;
    glReadPixels(pixel.x, pixel.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    return depth;
}

//RGBA
glm::vec4 get_color(const glm::vec2& pixel)
{
    glm::vec4 rgba;
    glReadPixels(pixel.x, pixel.y, 1, 1, GL_RGBA, GL_FLOAT, &rgba);
    return rgba;
}
/*
 Textures
 */
inline const GLenum get_texture_format(texture_format type)
{
    switch (type)
    {
        //RGBA
        case TF_RGBA8: return GL_RGBA8; break;
        //uint
        case TF_RGBA16UI: return GL_RGBA16UI; break;
        case TF_RGBA32UI: return GL_RGBA32UI; break;
        //int
        case TF_RGBA16I: return GL_RGBA16I; break;
        case TF_RGBA32I: return GL_RGBA32I; break;
        //float
        case TF_RGBA16F: return GL_RGBA16F; break;
        case TF_RGBA32F: return GL_RGBA32F; break;
        ////////////////////
        //RGB
        case TF_RGB8: return GL_RGBA8; break;
        //uint
        case TF_RGB16UI: return GL_RGB16UI; break;
        case TF_RGB32UI: return GL_RGB32UI; break;
        //int
        case TF_RGB16I: return GL_RGB16I; break;
        case TF_RGB32I: return GL_RGB32I; break;
        //float
        case TF_RGB16F: return GL_RGB16F; break;
        case TF_RGB32F: return GL_RGB32F; break;
        ////////////////////
        //RG
        case TF_RG8: return GL_RG8; break;
        //uint
        case TF_RG16UI: return GL_RG16UI; break;
        case TF_RG32UI: return GL_RG32UI; break;
        //int
        case TF_RG16I: return GL_RG16I; break;
        case TF_RG32I: return GL_RG32I; break;
        //float
        case TF_RG16F: return GL_RG16F; break;
        case TF_RG32F: return GL_RG32F; break;
        ////////////////////
        //R
        case TF_R8: return GL_R8; break;
        //uint
        case TF_R16UI: return GL_R16UI; break;
        case TF_R32UI: return GL_R32UI; break;
        //int
        case TF_R16I: return GL_R16I; break;
        case TF_R32I: return GL_R32I; break;
        //float
        case TF_R16F: return GL_R16F; break;
        case TF_R32F: return GL_R32F; break;
        ///////////////////
        case TF_RGB565:  return GL_RGB565;  break;
        case TF_RGB5A1:  return GL_RGB5_A1; break;
        case TF_RGBA4:   return GL_RGBA4;   break;
        ///////////////////
        //DEPTH & STENCIL
        case TF_DEPTH16_STENCIL8: return GL_DEPTH24_STENCIL8; break;
        case TF_DEPTH24_STENCIL8: return GL_DEPTH24_STENCIL8; break;
        case TF_DEPTH32_STENCIL8: return GL_DEPTH32F_STENCIL8; break;
        //DEPTH
        case TF_DEPTH_COMPONENT16: return GL_DEPTH_COMPONENT16; break;
        case TF_DEPTH_COMPONENT24: return GL_DEPTH_COMPONENT24; break;
        case TF_DEPTH_COMPONENT32: return GL_DEPTH_COMPONENT32; break;
            
        default: return GL_ZERO; break;
    }
}

inline const GLenum get_texture_type(texture_type type)
{
    switch (type)
    {
        case TT_R:             return GL_RED;
        case TT_RG:            return GL_RG;
        case TT_RGB:           return GL_RGB;
        case TT_RGBA:          return GL_RGBA;
        case TT_DEPTH:         return GL_DEPTH_COMPONENT;
        case TT_DEPTH_STENCIL: return GL_DEPTH_STENCIL;
        default: return GL_ZERO; break;
    }
}

inline const GLenum get_texture_type_format(texture_type_format type)
{
    switch (type)
    {
        default:
        case TTF_FLOAT:             return GL_FLOAT;
        case TTF_UNSIGNED_BYTE:     return GL_UNSIGNED_BYTE;
        case TTF_UNSIGNED_SHORT:    return GL_UNSIGNED_SHORT;
        case TTF_UNSIGNED_INT:      return GL_UNSIGNED_INT;
    }
}

static GLenum get_texture_min_filter(texture_min_filter_type type)
{
    switch (type)
    {
        default:
        case TMIN_NEAREST:                return GL_NEAREST;
        case TMIN_NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
        case TMIN_NEAREST_MIPMAP_LINEAR:  return GL_NEAREST_MIPMAP_LINEAR;
        case TMIN_LINEAR:                return GL_LINEAR;
        case TMIN_LINEAR_MIPMAP_NEAREST: return GL_LINEAR_MIPMAP_NEAREST;
        case TMIN_LINEAR_MIPMAP_LINEAR:  return GL_LINEAR_MIPMAP_LINEAR;
    }
}

static GLenum get_texture_mag_filter(texture_mag_filter_type type)
{
    switch (type)
    {
        default:
        case TMAG_NEAREST:  return GL_NEAREST;
        case TMAG_LINEAR:   return GL_LINEAR;
    }
}

static GLenum get_texture_edge_type(texture_edge_type type)
{
    switch (type)
    {
        default:
        case TEDGE_CLAMP:  return GL_CLAMP_TO_EDGE;
        case TEDGE_REPEAT: return GL_REPEAT;
    }
}

context_texture* create_texture(texture_format format,
                                        unsigned int w,
                                        unsigned int h,
                                        const unsigned char* bytes,
                                        texture_type   type,
                                        texture_type_format type_format,
                                        texture_min_filter_type min_type,
                                        texture_mag_filter_type mag_type,
                                        texture_edge_type       edge_s,
                                        texture_edge_type       edge_t,
                                        bool                    build_mipmap)
{
    
    //new texture
    context_texture* ctx_texture=new context_texture(w,h);
    //create a texture id
    ctx_texture->create_TBO();
    //format
    GLenum gl_format     =get_texture_format(format);
    GLenum gl_type       =get_texture_type(type);
    GLenum gl_type_format=get_texture_type_format(type_format);
    //enable texture
    glBindTexture(GL_TEXTURE_2D, ctx_texture->m_tbo);
    //create texture buffer
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 gl_format,
                 ctx_texture->m_width,
                 ctx_texture->m_height,
                 0,
                 gl_type,
                 gl_type_format,
                 bytes);
    //set filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, get_texture_min_filter(min_type));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, get_texture_mag_filter(mag_type));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, get_texture_edge_type(edge_s));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, get_texture_edge_type(edge_t));
    // Generate mipmaps, by the way.
    if(build_mipmap) glGenerateMipmap(GL_TEXTURE_2D);
    //disable texture
    glBindTexture(GL_TEXTURE_2D, 0);
    //test
    print_errors();
    //return texture
    return ctx_texture;
}

void bind_texture(context_texture* ctx_texture, int n)
{
    if(ctx_texture) ctx_texture->enable_TBO(n);
}

void unbind_texture(context_texture* ctx_texture)
{
    if(ctx_texture) ctx_texture->disable_TBO();
}

void delete_texture(context_texture*& texture)
{
    delete texture;
    texture = nullptr;
}
/*
 FBO
*/

static std::string framebuffer_error_to_str(GLenum error)
{
    switch (error)
    {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
//      case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:         return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        case GL_FRAMEBUFFER_UNSUPPORTED:                   return "GL_FRAMEBUFFER_UNSUPPORTED";
        case GL_FRAMEBUFFER_COMPLETE:                      return "GL_FRAMEBUFFER_COMPLETE";
        default:                                           return "GL_UNKNOW";
    }
}

context_render_target* create_render_target(const std::vector< target_field >& textures)
{
    bool   depth_attach = false;
    size_t color_count  = 0;
    //create FBO
    auto fbo = new context_render_target;
         fbo->gen_FBO();
    //enable
    fbo->enable_FBO();
    //attach
    for(const target_field& t_field : textures)
    {
        if(t_field.m_type == RT_COLOR)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   (GLenum)(GL_COLOR_ATTACHMENT0 + (color_count++)),
                                   GL_TEXTURE_2D,
                                   t_field.m_texture->m_tbo,
                                   0);
        }
        else if((!depth_attach) && t_field.m_type == RT_DEPTH)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_DEPTH_ATTACHMENT,
                                   GL_TEXTURE_2D,
                                   t_field.m_texture->m_tbo,
                                   0);
            depth_attach = true;
        }
        else if((!depth_attach) && t_field.m_type == RT_DEPTH_STENCIL)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_TEXTURE_2D,
                                   t_field.m_texture->m_tbo,
                                   0);
            depth_attach = true;
        }
    }
    //set buffer to draw
    std::vector < GLenum > m_buffers_to_draw(color_count);
    for(size_t i=0;i!=color_count;++i) m_buffers_to_draw[i] = (GLenum)(GL_COLOR_ATTACHMENT0+i);
    glDrawBuffers((GLuint)color_count,m_buffers_to_draw.data());
    //status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    //test
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << framebuffer_error_to_str(status) << std::endl;
    }
    //disable
    fbo->disable_FBO();
    //return
    return fbo;
}

void enable_render_target(context_render_target* rdtex)
{
    rdtex->enable_FBO();
}

void disable_render_target(context_render_target* rdtex)
{
    rdtex->disable_FBO();
}

void delete_render_target(context_render_target*& r_target)
{
    delete r_target;
    r_target = nullptr;
}

bool print_errors()
{
	//get all gl errors
	std::string gl_errors = debug_gl_errors_to_string();
	//print
	if (gl_errors.size())
	{
		std::cout << gl_errors << std::endl;
		return true;
	}
	return false;
}
  
//end render
};