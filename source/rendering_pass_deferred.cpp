#include <rendering_pass_deferred.h>
#include <basic_meshs.h>
#include <transform.h>


namespace hcube
{
	rendering_pass_deferred::rendering_pass_deferred(const ivec2& w_size, resources_manager& resources)
	{
		m_q_size = w_size;
		m_g_buffer.init(w_size);
		m_ssao.init(w_size, resources);
		m_ssao.set_kernel_size(8);
		m_ssao.set_radius(2.0);

		m_square = basic_meshs::square3D({ 2.0,2.0 }, true);
		m_shader = resources.get_shader("deferred_light");
		m_position = m_shader->get_uniform("g_position");
		m_normal = m_shader->get_uniform("g_normal");
		m_albedo = m_shader->get_uniform("g_albedo_spec");
		m_occlusion = m_shader->get_uniform("g_occlusion");
		m_view = m_shader->get_uniform("view");
		//uniforms
		m_ambient_light = m_shader->get_uniform("ambient_light");
		m_uniform_n_lights_used = m_shader->get_uniform("n_lights_used");
		//alloc lights
		m_uniform_lights.resize(m_max_lights);
		m_uniform_shadow_lights.resize(m_max_lights);
		//get unfirom lights
		for (unsigned i = 0; i != m_max_lights; ++i)
		{
			m_uniform_lights[i].get_uniform(i, m_shader);
			m_uniform_shadow_lights[i].get_uniform(i, m_shader);
		}
	}

	void rendering_pass_deferred::set_ambient_occlusion(bool enable)
	{
		if (m_enable_ambient_occlusion && !enable)
		{
			m_ssao.clear();
		}
		m_enable_ambient_occlusion = enable;
	}

	bool rendering_pass_deferred::is_enable_ambient_occlusion() const
	{
		return m_enable_ambient_occlusion;
	}

	void rendering_pass_deferred::draw_pass(vec4&  clear_color,
											vec4&  ambient_color,
											entity::ptr e_camera,
											render_queues& queues)
	{
		//camera
		camera::ptr   c_camera = e_camera->get_component<camera>();
		transform_ptr t_camera = e_camera->get_component<transform>();
		const vec4& viewport = c_camera->get_viewport();
		//buffer
		m_g_buffer.bind();
		//set state
		render::set_viewport_state({ viewport });
		render::set_clear_color_state(vec4{ clear_color.r, clear_color.g, clear_color.b,1.0 });
		render::clear();
		//save state
		auto render_state = render::get_render_state();
		//draw
		for (render_queues::element*
			weak_element = queues.m_cull_opaque;
			weak_element;
			weak_element = weak_element->m_next)
		{
			auto entity = weak_element->lock();
			auto t_entity = entity->get_component<transform>();
			auto r_entity = entity->get_component<renderable>();
			auto e_material = r_entity->get_material();
			//material
			if (e_material)
			{
				effect::ptr         mat_effect = e_material->get_effect();
				effect::technique*  mat_technique = mat_effect->get_technique("deferred");
				//applay all pass
				if(mat_technique) for (auto& pass : *mat_technique)
				{
					pass.bind(
						viewport,
						c_camera->get_projection(),
						t_camera->get_matrix_inv(),
						t_entity->get_matrix(),
						e_material->get_parameters()
					);
					//draw
					entity->get_component<renderable>()->draw();
					//end
					pass.unbind();
				}
			}
		}
		//reset state
		render::set_render_state(render_state);
		//unbind
		m_g_buffer.unbind();
		////////////////////////////////////////////////////////////////////////////////
		//clear frame buffer
		render::clear();
		////////////////////////////////////////////////////////////////////////////////
		//ssao pass
		if (m_enable_ambient_occlusion)
		{
			m_ssao.applay(e_camera, m_g_buffer, m_square);
		}
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		m_shader->bind();
		//set gbuffer and ssao
		m_position->set_value(m_g_buffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION));
		m_normal->set_value(m_g_buffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL));
		m_albedo->set_value(m_g_buffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO));
		m_occlusion->set_value(m_ssao.get_texture());
		m_view->set_value(t_camera->get_matrix_inv());
		//add info
		m_ambient_light->set_value(ambient_color);
		//init loop
		int light_count = 0;
		render_queues::element* weak_element = queues.m_cull_light;
		//pass lights
		for (; weak_element && light_count < m_max_lights;
			   weak_element = weak_element->m_next, ++light_count)
		{
			auto l_entity = weak_element->lock();
			//get light
			light_ptr l_light = l_entity->get_component<light>();
			transform_ptr t_light = l_entity->get_component<transform>();
			//uniform light
			m_uniform_lights[light_count].uniform(
				l_light,
				t_camera->get_matrix_inv(),
				t_light->get_matrix()
			);
			//uniform shadow
			if (l_light->is_enable_shadow())
			{
				m_uniform_shadow_lights[light_count].uniform(
					l_light,
					t_light->get_matrix_inv()
				);
			}
		}
		//compute max lights
		m_uniform_n_lights_used->set_value((int)light_count);
		//draw squere
		m_square->draw();
		//unbind texture
		m_g_buffer.disable_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION);
		m_g_buffer.disable_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL);
		m_g_buffer.disable_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO);
		m_ssao.disable_texture();
		//bind
		m_shader->unbind();
		////////////////////////////////////////////////////////////////////////////////
	}
}