#include <light.h>
#include <vector_math.h>
#include <entity.h>
#include <transform.h>

namespace hcube
{

	//UNIFORM SPOT LIGHT
	void uniform_light_spot::get_uniform(int i, shader::ptr shader)
	{
		std::string lights_i("spot_lights[" + std::to_string(i) + "]");
		m_uniform_position = shader->get_uniform((lights_i + ".m_position").c_str());
		m_uniform_direction = shader->get_uniform((lights_i + ".m_direction").c_str());

		m_uniform_diffuse = shader->get_uniform((lights_i + ".m_diffuse").c_str());
		m_uniform_specular = shader->get_uniform((lights_i + ".m_specular").c_str());

		m_uniform_constant      = shader->get_uniform((lights_i + ".m_constant").c_str());
		m_uniform_inside_radius = shader->get_uniform((lights_i + ".m_inside_radius").c_str());
		m_uniform_radius        = shader->get_uniform((lights_i + ".m_radius").c_str());

		m_uniform_inner_cut_off = shader->get_uniform((lights_i + ".m_inner_cut_off").c_str());
		m_uniform_outer_cut_off = shader->get_uniform((lights_i + ".m_outer_cut_off").c_str());

		m_uniform_use_shadow = shader->get_uniform((lights_i + ".m_use_shadow").c_str());

		m_uniform_shadow_projection = shader->get_uniform((lights_i + ".m_shadow_projection").c_str());
		m_uniform_shadow_view = shader->get_uniform((lights_i + ".m_shadow_view").c_str());
		m_uniform_shadow_map = shader->get_uniform((lights_i + ".m_shadow_map").c_str());

        //test
        m_valid =
		   m_uniform_position
        && m_uniform_direction
        && m_uniform_diffuse
        && m_uniform_specular
        && m_uniform_constant
        && m_uniform_inside_radius
        && m_uniform_radius
        && m_uniform_inner_cut_off
        && m_uniform_outer_cut_off
		&& m_uniform_use_shadow
		&& m_uniform_shadow_projection
		&& m_uniform_shadow_view
		&& m_uniform_shadow_map;
	}

	void uniform_light_spot::uniform(light_wptr weak_light,
							  	     const mat4& view,
								     const mat4& model)
	{
        if(!is_valid()) return;
        
		auto light = weak_light.lock();
		m_uniform_position->set_value((vec3)(view * model * vec4(0, 0, 0, 1.0)));
		m_uniform_direction->set_value((vec3)(view * model * vec4(0, 0, 1.0, 0.0)));
		m_uniform_diffuse->set_value(light->m_diffuse);
		m_uniform_specular->set_value(light->m_specular);
		m_uniform_constant->set_value(light->m_constant);
		m_uniform_inside_radius->set_value(light->m_inside_radius);
		m_uniform_radius->set_value(light->m_radius);
		m_uniform_inner_cut_off->set_value(light->m_inner_cut_off);
		m_uniform_outer_cut_off->set_value(light->m_outer_cut_off);
		m_uniform_use_shadow->set_value(light->is_enable_shadow() ? 1 : 0);

		if (light->is_enable_shadow())
		{
			m_uniform_shadow_projection->set_value(light->get_projection());
			m_uniform_shadow_view->set_value(light->get_view());
			m_uniform_shadow_map->set_value(light->get_shadow_buffer().get_depth_texture());
		}
    }
    
    bool uniform_light_spot::is_valid() const
    {
        return m_valid;
    }

	//UNIFORM POINT LIGHT
	void uniform_light_point::get_uniform(int i, shader::ptr shader)
	{
		std::string lights_i("point_lights[" + std::to_string(i) + "]");

		m_uniform_position = shader->get_uniform((lights_i + ".m_position").c_str());

		m_uniform_diffuse = shader->get_uniform((lights_i + ".m_diffuse").c_str());
		m_uniform_specular = shader->get_uniform((lights_i + ".m_specular").c_str());

		m_uniform_constant = shader->get_uniform((lights_i + ".m_constant").c_str());
		m_uniform_inside_radius = shader->get_uniform((lights_i + ".m_inside_radius").c_str());
		m_uniform_radius = shader->get_uniform((lights_i + ".m_radius").c_str());


		//test
		m_valid =
			   m_uniform_position
			&& m_uniform_diffuse
			&& m_uniform_specular
			&& m_uniform_constant
			&& m_uniform_inside_radius
			&& m_uniform_radius;
	}

