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
#include <material.h>

class resources_manager
{
public:
    
    void add_directory(const std::string& directory,bool recursive = false);
    shader::ptr get_shader(const std::string& name);
    texture::ptr get_texture(const std::string& name);
	material_ptr get_material(const std::string& name);

    
private:


	using resources_path_map = std::unordered_map< std::string, std::string >;
    using resources_map      = std::unordered_map< std::string, resource_ptr >;
    using resources_map_it   = resources_map::iterator;
    using resources_map_cit  = resources_map::const_iterator;

	resources_map      m_resources_map;
	resources_path_map m_resources_path_map;

	void set_shader_path(const std::string& name, const std::string& path);
	void set_texture_path(const std::string& name, const std::string& path);
	void set_material_path(const std::string& name, const std::string& path);

};