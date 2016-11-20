#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <hcube/core/filesystem.h>
#include <hcube/core/resources_manager.h>
#include <hcube/render/effect.h>
#include <hcube/data/parser/effect_parser.h>

namespace hcube
{


	using  map_parameters  = effect::map_parameters;
	using  map_techniques  = effect::map_techniques;
	using  technique       = effect::technique;
	using  parameter       = effect::parameter;
    using  parameters      = effect::parameters;
    using  parameter_type  = effect::parameter_type;
    using  parameter_queue = effect::parameter_queue;
	using  effect_parser   = parser::effect_parser;

	//struct by type
	struct parameter_texture : public effect::parameter
	{
		//value
		texture::ptr m_value;
		//init
		parameter_texture()
		{
			m_value = texture::ptr(nullptr);
			m_type = effect::PT_TEXTURE;
		}
		parameter_texture(texture::ptr value)
		{
			m_value = value;
			m_type = effect::PT_TEXTURE;
		}
		//set value
		virtual void set_value(texture::ptr value)
		{
			m_value = value;
		}
		//get value
		virtual texture::ptr get_texture() const 
		{ 
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_texture(m_value);
		}
	};
	struct parameter_int : public effect::parameter 
	{
		//value
		int m_value;
		//init
		parameter_int(int value = 0)
		{
			m_value = value;
			m_type  = effect::PT_INT;
		}
		//set value
		virtual void set_value(int i)
		{
			m_value = i;
		}
		//get value
		virtual int get_int() const
		{
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_int(m_value);
		}
	};
	struct parameter_float : public effect::parameter
	{
		//value
		float m_value;
		//init
		parameter_float(float value = 0.0f)
		{
			m_value = value;
			m_type  = effect::PT_FLOAT;
		}
		//set value
		virtual void set_value(float f)
		{
			m_value = f;
		}
		//get value
		virtual float get_float() const 
		{
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_float(m_value);
		}
	};
	struct parameter_vec2 : public effect::parameter
	{
		//value
		vec2 m_value;
		//init
		parameter_vec2()
		{
			m_type = effect::PT_VEC2;
		}
		parameter_vec2(const vec2& value)
		{
			m_value = value;
			m_type  = effect::PT_VEC2;
		}
		//set value
		virtual void set_value(const vec2& value)
		{
			m_value = value;
		}
		//get value
		virtual const vec2& get_vec2() const
		{
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_vec2(m_value);
		}
	};
	struct parameter_vec3 : public effect::parameter
	{
		//value
		vec3 m_value;
		//init
		parameter_vec3()
		{
			m_type = effect::PT_VEC3;
		}
		parameter_vec3(const vec3& value)
		{
			m_value = value;
			m_type  = effect::PT_VEC3;
		}
		//set value
		virtual void set_value(const vec3& value)
		{
			m_value = value;
		}
		//get value
		virtual const vec3& get_vec3() const
		{
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_vec3(m_value);
		}
	};
	struct parameter_vec4 : public effect::parameter
	{
		//value
		vec4 m_value;
		//init
		parameter_vec4()
		{
			m_type = effect::PT_VEC4;
		}
		parameter_vec4(const vec4& value)
		{
			m_value = value;
			m_type  = effect::PT_VEC4;
		}
		//set value
		virtual void set_value(const vec4& value)
		{
			m_value = value;
		}
		//get value
		virtual const vec4& get_vec4() const
		{
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_vec4(m_value);
		}
	};
	struct parameter_mat4 : public effect::parameter
	{
		//value
		mat4 m_value;
		//init
		parameter_mat4()
		{
			m_type = effect::PT_MAT4;
		}
		parameter_mat4(const mat4& value)
		{
			m_value = value;
			m_type  = effect::PT_MAT4;
		}
		//set value
		virtual void set_value(const mat4& value)
		{
			m_value = value;
		}
		//get value
		virtual const mat4& get_mat4() const
		{
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_mat4(m_value);
		}
	};
    

	//enable pass effect
	void effect::pass::bind
	(
		camera::wptr   c_camera,
		transform_wptr c_camera_transform,
		transform_wptr c_model_transform,
		parameters* params
	) const
	{
		//bind
		bind(params);
		//default uniforms
		if (m_uniform_camera.is_valid())
		{
			m_uniform_camera.uniform(c_camera, c_camera_transform.lock()->get_matrix());
		}
		if (m_uniform_transform.is_valid())
		{
			m_uniform_transform.uniform(c_model_transform);
		}
	}