	void uniform_light_point::uniform(light_wptr weak_light,
									  const mat4& view,
									  const mat4& model)
	{
		if (!is_valid()) return;

		auto light = weak_light.lock();
		m_uniform_position->set_value((vec3)(view * model * vec4(0, 0, 0, 1.0)));
		m_uniform_diffuse->set_value(light->m_diffuse);
		m_uniform_specular->set_value(light->m_specular);
		m_uniform_constant->set_value(light->m_constant);
		m_uniform_inside_radius->set_value(light->m_inside_radius);
		m_uniform_radius->set_value(light->m_radius);
	}

	bool uniform_light_point::is_valid() const
	{
		return m_valid;
	}

	//UNIFORM POINT DIRECTION
	void uniform_light_direction::get_uniform(int i, shader::ptr shader)
	{
		std::string lights_i("direction_lights[" + std::to_string(i) + "]");

		m_uniform_direction = shader->get_uniform((lights_i + ".m_direction").c_str());

		m_uniform_diffuse = shader->get_uniform((lights_i + ".m_diffuse").c_str());
		m_uniform_specular = shader->get_uniform((lights_i + ".m_specular").c_str());
		
		//test
		m_valid =
			   m_uniform_direction
			&& m_uniform_diffuse
			&& m_uniform_specular;
	}

	void uniform_light_direction::uniform(light_wptr weak_light,
										  const mat4& view,
										  const mat4& model)
	{
		if (!is_valid()) return;

		auto light = weak_light.lock();
		m_uniform_direction->set_value((vec3)(view * model * vec4(0, 0, 1.0, 0.0)));
		m_uniform_diffuse->set_value(light->m_diffuse);
		m_uniform_specular->set_value(light->m_specular);
	}

	bool uniform_light_direction::is_valid() const
	{
		return m_valid;
	}

	//LIGHT CLASS
	component_ptr light::copy() const
	{
		light_ptr new_light = light_snew(light{
			m_type,
			m_diffuse,
			m_specular,
			m_constant,
			m_inside_radius,
			m_radius ,
			m_inner_cut_off,
			m_outer_cut_off
		});
		//add shadow
		if (is_enable_shadow())
		{
			new_light->set_shadow(m_shadow.m_buffer.get_size());
		}
		//return
		return new_light;
	}

	//shadow
	bool light::set_shadow(const ivec2& size)
	{
		if (m_type == light::DIRECTION_LIGHT) return false;
		//set to true
		m_shadow.m_enable = true;
		m_shadow.m_buffer.init(size, m_type == light::POINT_LIGHT);
		//viewport
		m_shadow.m_viewport = ivec4{ 0, 0, size.x, size.y };
		//update projection
		update_projection_matrix();
		//end
		return true;
	}
  
	void light::disable_shadow()
	{
		if (m_shadow.m_enable)
		{
			m_shadow.m_enable = false;
			m_shadow.m_buffer.destoy();
		}
	}

	bool light::is_enable_shadow() const
	{
		return m_shadow.m_enable;
	}

	const ivec4& light::get_viewport() const
	{
		return m_shadow.m_viewport;
	}
    
    const mat4& light::get_projection() const
    {
        return m_projection;
    }
    
    const mat4& light::get_view()
	{
		update_view_matrix();
		return m_view;
	}

	const std::vector<mat4>& light::get_cube_view()
	{
		update_view_matrix();
		return m_cube_view;
	}

	void light::on_attach(entity& entity)
	{
		m_view_is_dirty = true;
	}

	bool light::on_activate()
	{
		m_view_is_dirty = true;
		return true;
	}

