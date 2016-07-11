//
//  shader_map.h
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once;
#include <string>
#include <map>
#include <shader.h>

class shader_map
{
public:
    
    void load_standard_shader();
    shader& get_shader(const std::string& name);
    
protected:
    
    std::map< std::string, shader > m_map_shaders;
    
};