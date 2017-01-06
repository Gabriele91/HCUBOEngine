//
//  shader.h
//  OGLHCubeView
//
//  Created by Gabriele on 28/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <hcube/config.h>
#include <hcube/math/vector_math.h>
#include <hcube/core/smart_pointers.h>
#include <hcube/resources/texture.h>

namespace hcube
{
	class HCUBE_RENDER_API shader : public smart_pointers<shader>, public resource
	{

	public:

        using preprocess_element = std::tuple<std::string,std::string>;
        using preprocess_map     = std::vector< preprocess_element >;
		using filepath_map       = std::unordered_map< int, std::string >;

		shader() {}

		virtual ~shader();

		bool load(resources_manager& resources, const std::string& path);
        
        bool load(const std::string& effect_file, const preprocess_map& defines);
        
		bool load_effect(const std::string& effect, const std::string& effect_file, const preprocess_map& defines,  const size_t line);

		bool load_effect(resources_manager& resources,  const std::string& effect,  const std::string& effect_file,  const preprocess_map& defines, const size_t line);

		//get compiler errors
		std::string get_errors() const;

		//get consts
		context_uniform* get_uniform(const char *name);
		context_uniform* get_uniform_buffer(const char *name);

		//imposta shader
		virtual void bind();
		virtual void unbind();
		
		//program
		context_shader* get_raw_shader() const;

	protected:

		//load raw shader
		bool load_shader(
			const std::string& vs_str, const size_t line_vs,
			const std::string& fs_str, const size_t line_fs,
			const std::string& gs_str, const size_t line_gs,
			const std::string& tcs_str, const size_t line_tcs,
			const std::string& tes_str, const size_t line_tes,
			const preprocess_map& all_process
		);

		//file error mapping
		filepath_map m_file_path_id;

	private:

		context_shader* m_shader{ nullptr };

	};
}