#include <light.h>

namespace hcube
{
	void uniform_light::get_uniform(int i, shader::ptr shader)
	{
		std::string lights_i("lights[" + std::to_string(i) + "]");
		m_uniform_type = shader->get_uniform((lights_i + ".m_type").c_str());

		m_uniform_position = shader->get_uniform((lights_i + ".m_position").c_str());
		m_uniform_direction = shader->get_uniform((lights_i + ".m_direction").c_str());

		m_uniform_diffuse = shader->get_uniform((lights_i + ".m_diffuse").c_str());
		m_uniform_specular = shader->get_uniform((lights_i + ".m_specular").c_str());

		m_uniform_constant = shader->get_uniform((lights_i + ".m_inv_constant").c_str());
		m_uniform_linear = shader->get_uniform((lights_i + ".m_inv_quad_linear").c_str());
		m_uniform_quadratic = shader->get_uniform((lights_i + ".m_inv_quad_quadratic").c_str());

		m_uniform_inner_cut_off = shader->get_uniform((lights_i + ".m_inner_cut_off").c_str());
		m_uniform_outer_cut_off = shader->get_uniform((lights_i + ".m_outer_cut_off").c_str());
	}

	void uniform_light::uniform(light_wptr weak_light, const glm::mat4& view, const glm::mat4& model)
	{
		auto light = weak_light.lock();
		m_uniform_type->set_value(light->m_type);
		m_uniform_position->set_value((glm::vec3)(view * model * glm::vec4(0, 0, 0, 1.0)));
		m_uniform_direction->set_value((glm::vec3)(view * model * glm::vec4(0, 0, 1.0, 0.0)));
		m_uniform_diffuse->set_value(light->m_diffuse);
		m_uniform_specular->set_value(light->m_specular);
		m_uniform_constant->set_value(1.f / light->m_constant);
		m_uniform_linear->set_value(1.f / (light->m_linear*light->m_linear));
		m_uniform_quadratic->set_value(1.f / (light->m_quadratic*light->m_quadratic));
		m_uniform_inner_cut_off->set_value(light->m_inner_cut_off);
		m_uniform_outer_cut_off->set_value(light->m_outer_cut_off);
	}
}