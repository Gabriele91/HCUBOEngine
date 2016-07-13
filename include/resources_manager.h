//
//  assets.h
//  HCubo
//
//  Created by Gabriele on 12/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <unordered_map>
#include <resource.h>
#include <shader.h>
#include <texture.h>

class resources_manager
{
public:
    
    void add_directory(const std::string& directory,bool recursive = false);
    shader::ptr get_shader(const std::string& name);
    texture::ptr get_texture(const std::string& name);
    
private:
    
    using resources_map     = std::unordered_map< std::string, resource_ptr >;
    using resources_map_it  = resources_map::iterator;
    using resources_map_cit = resources_map::const_iterator;
    resources_map m_resources_map;

};