//
//  material.h
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <OpenGL4.h>
#include <camera.h>
#include <vector>
#include <texture.h>
#include <shader.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class material : public resource
{
public:
    
    void cullface(bool face)
    {
        m_cullface = face;
    }
    
    void cullmode(int mode)
    {
        m_cullmode = mode;
    }
    
    void blend(bool b)
    {
        m_blend = b;
    }
    
    void blend_src(int src)
    {
        m_blend_src = src;
    }
    
    void blend_dst(int dst)
    {
        m_blend_dst = dst;
    }
    
    void load(resources_manager& resources,const std::string& path);
    
    virtual void bind(camera* cam,const glm::mat4& model)
    {
        m_shader->bind();
        
        if(m_uniform_projection)
            m_uniform_projection->set_value(cam->get_projection());
        
        if(m_uniform_view)
            m_uniform_view->set_value(cam->get_view());
        
        if(m_uniform_model)
            m_uniform_model->set_value(model);
        
        for(size_t i=0; i!=m_ints.size(); ++i)
        {
            m_uniform_ints[i]->set_value(m_ints[i]);
        }
        
        for(size_t i=0; i!=m_floats.size(); ++i)
        {
            m_uniform_floats[i]->set_value(m_floats[i]);
        }
        
        for(size_t i=0; i!=m_textures.size(); ++i)
        {
            m_uniform_textures[i]->set_value(m_textures[i]);
        }
        
        for(size_t i=0; i!=m_vec2s.size(); ++i)
        {
            m_uniform_vec2s[i]->set_value(m_vec2s[i]);
        }
        
        for(size_t i=0; i!=m_vec3s.size(); ++i)
        {
            m_uniform_vec3s[i]->set_value(m_vec3s[i]);
        }
        
        for(size_t i=0; i!=m_vec4s.size(); ++i)
        {
            m_uniform_vec4s[i]->set_value(m_vec4s[i]);
        }
        
        for(size_t i=0; i!=m_mat4s.size(); ++i)
        {
            m_uniform_mat4s[i]->set_value(m_mat4s[i]);
        }
    }
    
    virtual void unbind()
    {
        m_shader->unbind();
    }
    
    virtual ~material(){};
        
    void  bind_state()
    {
        if(m_cullface)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(m_cullmode);
        }
        if(m_blend)
        {
            glEnable(GL_BLEND);
            glBlendFunc(m_blend_src, m_blend_dst);
        }
    }
    
    void unbind_state()
    {
        if(m_blend)
        {
            glDisable(GL_BLEND);
        }
        if(m_cullface)
        {
            glDisable(GL_CULL_FACE);
            glCullFace(m_cullmode);
        }
    }

protected:

    bool m_cullface  { true   };
    int  m_cullmode  { GL_BACK };
    
    bool m_blend     { false  };
    int  m_blend_src { GL_ONE };
    int  m_blend_dst { GL_ZERO};
    
    
    //shader
    shader::ptr  m_shader { nullptr };
    //standard uniform
    uniform_mat4::ptr m_uniform_projection{ nullptr };
    uniform_mat4::ptr m_uniform_view{ nullptr };
    uniform_mat4::ptr m_uniform_model{ nullptr };
    //uniform textures
    std::vector< texture::ptr > m_textures;
    std::vector< uniform_texture::ptr > m_uniform_textures;
    //uniform float
    std::vector< float > m_floats;
    std::vector< uniform_float::ptr > m_uniform_floats;
    //uniform int
    std::vector< int > m_ints;
    std::vector< uniform_int::ptr > m_uniform_ints;
    //uniform vec2
    std::vector< glm::vec2 > m_vec2s;
    std::vector< uniform_vec2::ptr > m_uniform_vec2s;
    //uniform vec3
    std::vector< glm::vec3 > m_vec3s;
    std::vector< uniform_vec3::ptr > m_uniform_vec3s;
    //uniform vec4
    std::vector< glm::vec4 > m_vec4s;
    std::vector< uniform_vec4::ptr > m_uniform_vec4s;
    //uniform mat4
    std::vector< glm::mat4 > m_mat4s;
    std::vector< uniform_mat4::ptr > m_uniform_mat4s;
};

using material_ptr  = std::shared_ptr< material >;
using material_uptr = std::unique_ptr< material >;
using material_wptr = std::weak_ptr  < material >;
template < class... Args >
static inline material_ptr material_snew(Args&&... args)
{
    return std::make_shared< material >( args... );
}