	void effect::pass::bind(parameters* params) const
	{
		render::set_blend_state(m_blend);
		render::set_cullface_state(m_cullface);
		render::set_depth_buffer_state(m_depth);
		m_shader->bind();
		//if null use default
		if (!params) params = &m_effect->m_parameters;
		//uniform
		for (size_t i = 0; i != m_uniform.size(); ++i)
		{
			auto& param = (*params)[m_param_id[i]];
			//uniform value
			switch (param->get_type())
			{
			default:
			case PT_TEXTURE_ARRAY:
			case PT_NONE:
				/* void */
				break;
				//uniform
			case PT_INT: m_uniform[i]->set_value(param->get_int()); break;
			case PT_FLOAT: m_uniform[i]->set_value(param->get_float()); break;
			case PT_TEXTURE: m_uniform[i]->set_value(param->get_texture()); break;
			case PT_VEC2: m_uniform[i]->set_value(param->get_vec2()); break;
			case PT_VEC3: m_uniform[i]->set_value(param->get_vec3()); break;
			case PT_VEC4: m_uniform[i]->set_value(param->get_vec4()); break;
			case PT_MAT4: m_uniform[i]->set_value(param->get_mat4()); break;
			case PT_INT_ARRAY: m_uniform[i]->set_value(param->get_int_array()); break;
			case PT_FLOAT_ARRAY: m_uniform[i]->set_value(param->get_float_array()); break;
			case PT_VEC2_ARRAY: m_uniform[i]->set_value(param->get_vec2_array()); break;
			case PT_VEC3_ARRAY: m_uniform[i]->set_value(param->get_vec3_array()); break;
			case PT_VEC4_ARRAY: m_uniform[i]->set_value(param->get_vec4_array()); break;
			case PT_MAT4_ARRAY: m_uniform[i]->set_value(param->get_mat4_array()); break;
			}
		}
	}

	//disable pass effect
	void effect::pass::unbind()
	{
		m_shader->unbind();
	}

	//safe enable pass effect
	render_state effect::pass::safe_bind
	(
		camera::wptr   c_camera,
		transform_wptr c_camera_transform,
		transform_wptr c_model_transform,
		parameters* params
	) const
	{
		render_state state = render::get_render_state();
		bind(c_camera, c_camera_transform, c_model_transform,params);
		return state;
	}

	render_state effect::pass::safe_bind(parameters* params) const
	{
		render_state state = render::get_render_state();
		bind(params);
		return state;
	}
	//safe disable pass effect
	void effect::pass::safe_unbind(const render_state& state)
	{
		render::set_render_state(state);
		unbind();
	}

    enum shader_define_rendering
    {
        DEF_RENDERING_COLOR,
        DEF_RENDERING_AMBIENT_LIGHT,
        DEF_RENDERING_DIRECTION_LIGHT,
        DEF_RENDERING_POINT_LIGHT,
        DEF_RENDERING_SPOT_LIGHT
    };
    
    const std::string shader_define_table[]=
    {
        "RENDERING_COLOR",
        "RENDERING_AMBIENT_LIGHT",
        "RENDERING_DIRECTION_LIGHT",
        "RENDERING_POINT_LIGHT",
        "RENDERING_SPOT_LIGHT"
    };
    
