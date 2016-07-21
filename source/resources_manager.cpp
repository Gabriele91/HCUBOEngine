//
//  resources.cpp
//  HCubo
//
//  Created by Gabriele on 12/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <resource.h>
#include <resources_manager.h>
#include <filesystem.h>


void resources_manager::add_directory(const std::string& directory, bool recursive)
{
    //get all files
    filesystem::files_list files = filesystem::get_files(directory);
    //success?
    if(!files.m_success) return;
    //put into table
    for(const std::string& filename : files.m_fields)
    {
        //get extention
        std::string ext = filesystem::get_extension(filename);
        std::string basename = filesystem::get_basename(filename);
        //types
        if(ext == ".png")
        {
            get_texture(basename)->load(*this,directory+"/"+filename);
        }
        else if(ext == ".glsl")
        {
            get_shader(basename)->load(*this,directory+"/"+filename);
        }
        else if(ext == ".mat")
        {
            get_material(basename)->load(*this,directory+"/"+filename);
        }
        else if(ext == ".json")
        {
            //none
        }
    }
    //sub directories
    if(recursive)
    {
        //get all directories
        filesystem::directories_list directories = filesystem::get_sub_directories(directory);
        //success?
        if(!directories.m_success) return;
        //push dir into table
        for(const std::string& directoryname : directories.m_fields)
        {
            add_directory(directory+"/"+directoryname,true);
        }
    }
}

shader::ptr resources_manager::get_shader(const std::string& name)
{
    //shader name
    std::string shader_name = "shader:"+name;
    //rerouce
    resources_map_it resource_it = m_resources_map.find(shader_name);
    //alloc
    if(resource_it == m_resources_map.end())
    {
        m_resources_map[shader_name] =  std::static_pointer_cast< resource >(shader::snew());
        resource_it = m_resources_map.find(shader_name);
    }
    //return
    return std::static_pointer_cast< shader >(resource_it->second);
}

texture::ptr resources_manager::get_texture(const std::string& name)
{
    //shader name
    std::string texture_name = "texture:"+name;
    //rerouce
    resources_map_it resource_it = m_resources_map.find(texture_name);
    //alloc
    if(resource_it == m_resources_map.end())
    {
        m_resources_map[texture_name] = std::static_pointer_cast< resource >(texture::snew());
        resource_it = m_resources_map.find(texture_name);
    }
    //return
    return std::static_pointer_cast< texture >(resource_it->second);
}

material_ptr resources_manager::get_material(const std::string& name)
{
    //shader name
    std::string material_name = "material:"+name;
    //rerouce
    resources_map_it resource_it = m_resources_map.find(material_name);
    //alloc
    if(resource_it == m_resources_map.end())
    {
        m_resources_map[material_name] = std::static_pointer_cast< resource >(material_snew());
        resource_it = m_resources_map.find(material_name);
    }
    //return
    return std::static_pointer_cast< material >(resource_it->second);
}