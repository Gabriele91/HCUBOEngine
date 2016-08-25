#include <light.h>

namespace hcube
{

	component_ptr light::copy() const
	{
		return light_snew(light{
			m_type,
			m_diffuse,
			m_specular,
			m_constant,
			m_inside_radius,
			m_radius ,
			m_inner_cut_off,
			m_outer_cut_off
		});
	}

	void uniform_light::get_uniform(int i, shader::ptr shader)
	{
		std::string lights_i("lights[" + std::to_string(i) + "]");
		m_uniform_type = shader->get_uniform((lights_i + ".m_type").c_str());

		m_uniform_position = shader->get_uniform((lights_i + ".m_position").c_str());
		m_uniform_direction = shader->get_uniform((lights_i + ".m_direction").c_str());

		m_uniform_diffuse = shader->get_uniform((lights_i + ".m_diffuse").c_str());
		m_uniform_specular = shader->get_uniform((lights_i + ".m_specular").c_str());

		m_uniform_inv_constant           = shader->get_uniform((lights_i + ".m_inv_constant").c_str());
		m_uniform_inv_quad_inside_radius = shader->get_uniform((lights_i + ".m_inv_quad_inside_radius").c_str());
		m_uniform_inv_quad_radius        = shader->get_uniform((lights_i + ".m_inv_quad_radius").c_str());

		m_uniform_inner_cut_off = shader->get_uniform((lights_i + ".m_inner_cut_off").c_str());
		m_uniform_outer_cut_off = shader->get_uniform((lights_i + ".m_outer_cut_off").c_str());
        
        //test
        m_valid =
           m_uniform_type
        && m_uniform_position
        && m_uniform_direction
        && m_uniform_diffuse
        && m_uniform_specular
        && m_uniform_inv_constant
        && m_uniform_inv_quad_inside_radius
        && m_uniform_inv_quad_radius
        && m_uniform_inner_cut_off
        && m_uniform_outer_cut_off;
	}

	void uniform_light::uniform(light_wptr weak_light, const glm::mat4& view, const glm::mat4& model)
	{
        if(!is_valid()) return;
        
		auto light = weak_light.lock();
		m_uniform_type->set_value(light->m_type);
		m_uniform_position->set_value((glm::vec3)(view * model * glm::vec4(0, 0, 0, 1.0)));
		m_uniform_direction->set_value((glm::vec3)(view * model * glm::vec4(0, 0, 1.0, 0.0)));
		m_uniform_diffuse->set_value(light->m_diffuse);
		m_uniform_specular->set_value(light->m_specular);
		m_uniform_inv_constant->set_value(1.f / light->m_constant);
		m_uniform_inv_quad_inside_radius->set_value(1.f / (light->m_inside_radius*light->m_inside_radius));
		m_uniform_inv_quad_radius->set_value(1.f / (light->m_radius*light->m_radius));
		m_uniform_inner_cut_off->set_value(light->m_inner_cut_off);
		m_uniform_outer_cut_off->set_value(light->m_outer_cut_off);
    }
    
    bool uniform_light::is_valid() const
    {
        return m_valid;
    }
}