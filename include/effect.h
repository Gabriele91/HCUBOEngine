#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <texture.h>
#include <shader.h>
#include <smart_pointers.h>

namespace hcube
{
	class effect : public smart_pointers<effect>, public resource
	{

	public:

		//parameter type
		struct parameter
		{
			enum parameter_type
			{
				PT_NONE,
				PT_INT,
				PT_FLOAT,
				PT_TEXTURE,
				PT_VEC2,
				PT_VEC3,
				PT_VEC4,
				PT_MAT4,
				PT_INT_ARRAY,
				PT_FLOAT_ARRAY,
				PT_TEXTURE_ARRAY,
				PT_VEC2_ARRAY,
				PT_VEC3_ARRAY,
				PT_VEC4_ARRAY,
				PT_MAT4_ARRAY
			};

			virtual void set_value(texture::ptr in_texture) {}
			virtual void set_value(int i) {}
			virtual void set_value(float f) {}
			virtual void set_value(const glm::vec2& v2) {}
			virtual void set_value(const glm::vec3& v3) {}
			virtual void set_value(const glm::vec4& v4) {}
			virtual void set_value(const glm::mat4& m4) {}

			virtual void set_value(const int* i, size_t n) {}
			virtual void set_value(const float* f, size_t n) {}
			virtual void set_value(const glm::vec2* v2, size_t n) {}
			virtual void set_value(const glm::vec3* v3, size_t n) {}
			virtual void set_value(const glm::vec4* v4, size_t n) {}
			virtual void set_value(const glm::mat4* m4, size_t n) {}

			virtual void set_value(const std::vector < int >& i) {}
			virtual void set_value(const std::vector < float >& f) {}
			virtual void set_value(const std::vector < glm::vec2 >& v2) {}
			virtual void set_value(const std::vector < glm::vec3 >& v3) {}
			virtual void set_value(const std::vector < glm::vec4 >& v4) {}
			virtual void set_value(const std::vector < glm::mat4 >& m4) {}

			parameter() {}

			virtual bool is_valid() { return m_id >= 0; }
			parameter_type get_type() { return m_type; }

		protected:

			friend class effect;

			int			   m_id{ -1 };
			parameter_type m_type{ PT_NONE };

		};

		//parameters list
		using parameters = std::vector < parameter* >;

		//pass type
		struct pass
		{
			cullface_state m_cullface;
			depth_buffer_state m_depth;
			blend_state m_blend;
			shader::ptr m_shader;
			std::vector< int >		m_param_id;
			std::vector< uniform* > m_uniform;
		};

		//pass list
		using technique = std::vector < pass >;

		//maps
		using map_parameters = std::unordered_map< std::string, int >;
		using map_techniques = std::unordered_map< std::string, technique >;

		//load effect
		bool load(resources_manager& resources, const std::string& path);

		//get technique
		technique* get_technique(const std::string& technique);

		//get parameter
		parameter* get_parameter(int parameter);
		parameter* get_parameter(const std::string& parameter);

		//get id
		int get_parameter_id(const std::string& parameter);

	protected:
		parameters		m_parameters;
		map_parameters	m_map_parameters;
		map_techniques  m_map_techniques;

	};
}