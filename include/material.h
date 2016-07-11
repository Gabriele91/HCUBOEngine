//
//  material.h
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <shader.h>
#include <shader_map.h>
#include <camera.h>

class material
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
    
    virtual void bind(camera* cam,const glm::mat4& model) = 0;
    virtual void unbind() = 0;
    
    virtual ~material(){};
    
protected:
    
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
    
    bool m_cullface  { false   };
    int  m_cullmode  { GL_BACK };
    
    bool m_blend     { false  };
    int  m_blend_src { GL_ONE };
    int  m_blend_dst { GL_ZERO};
    
    
};

using material_ptr  = std::shared_ptr< material >;
using material_uptr = std::unique_ptr< material >;
