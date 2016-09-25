#include <hcube/render/rendering_pass_forward.h>
#include <hcube/geometries/intersection.h>
#include <hcube/components/transform.h>
#include <hcube/components/renderable.h>

namespace hcube
{

	#define ENABLE_RADIUS_TEST

	void rendering_pass_forward::draw_pass
    (
        int    n_pass,
		vec4&  clear_color,
		vec4&  ambient_color,
		entity::ptr e_camera,
		render_scene& rscene
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
        //is pass 0
        if(!n_pass)
        {
            render::set_clear_color_state({ clear_color });
            render::clear(/* flag */);
        }
		//draw
		HCUBE_FOREACH_QUEUE(weak_element,rscene.get_first(m_queue_type))
		{
			auto entity = weak_element->lock();
			auto t_entity = entity->get_component<transform>();
			auto r_entity = entity->get_component<renderable>();
			auto e_material = r_entity->get_material();
			//material
			if (e_material)
			{
				effect::ptr         mat_effect  = e_material->get_effect();
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
					if (!pass.m_support_light)
					{
						//draw
						r_entity->draw();
					}
					//ambient draw
					else if (pass.m_uniform_ambient_light)
					{
						pass.m_uniform_ambient_light->set_value(ambient_color);
						//draw
						r_entity->draw();
					}
					//draw all spot lights
					else if(pass.m_uniform_spot.is_valid())
					{
						HCUBE_FOREACH_QUEUE(weak_light, rscene.get_first(RQ_SPOT_LIGHT))
						{
							auto e_light = weak_light->lock();
							auto l_light = e_light->get_component<light>();
							auto t_light = e_light->get_component<transform>();
#ifdef ENABLE_RADIUS_TEST 
							#if 0
							if (!r_entity->has_support_culling() || 
								intersection::check(r_entity->get_bounding_box(), l_light->get_sphere()))
							#elif 0
							if (!r_entity->has_support_culling() ||
								 intersection::check(l_light->update_frustum(), r_entity->get_bounding_box())))
                            #else
                            if (!r_entity->has_support_culling() ||
                                 (intersection::check(r_entity->get_bounding_box(),
													  l_light->get_sphere()) 
								  && intersection::check(l_light->update_frustum(),
														 r_entity->get_bounding_box()))
                                )
                            #endif
#endif
							{
								//update shader
								pass.m_uniform_spot.uniform(
									l_light,
									t_light->get_matrix()
								);
								//draw
								r_entity->draw();
							}
						}
                    }
                    //draw all point lights
                    else if(pass.m_uniform_point.is_valid())
                    {
                        HCUBE_FOREACH_QUEUE(weak_light, rscene.get_first(RQ_POINT_LIGHT))
                        {
                            auto e_light = weak_light->lock();
                            auto l_light = e_light->get_component<light>();
#ifdef ENABLE_RADIUS_TEST
							if (!r_entity->has_support_culling() ||
								intersection::check(r_entity->get_bounding_box(),l_light->get_sphere()))
#endif
							{
								//get pos
								auto t_light = e_light->get_component<transform>();
								//uniform
								pass.m_uniform_point.uniform
								(
									l_light,
									t_light->get_matrix()
								);
								//draw
								r_entity->draw();
							}
                        }
                    }
                    //draw all direction lights
                    else if(pass.m_uniform_direction.is_valid())
                    {
                        HCUBE_FOREACH_QUEUE(weak_light,  rscene.get_first(RQ_DIRECTION_LIGHT))
                        {
                            auto e_light = weak_light->lock();
                            auto l_light = e_light->get_component<light>();
                            auto t_light = e_light->get_component<transform>();
                            pass.m_uniform_direction.uniform(
                                                            l_light,
                                                            t_light ->get_matrix()
                                                            );
                            //draw
							r_entity->draw();
                        }
                    }
					//end
					pass.unbind();
				
				}
			}
		}
		//reset state
		render::set_render_state(render_state);
	}
}