	void light::on_message(const message& message)
	{
		if (message.m_id == transform::MSG_DIRTY) m_view_is_dirty = true;
	}


	const frustum& light::get_frustum() const
	{
		return m_frustum;
	}

	const frustum& light::update_frustum()
	{
        m_frustum.update_frustum(m_projection*get_view());
		return m_frustum;
	}

	const shadow_buffer& light::get_shadow_buffer() const
	{
		return m_shadow.m_buffer;
	}

	void light::update_projection_matrix()
	{
		//aspect
		float aspect = 1.0f;
		//shadow texture?
		if (m_shadow.m_enable)
		{
			//get size
			ivec2 size = m_shadow.m_buffer.get_size();
			//projection
			aspect = float(size.x) / float(size.y);
		}
		//update
		switch (m_type)
		{
		case hcube::light::DIRECTION_LIGHT:
			m_projection = hcube::perspective(radians(180.0f), aspect, 0.1f, m_radius);
			break;
		case hcube::light::POINT_LIGHT:
			m_projection = hcube::perspective(radians(90.0f), aspect, 0.1f, m_radius);
			break;
		case hcube::light::SPOT_LIGHT:
			m_projection = hcube::perspective(m_outer_cut_off * 2.0f, aspect, 0.1f, m_radius);
			break;
		default:
			break;
		}
	}

	void light::update_view_matrix()
	{
		if (!m_view_is_dirty) return;

		entity*       e_light = get_entity();
		transform_ptr t_light = nullptr;

		switch (m_type)
		{
		case hcube::light::POINT_LIGHT:
		{
			m_cube_view.resize(0);
			m_cube_view.reserve(6);
			//position
			vec3 l_position;
			//get position
			if (e_light && (t_light = e_light->get_component<transform>()))
			{
				l_position = t_light->get_global_position();
			}
            //(+x -x +y -y +z -z)^-1
            m_cube_view.push_back(glm::lookAt(l_position, l_position + glm::vec3(-1.0,0.0, 0.0),  glm::vec3(0.0,  1.0,  0.0)));
			m_cube_view.push_back(glm::lookAt(l_position, l_position + glm::vec3( 1.0,0.0, 0.0),  glm::vec3(0.0,  1.0,  0.0)));
			m_cube_view.push_back(glm::lookAt(l_position, l_position + glm::vec3(0.0, 1.0, 0.0),  glm::vec3(0.0,  0.0, -1.0)));
			m_cube_view.push_back(glm::lookAt(l_position, l_position + glm::vec3(0.0,-1.0, 0.0),  glm::vec3(0.0,  0.0,  1.0)));
			m_cube_view.push_back(glm::lookAt(l_position, l_position + glm::vec3(0.0, 0.0, 1.0),  glm::vec3(0.0,  1.0,  0.0)));
			m_cube_view.push_back(glm::lookAt(l_position, l_position + glm::vec3(0.0, 0.0,-1.0),  glm::vec3(0.0,  1.0,  0.0)));
			//
			for (mat4& view : m_cube_view)
			{
				//OpenGL LH, z*-1
				view[2] *= -1;
			}
		}
		break;
		case hcube::light::SPOT_LIGHT:
		{
			if (!e_light || !(t_light = e_light->get_component<transform>()))
			{
				m_view = mat4(1);
				m_view_is_dirty = false;
			}
			//get matrix
			vec3 scale = t_light->get_global_scale();
			mat4 view  = t_light->get_matrix();
			//remove scale
			view[0][0] /= scale[0];
			view[0][1] /= scale[0];
			view[0][2] /= scale[0];

			view[1][0] /= scale[1];
			view[1][1] /= scale[1];
			view[1][2] /= scale[1];

			view[2][0] /= scale[2];
			view[2][1] /= scale[2];
			view[2][2] /= scale[2];
			//OpenGL LH, z*-1
			view[2] *= -1;
			//M^-1 = V
			m_view = inverse(view);
		}
		break;
		default: break;
		}
		//to false
		m_view_is_dirty = false;
	}
}