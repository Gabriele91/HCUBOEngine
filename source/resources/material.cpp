//
//  material.cpp
//  HCubo
//
//  Created by Gabriele on 21/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <list>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <hcube/core/filesystem.h>
#include <hcube/core/resources_manager.h>
#include <hcube/resources/material.h>
#include <hcube/data/parser/material_parser.h>

namespace hcube
{

	material::~material() { if (m_parameters) delete m_parameters; };

	static std::string name_of_default_parameters[material::MAT_DEFAULT_MAX] =
	{
		"diffuse_map",
		"normal_map",
		"specular_map",
		"color",
        "mask"
	};
	static void save_if_is_a_default_parameter(int id_param,const std::string& name, int default_parameters[])
	{
		for (int i = 0; i != material::MAT_DEFAULT_MAX; ++i)
		{
			if (name_of_default_parameters[i] == name)
			{
				default_parameters[i] = id_param;
				return;
			}
		}
	}

	bool material::load(resources_manager& resources,const std::string& path)
	{
		//using namespace
		using namespace parser;
		//parse
		material_parser parser;
		material_parser::context context;
		//do parsing
		if(!parser.parse(context, filesystem::text_file_read_all(path)))
		{
			std::cout << "Material: "
					  << path
					  << std::endl
					  << context.errors_to_string()
					  << std::endl;
			return false;
		}
		//load effect
		m_effect = resources.get_effect(context.m_effect.m_name);
		//load params
		if (m_effect)
		{
			//copy context
			m_parameters = m_effect->copy_all_parameters();
			//set local params
			for (size_t i = 0; i != context.m_effect.m_parameters.size(); ++i)
			{
				//get field
				material_parser::parameter_field & ctx_param = context.m_effect.m_parameters[i];
				//get id
				int   id_param                               = m_effect->get_parameter_id(ctx_param.m_name);
				//test
				if (id_param < 0) continue;
				//get pram
				effect::parameter* param                     = (*m_parameters)[id_param].get();
				//select
				switch (ctx_param.m_type)
				{
				default:
				case effect::PT_TEXTURE_ARRAY:
				case effect::PT_NONE:
					/* void */
				break;
				case effect::PT_INT:    param->set_value(ctx_param.m_value.m_int); break;
				case effect::PT_FLOAT:  param->set_value(ctx_param.m_value.m_float); break;
				case effect::PT_TEXTURE:param->set_value(resources.get_texture(ctx_param.m_value.m_texture)); break;
				case effect::PT_VEC2:	param->set_value(ctx_param.m_value.m_vec2); break;
				case effect::PT_VEC3:	param->set_value(ctx_param.m_value.m_vec3); break;
				case effect::PT_VEC4:	param->set_value(ctx_param.m_value.m_vec4); break;
				case effect::PT_MAT4:	param->set_value(ctx_param.m_value.m_mat4); break;
				}
				//if a default parameter
				save_if_is_a_default_parameter(id_param, ctx_param.m_name, m_default_parameters);
			}
		}
		return true;
	}
	
	effect::ptr	material::get_effect() const { return m_effect; }

	effect::parameters* material::get_parameters() const { return m_parameters; }
	
	const effect::parameter* material::get_default_parameter(material::default_parameters dp) const
	{
		int id_param = m_default_parameters[dp];
		if(id_param>=0) return (*m_parameters)[ m_default_parameters[dp] ].get();
		return nullptr;
	}

	const effect::parameter* material::get_parameter_by_name(const std::string& name) const
	{
		if (!m_effect) return nullptr;
		//get id
		int   id_param = m_effect->get_parameter_id(name);
		//return param
		return (*m_parameters)[id_param].get();
	}	

	effect::parameter* material::get_default_parameter(material::default_parameters dp)
	{
		int id_param = m_default_parameters[dp];
		if (id_param >= 0) return (*m_parameters)[m_default_parameters[dp]].get();
		return nullptr;
	}

	effect::parameter* material::get_parameter_by_name(const std::string& name)
	{
		if (!m_effect) return nullptr;
		//get id
		int   id_param = m_effect->get_parameter_id(name);
		//return param
		return (*m_parameters)[id_param].get();
	}


	material_ptr material::copy() const
	{
		auto omaterial = material_snew();
		//effect
		omaterial->m_effect = m_effect;
		//parameters alloc
		omaterial->m_parameters = new effect::parameters(m_parameters->size());
		//copy
		for (size_t i = 0; i != m_parameters->size(); ++i)
		{
			(*omaterial->m_parameters)[i] = std::unique_ptr< effect::parameter >((*m_parameters)[i]->copy());
		}
		//return
		return omaterial;
	}
}