	//load effect
	bool effect::load(resources_manager& resources, const std::string& path)
	{
		//parser
		effect_parser		   e_parser;
		effect_parser::context e_context;
		//do parsing
		if(!e_parser.parse(e_context, filesystem::text_file_read_all(path)))
		{
			std::cout << "Effect: "
					  << path
					  << std::endl
					  << e_context.errors_to_string()
					  << std::endl;
			return false;
        }
        //alloc params
        m_parameters.resize(e_context.m_parameters.size());
		//add params
		for (size_t i = 0; i != m_parameters.size(); ++i)
		{
			switch (e_context.m_parameters[i].m_type)
			{
			case parameter_type::PT_INT:    m_parameters[i] = std::unique_ptr< parameter >(new parameter_int(e_context.m_parameters[i].m_value.m_int));     break;
			case parameter_type::PT_FLOAT:  m_parameters[i] = std::unique_ptr< parameter >(new parameter_float(e_context.m_parameters[i].m_value.m_float)); break;
			case parameter_type::PT_VEC2:   m_parameters[i] = std::unique_ptr< parameter >(new parameter_vec2(e_context.m_parameters[i].m_value.m_vec2));   break;
			case parameter_type::PT_VEC3:   m_parameters[i] = std::unique_ptr< parameter >(new parameter_vec3(e_context.m_parameters[i].m_value.m_vec3));   break;
			case parameter_type::PT_VEC4:   m_parameters[i] = std::unique_ptr< parameter >(new parameter_vec4(e_context.m_parameters[i].m_value.m_vec4));   break;
			case parameter_type::PT_MAT4:   m_parameters[i] = std::unique_ptr< parameter >(new parameter_mat4(e_context.m_parameters[i].m_value.m_mat4));   break;
			case parameter_type::PT_TEXTURE:m_parameters[i] = std::unique_ptr< parameter >(new parameter_texture(resources.get_texture(e_context.m_parameters[i].m_value.m_texture))); break;
			default: assert(0); break;
			}
			//save id
			m_parameters[i]->m_id = (int)i;
			//add into map
			m_map_parameters[e_context.m_parameters[i].m_name] = (int)i;
        }
        //ref to sub effect
        effect_parser::sub_effect_field* ptr_sub_effect = nullptr;
        //test sub effect
        for(effect_parser::sub_effect_field& sub_effect : e_context.m_sub_effect )
        {
            if(sub_effect.m_requirement.test())
            {
                ptr_sub_effect = &sub_effect;
                break;
            }
        }
        //fail
        if(!ptr_sub_effect)
        {
            std::cout << "Effect: "
            << path
            << std::endl
            << "All sub effects unsupported."
            << std::endl;
            return false;
        }
        //set queue
        set_queue(ptr_sub_effect->m_queue);
        //n_pass
        size_t n_techniques_parser = ptr_sub_effect->m_techniques.size();
		//add tech
		for (size_t t = 0; t != n_techniques_parser; ++t)
		{
			//add into map
            technique& this_technique = m_map_techniques[ptr_sub_effect->m_techniques[t].m_name];
            //n pass
            size_t n_pass_parser = ptr_sub_effect->m_techniques[t].m_pass.size();
			//alloc pass
			this_technique.reserve(n_pass_parser);
			//add pass
			for (size_t p = 0; p != n_pass_parser; ++p)
            {
                //ref
                effect_parser::pass_field& parser_pass = ptr_sub_effect->m_techniques[t].m_pass[p];
                //lights sub pass
                int light_sub_pass = parser_pass.m_lights;
                //Type render
                shader_define_rendering current_shader_def = DEF_RENDERING_COLOR;
                //pass
                while(light_sub_pass)
                {
                    //sub light
                    if(light_sub_pass & effect_parser::LF_BASE)
                    {
                        current_shader_def = DEF_RENDERING_COLOR;
                        light_sub_pass    ^= effect_parser::LF_BASE;
                    }
                    else if(light_sub_pass & effect_parser::LF_AMBIENT)
                    {
                        current_shader_def = DEF_RENDERING_AMBIENT_LIGHT;
                        light_sub_pass    ^= effect_parser::LF_AMBIENT;
                    }
                    else if(light_sub_pass & effect_parser::LF_DIRECTION)
                    {
                        current_shader_def = DEF_RENDERING_DIRECTION_LIGHT;
                        light_sub_pass    ^= effect_parser::LF_DIRECTION;
                    }
                    else if(light_sub_pass & effect_parser::LF_POINT)
                    {
                        current_shader_def = DEF_RENDERING_POINT_LIGHT;
                        light_sub_pass    ^= effect_parser::LF_POINT;
                    }
                    else if(light_sub_pass & effect_parser::LF_SPOT)
                    {
                        current_shader_def = DEF_RENDERING_SPOT_LIGHT;
                        light_sub_pass    ^= effect_parser::LF_SPOT;
                    }
                    //add pass
                    this_technique.push_back(effect::pass());
                    //pass
                    effect::pass& this_pass = this_technique[this_technique.size()-1];
                    //get all values
                    this_pass.m_effect   = this;
                    this_pass.m_blend    = parser_pass.m_blend;
                    this_pass.m_cullface = parser_pass.m_cullface;
                    this_pass.m_depth    = parser_pass.m_depth;
                    //shader
                    if (parser_pass.m_shader.m_name)
                    {
                        this_pass.m_shader = resources.get_shader(parser_pass.m_shader.m_text);
                    }
                    else
                    {
                        shader::preprocess_map shader_defines
                        {
                            { "version", std::to_string(ptr_sub_effect->m_requirement.m_shader_version) },
                            { "define" , shader_define_table[current_shader_def]                        }
                        };
                        //shader
                        this_pass.m_shader = shader::snew();
                        //load effect
                        if (!this_pass.m_shader->load_effect
                            (parser_pass.m_shader.m_text,
                             path,
                             shader_defines,
                             parser_pass.m_shader.m_line - 1
                            ))
                        {
                            //preproc, debug
                            std::string debug_preproc;
                            for(const shader::preprocess_element& preproc : shader_defines)
                            {
                                debug_preproc += "#"+std::get<0>(preproc)+" "+std::get<1>(preproc)+"\n";
                            }
                            //output
                            std::cout << "Effect: "
                                      << path
                                      << std::endl
                                      << "Error from technique: " << ptr_sub_effect->m_techniques[t].m_name << ", pass["<< p << "] "
                                      << debug_preproc
                                      << std::endl;

                        }
                    }
                    //default uniform
					this_pass.m_uniform_camera.get_uniform(this_pass.m_shader);
					this_pass.m_uniform_transform.get_uniform(this_pass.m_shader);
					//default true
                    this_pass.m_support_light = true;
                    //lights uniforms
                    switch (current_shader_def)
                    {
                        case DEF_RENDERING_SPOT_LIGHT:
                            this_pass.m_uniform_spot.get_uniform(this_pass.m_shader);
                        break;
                        case DEF_RENDERING_POINT_LIGHT:
                            this_pass.m_uniform_point.get_uniform(this_pass.m_shader);
                        break;
                        case DEF_RENDERING_DIRECTION_LIGHT:
                            this_pass.m_uniform_direction.get_uniform(this_pass.m_shader);
                        break;
                        case DEF_RENDERING_AMBIENT_LIGHT:
                            this_pass.m_uniform_ambient_light = this_pass.m_shader->get_uniform("ambient_light");
                            break;
                        default:
                            this_pass.m_support_light = false;
                            break;
                    }
                    //get uniforms
                    for (auto it : m_map_parameters)
                    {
                        //get
                        uniform* u_shader = this_pass.m_shader->get_uniform(it.first.c_str());
                        //test
                        if (u_shader)
                        {
                            //push
                            this_pass.m_param_id.push_back(it.second);
                            this_pass.m_uniform.push_back(u_shader);
                        }
                    }
                }
			}
            
		}
		
		return true;
	}

