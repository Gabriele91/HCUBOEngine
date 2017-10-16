#include <hcube/render/rendering_pass_ui.h>
#include <hcube/components/renderable.h>

namespace hcube
{
#define ENABLE_RADIUS_TEST

	void rendering_pass_ui::draw_pass
	(
		int    n_pass,
		vec4&  clear_color,
		vec4&  ambient_color,
		entity::ptr e_camera,
		render_scene& rscene,
		rendering_system& rsystem
	)
	{
		//save state
		auto render_state = render::get_render_state();
		//get camera
		camera::ptr   c_camera = e_camera->get_component<camera>();
		transform_ptr t_camera = e_camera->get_component<transform>();
		const vec4&   viewport = c_camera->get_viewport();
		//set state camera
		render::set_viewport_state({ viewport });
		//draw
		HCUBE_FOREACH_QUEUE(weak_element, rscene.get_first(m_queue_type))
		{
			auto entity = weak_element->lock();
			auto t_entity = entity->get_component<transform>();
			auto r_entity = entity->get_component<renderable>();
			auto e_material = r_entity->get_material();
			//material
			if (e_material)
			{
				effect::ptr         mat_effect = e_material->get_effect();
				effect::technique*  mat_forward = mat_effect->get_technique("forward");
				//applay all pass
				if (mat_forward) for (auto& pass : *mat_forward)
				{
					pass.bind(
						c_camera,
						t_camera,
						t_entity,
						e_material->get_parameters()
					);
					//draw
					r_entity->draw(rsystem, e_camera);
					//end
					pass.unbind();

				}
			}
		}
		//reset state
		render::set_render_state(render_state);
	}
}