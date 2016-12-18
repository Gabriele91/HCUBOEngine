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
	class uniform;
	class shader;

	class HCUBE_RENDER_API uniform
	{

	public:

		void set_value(texture::ptr in_texture);
		void set_value(context_texture* in_texture);
		void set_value(int i);
		void set_value(float f);
		void set_value(const vec2& v2);
		void set_value(const vec3& v3);
		void set_value(const vec4& v4);
		void set_value(const mat3& m3);
		void set_value(const mat4& m4);

		void set_value(const int* i, size_t n);
		void set_value(const float* f, size_t n);
		void set_value(const vec2* v2, size_t n);
		void set_value(const vec3* v3, size_t n);
		void set_value(const vec4* v4, size_t n);
		void set_value(const mat3* m3, size_t n);
		void set_value(const mat4* m4, size_t n);

		void set_value(const std::vector < int >& i);
		void set_value(const std::vector < float >& f);
		void set_value(const std::vector < vec2 >& v2);
		void set_value(const std::vector < vec3 >& v3);
		void set_value(const std::vector < vec4 >& v4);
		void set_value(const std::vector < mat3 >& m3);
		void set_value(const std::vector < mat4 >& m4);

		void set_value(const context_const_buffer*);

		uniform() {}

		bool is_valid() { return m_shader && m_id >= -1; }

	protected:

		friend class shader;
		shader* m_shader{ nullptr };
		long    m_id{ -1 };

		uniform(shader* arg_shader, long arg_id) : m_shader(arg_shader), m_id(arg_id) {}
	};

	class HCUBE_RENDER_API shader : public smart_pointers<shader>, public resource
	{

	public:

        using preprocess_element = std::tuple<std::string,std::string>;
        using preprocess_map     = std::vector< preprocess_element >;
		using uniform_map        = std::unordered_map< std::string, uniform >;
		using filepath_map       = std::unordered_map< int, std::string >;

		shader() {}

		virtual ~shader();

		bool load(resources_manager& resources, const std::string& path);
        
        bool load(const std::string& file_vs,
                  const std::string& file_fs,
				  const std::string& file_gs,
				  const std::string& file_tcs,
				  const std::string& file_tes,
                  const preprocess_map& defines);
        
        
        bool load(const std::string& effect_file,
                  const preprocess_map& defines);
        
		bool load_effect(const std::string& effect,
			             const std::string& effect_file,
						 const preprocess_map& defines,
                         const size_t line);

		bool load_effect(resources_manager& resources, 
						 const std::string& effect,
						 const std::string& effect_file,
						 const preprocess_map& defines,
						 const size_t line);

		bool load_shader(const std::string& vs,
                         const size_t line_vs,
			             const std::string& fs,
                         const size_t line_fs,
			             const std::string& gs,
                         const size_t line_gs,
						 const std::string& tc,
						 const size_t line_tc,
						 const std::string& te,
						 const size_t line_te,
			             const preprocess_map& defines);

		//get consts
		uniform* get_uniform(const char *name);
		uniform* get_uniform_buffer(const char *name);

		//imposta shader
		virtual void bind();
		virtual void unbind();

		//id programma
		unsigned int program_id() const;

	protected:

		//get uniform id 
		int get_uniform_id(const char *name);
		unsigned int get_uniform_buffer_id(const char *name);

		//friend class
		friend class uniform;

		//count uniforms texture
		long m_uniform_ntexture{ -1 };

		//uniforms
		uniform_map m_uniform_map;

		//file error mapping
		filepath_map m_file_path_id;

	private:

		unsigned int m_shader_id { 0 }; //< shader program
		unsigned int m_shader_vs { 0 }; //< vertex shader
		unsigned int m_shader_fs { 0 }; //< fragment shader
		unsigned int m_shader_gs { 0 }; //< geometry shader
		unsigned int m_shader_tcs{ 0 }; //< tessellation control shader
		unsigned int m_shader_tes{ 0 }; //< tessellation evaluation  shader
		void delete_program(); 

	};
}