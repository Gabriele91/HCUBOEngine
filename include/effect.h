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
#include <light.h>
#include <smart_pointers.h>

namespace hcube
{
	class effect : public smart_pointers<effect>, public resource
	{

	public:

		//parameter type
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

		//parameter class
		struct parameter
		{
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

			virtual texture::ptr get_texture() { return texture::ptr(nullptr); }
			virtual int          get_int()     { return 0;    }
			virtual float        get_float()   { return 0.0f; }
			virtual const glm::vec2& get_vec2() { return glm::vec2();  }
			virtual const glm::vec3& get_vec3() { return glm::vec3();  }
			virtual const glm::vec4& get_vec4() { return glm::vec4();  }
			virtual const glm::mat4& get_mat4() { return glm::mat4();  }

			virtual const std::vector<int>&       get_int_array()  { return{}; }
			virtual const std::vector<float>&     get_float_array(){ return{}; }
			virtual const std::vector<glm::vec2>& get_vec2_array() { return{}; }
			virtual const std::vector<glm::vec3>& get_vec3_array() { return{}; }
			virtual const std::vector<glm::vec4>& get_vec4_array() { return{}; }
			virtual const std::vector<glm::mat4>& get_mat4_array() { return{}; }

			virtual parameter* copy() const = 0;

		protected:

			friend class effect;

			int			   m_id{ -1 };
			parameter_type m_type{ PT_NONE };

		};

		//parameters list
		using parameters = std::vector < std::unique_ptr< parameter > >;

		//pass type
		struct pass
		{
			effect*				    m_effect{ nullptr };
			cullface_state		    m_cullface;
			depth_buffer_state	    m_depth;
			blend_state			    m_blend;
			shader::ptr			    m_shader;
			std::vector< int >		m_param_id;
			std::vector< uniform* > m_uniform;
			//default uniform
			uniform* m_uniform_projection{ nullptr };
			uniform* m_uniform_view{ nullptr };
			uniform* m_uniform_model{ nullptr };
			uniform* m_uniform_viewport{ nullptr };
			//all light uniform
			bool m_support_light{ false };
			uniform* m_uniform_ambient_light{ nullptr };
			uniform* m_uniform_n_lights_used{ nullptr };
			std::vector< uniform_light > m_uniform_lights;
			//unsafe
			void bind(
				const glm::vec4& viewport,
				const glm::mat4& projection,
				const glm::mat4& view,
				const glm::mat4& model, 
				parameters* params = nullptr
			);
			void unbind();
			//safe
			render_state safe_bind(
				const glm::vec4& viewport,
				const glm::mat4& projection,
				const glm::mat4& view,
				const glm::mat4& model,
				parameters* params = nullptr
			);
			void safe_unbind(const render_state&);
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
		parameter*  get_parameter(int parameter);
		parameter*  get_parameter(const std::string& parameter);
		parameters* copy_all_parameters();

		//get id
		int get_parameter_id(const std::string& parameter);


	protected:
		parameters		m_parameters;
		map_parameters	m_map_parameters;
		map_techniques  m_map_techniques;

	};
}