#include <transform.h>
#include <rendering_pass_forward.h>

namespace hcube
{


	void rendering_pass_forward::draw_pass(
		vec4&  clear_color,
		vec4&  ambient_color,
		entity::ptr e_camera,
		render_queues& queues
	)
	{
		//save state
		auto render_state = render::get_render_state();
		//get camera
		camera::ptr   c_camera = e_camera->get_component<camera>();
		transform_ptr t_camera = e_camera->get_component<transform>();
		const vec4& viewport   = c_camera->get_viewport();
		//set state camera
		render::set_viewport_state({ viewport });
		render::set_clear_color_state({ clear_color });
		render::clear();
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
				effect::technique*  mat_technique = mat_effect->get_technique("forward");
				//applay all pass
				if (mat_technique) for (auto& pass : *mat_technique)
				{
					pass.bind(
						viewport,
						c_camera->get_projection(),
						t_camera->get_matrix_inv(),
						t_entity->get_matrix(),
						e_material->get_parameters()
					);
					//lights?
					if (pass.m_support_light)
					{
						//uniform ambient light
						if (pass.m_uniform_ambient_light)
							pass.m_uniform_ambient_light->set_value(ambient_color);
						//init loop
						int light_count = 0;
						render_queues::element* weak_element = queues.m_cull_light;
						//pass lights
						for (; weak_element && light_count < pass.m_uniform_lights.size();
							weak_element = weak_element->m_next, ++light_count)
						{
							auto l_entity = weak_element->lock();
							//uniform light
							pass.m_uniform_lights[light_count].uniform(
								l_entity->get_component<light>(),
								t_camera->get_matrix_inv(),
								l_entity->get_component<transform>()->get_matrix()
							);
						}
						//pass n lights
						if (pass.m_uniform_n_lights_used)
							pass.m_uniform_n_lights_used->set_value(light_count);
					}
					//draw
					entity->get_component<renderable>()->draw();
					//end
					pass.unbind();
				}
			}
		}
		//reset state
		render::set_render_state(render_state);
	}
}