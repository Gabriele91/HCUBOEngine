#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <vector_math.h>
#include <texture.h>
#include <shader.h>
#include <light.h>
#include <camera.h>
#include <transform.h>
#include <smart_pointers.h>
#include <render_queues.h>

namespace hcube
{
	class effect : public smart_pointers<effect>, public resource
	{

	public:

        //queue param
        struct parameter_queue
        {
            render_queue_type m_type { RQ_OPAQUE };
            int               m_order{ 0         };
        };
        
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
			virtual void set_value(const vec2& v2) {}
			virtual void set_value(const vec3& v3) {}
			virtual void set_value(const vec4& v4) {}
			virtual void set_value(const mat4& m4) {}

			virtual void set_value(const int* i, size_t n) {}
			virtual void set_value(const float* f, size_t n) {}
			virtual void set_value(const vec2* v2, size_t n) {}
			virtual void set_value(const vec3* v3, size_t n) {}
			virtual void set_value(const vec4* v4, size_t n) {}
			virtual void set_value(const mat4* m4, size_t n) {}

			virtual void set_value(const std::vector < int >& i) {}
			virtual void set_value(const std::vector < float >& f) {}
			virtual void set_value(const std::vector < vec2 >& v2) {}
			virtual void set_value(const std::vector < vec3 >& v3) {}
			virtual void set_value(const std::vector < vec4 >& v4) {}
			virtual void set_value(const std::vector < mat4 >& m4) {}

			parameter() {}

			virtual bool is_valid() { return m_id >= 0; }
			parameter_type get_type() { return m_type; }

			virtual texture::ptr get_texture() const { return texture::ptr(nullptr); }
			virtual int          get_int()     const { return 0;    }
			virtual float        get_float()   const { return 0.0f; }
			virtual const vec2&  get_vec2()    const { return vec2();  }
			virtual const vec3&  get_vec3()    const { return vec3();  }
			virtual const vec4&  get_vec4()    const { return vec4();  }
			virtual const mat4&  get_mat4()    const { return mat4();  }

			virtual const std::vector<int>&       get_int_array()   const { return{}; }
			virtual const std::vector<float>&     get_float_array() const { return{}; }
			virtual const std::vector<vec2>& get_vec2_array()  const { return{}; }
			virtual const std::vector<vec3>& get_vec3_array()  const { return{}; }
			virtual const std::vector<vec4>& get_vec4_array()  const { return{}; }
			virtual const std::vector<mat4>& get_mat4_array()  const { return{}; }

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
			uniform_camera    m_uniform_camera;
			uniform_transform m_uniform_transform;
			//all light uniform
			bool m_support_light{ false };
			//uniforms
			uniform*				 m_uniform_ambient_light{ nullptr };
			uniform_light_spot       m_uniform_spot;
			uniform_light_point      m_uniform_point;
			uniform_light_direction  m_uniform_direction;
			//unsafe
			void bind
			(
				camera::wptr   c_camera,
				transform_wptr c_camera_transform,
				transform_wptr c_model_transform,
				parameters* params = nullptr
			) const;

			void bind(parameters* params = nullptr) const;

			void unbind();
			//safe
			render_state safe_bind
			(
				camera::wptr   c_camera,
				transform_wptr c_camera_transform,
				transform_wptr c_model_transform,
				parameters*   params = nullptr
			) const;

			render_state safe_bind
			(
				parameters* params = nullptr
			) const;

			void safe_unbind(const render_state&);
		};

        
        //pass list
        class technique
        {
            
        public:
            
            using pass_list     = std::vector < pass >;
            using pass_list_it  = pass_list::iterator;
            using pass_list_cit = pass_list::const_iterator;
            
            //set queue
            void set_queue(const parameter_queue& queue)
            {
                m_queue = queue;
            }
            
            const parameter_queue& get_queue() const
            {
                return m_queue;
            }
            //info
            size_t size()
            {
                return m_passes.size();
            }
            
            //alloc
            void resize(size_t n)
            {
                m_passes.resize(n);
            }
            
            void reserve(size_t n)
            {
                m_passes.reserve(n);
            }
            
            //push
            void push_back(const pass&& pass)
            {
                m_passes.push_back(std::move(pass));
            }
            
            //operator
            pass& operator[](size_t i)
            {
                return m_passes[i];
            }
            
            const pass& operator[](size_t i) const
            {
                return m_passes[i];
            }
            
            //iterators
            pass_list_it begin()
            {
                return m_passes.begin();
            }
            
            pass_list_it end()
            {
                return m_passes.end();
            }
            
            pass_list_cit begin() const
            {
                return m_passes.begin();
            }
            
            pass_list_cit end() const
            {
                return m_passes.end();
            }
            
        protected:
            
            parameter_queue      m_queue;
            std::vector < pass > m_passes;
        
        };

		//maps
		using map_parameters = std::unordered_map< std::string, int >;
		using map_techniques = std::unordered_map< std::string, technique >;

		//load effect
		bool load(resources_manager& resources, const std::string& path);

		//get technique
		technique* get_technique(const std::string& technique);
        //all techniques
        const map_techniques& get_techniques()
        {
            return m_map_techniques;
        }
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