	//get technique
	effect::technique* effect::get_technique(const std::string& technique_name)
	{
		auto it_tech = m_map_techniques.find(technique_name);
		if (it_tech != m_map_techniques.end()) return &it_tech->second;
		return nullptr;
	}
	 
	//get parameter
	effect::parameter* effect::get_parameter(int parameter_id)
	{
		if(m_parameters.size() > (size_t)parameter_id)
			return m_parameters[parameter_id].get();
		return nullptr;
	}

	effect::parameter* effect::get_parameter(const std::string& parameter_name)
	{
		auto it_param = m_map_parameters.find(parameter_name);
		if(it_param != m_map_parameters.end()) return get_parameter(it_param->second);
		return nullptr;
	}

	effect::parameters* effect::copy_all_parameters()
	{
		//alloc vector
		auto new_params = new parameters(m_parameters.size());
		//copy all
		for (size_t i = 0; i != m_parameters.size(); ++i)
		{
			(*new_params)[i] = std::unique_ptr< parameter >( m_parameters[i]->copy() );
		}
		return new_params;
	}

	//get id
	int effect::get_parameter_id(const std::string& parameter_name)
	{
		auto it_param = m_map_parameters.find(parameter_name);
		if (it_param != m_map_parameters.end()) return it_param->second;
		return -1;
	}

}
