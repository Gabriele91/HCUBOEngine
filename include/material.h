//
//  material.h
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <OpenGL4.h>
#include <component.h>
#include <camera.h>
#include <vector>
#include <texture.h>
#include <shader.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class material : public component, public resource
{
    
    COMPONENT_DEC(material)
    
public:

	virtual ~material();

    bool load(resources_manager& resources,const std::string& path);
    
	void cullface(bool face);
    
	void cullmode(int mode);
    
	void blend(bool b);
    
	void blend_src(int src);
    
	void blend_dst(int dst);
    
	virtual void bind(const glm::vec4& viewport,
					  const glm::mat4& projection,
					  const glm::mat4& view,
					  const glm::mat4& model);
    
	virtual void unbind();
            
	void  bind_state();
    
	void unbind_state();

	component_ptr material::copy() const;

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
    uniform_vec4::ptr m_uniform_viewport{ nullptr };
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
