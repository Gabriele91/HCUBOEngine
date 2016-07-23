//
//  resources.cpp
//  HCubo
//
//  Created by Gabriele on 12/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <resource.h>
#include <resources_manager.h>
#include <static_model.h>
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
        if(ext == ".png" || ext == ".tga")
        {
			set_texture_path(basename, directory + "/" + filename);
        }
		else if (ext == ".glsl")
        {
			set_shader_path(basename, directory + "/" + filename);
        }
		else if (ext == ".mat")
		{
			set_material_path(basename, directory + "/" + filename);
		}
		else if (ext == ".smhc")
		{
			set_static_model_path(basename, directory + "/" + filename);
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

void resources_manager::set_shader_path(const std::string& name, const std::string& path)
{
	//shader name
	std::string resource_name = "shader:" + name;
	//set path
	m_resources_path_map[resource_name] = path;
}

void resources_manager::set_texture_path(const std::string& name, const std::string& path)
{
	//shader name
	std::string resource_name = "texture:" + name;
	//set path
	m_resources_path_map[resource_name] = path;

}

void resources_manager::set_material_path(const std::string& name, const std::string& path)
{
	//shader name
	std::string resource_name = "material:" + name;
	//set path
	m_resources_path_map[resource_name] = path;
}

void resources_manager::set_static_model_path(const std::string& name, const std::string& path)
{
	//shader name
	std::string resource_name = "static_model:" + name;
	//set path
	m_resources_path_map[resource_name] = path;
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
		resource_it->second->load(*this, m_resources_path_map[shader_name]);
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
		resource_it->second->load(*this, m_resources_path_map[texture_name]);
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
		resource_it->second->load(*this, m_resources_path_map[material_name]);
    }
    //return
    return std::static_pointer_cast< material >(resource_it->second);
}


static_model::ptr resources_manager::get_static_model(const std::string& name)
{
	//shader name
	std::string static_model_name = "static_model:" + name;
	//rerouce
	resources_map_it resource_it = m_resources_map.find(static_model_name);
	//alloc
	if (resource_it == m_resources_map.end())
	{
		m_resources_map[static_model_name] = std::static_pointer_cast< resource >(static_model::snew());
		resource_it = m_resources_map.find(static_model_name);
		resource_it->second->load(*this, m_resources_path_map[static_model_name]);
	}
	//return
	return std::static_pointer_cast< static_model >(resource_it->second);
}
