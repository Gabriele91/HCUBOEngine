//
//  assets.h
//  HCubo
//
//  Created by Gabriele on 12/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <regex>
#include <unordered_map>
#include <hcube/config.h>
#include <hcube/core/resource.h>
#include <hcube/render/shader.h>
#include <hcube/render/effect.h>
#include <hcube/resources/texture.h>
#include <hcube/resources/material.h>
#include <hcube/resources/prefab.h>

namespace hcube
{
	class HCUBE_API resources_manager
	{
	public:

		bool add_resources_file(const std::string& path);

		void add_file(const std::string& path);
		void add_file(const std::string& asset_name, const std::string& path);

		void add_directory(const std::string& directory, bool recursive = false);
		void add_directory(const std::string& directory, const std::string& filter, bool recursive = false);

		shader::ptr get_shader(const std::string& name);
		effect::ptr get_effect(const std::string& name);
		texture::ptr get_texture(const std::string& name);
		material_ptr get_material(const std::string& name);
		prefab_ptr get_prefab(const std::string& name);

		void set_shader_path(const std::string& name, const std::string& path);
		void set_effect_path(const std::string& name, const std::string& path);
		void set_texture_path(const std::string& name, const std::string& path);
		void set_material_path(const std::string& name, const std::string& path);
		void set_prefab_path(const std::string& name, const std::string& path);

		const std::string& get_shader_path(const std::string& name);
		const std::string& get_effect_path(const std::string& name);
		const std::string& get_texture_path(const std::string& name);
		const std::string& get_material_path(const std::string& name);
		const std::string& get_prefab_path(const std::string& name);

	private:

		void add_file(const std::string& asset_name,
					  const std::string& directory,
					  const std::string& filename);

		void add_directory(const std::string& directory,
						   const std::regex& filter,
						   bool recursive = false);

		using resources_path_map = std::unordered_map< std::string, std::string >;
		using resources_map = std::unordered_map< std::string, resource_ptr >;
		using resources_map_it = resources_map::iterator;
		using resources_map_cit = resources_map::const_iterator;

		resources_map      m_resources_map;
		resources_path_map m_resources_path_map;
	};
}