#include <rendering_pass_deferred.h>
#include <basic_meshs.h>
#include <transform.h>


namespace hcube
{
	#pragma region "ambient light"

    void rendering_pass_deferred::ambient_light_shader::init(resources_manager& rsmanager)
    {
        //compile
        m_shader    = shader::snew();
        m_shader->load(rsmanager.get_shader_path("deferred_ambient_light"),std::vector<std::string>{ "AMBIENT_LIGHTS" });
        //color
        m_albedo    = m_shader->get_uniform("g_albedo_spec");
		m_occlusion = m_shader->get_uniform("g_occlusion");
        //get all spot lights
        m_ambient_light = m_shader->get_uniform("ambient_light");
        
    }
   
    void rendering_pass_deferred::ambient_light_shader::uniform(g_buffer& gbuffer,
																context_texture* ssao, 
																const vec4& ambient_light)
    {
        m_shader->bind();
        //uniform g_buffer and etc...
        m_albedo->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO));
		m_occlusion->set_value(ssao);
        //ambient light
        m_ambient_light->set_value(ambient_light);
    }
    
    void rendering_pass_deferred::ambient_light_shader::unbind()
    {
        m_shader->unbind();
    }

	#pragma endregion

	#pragma region "spot light"
    void rendering_pass_deferred::spot_light_shader::init(resources_manager& rsmanager)
    {
        //compile
        m_shader    = shader::snew();
        m_shader->load(rsmanager.get_shader_path("deferred_spot_light"),std::vector<std::string>{ "SPOT_LIGHTS" });
        
        m_position		  = m_shader->get_uniform("g_position");
        m_normal		  = m_shader->get_uniform("g_normal");
        m_albedo		  = m_shader->get_uniform("g_albedo_spec");
        m_occlusion		  = m_shader->get_uniform("g_occlusion");
		m_camera.get_uniform(m_shader);
		m_transform_cone.get_uniform(m_shader);
		//get mesh
		auto root_cone = rsmanager.get_prefab("cone")->instantiate();
		//get sphere
		auto e_cone = root_cone->get_childs().begin()->second;
		//get component
		m_cone = std::static_pointer_cast<mesh>(e_cone->get_component<renderable>()->copy());
        //get spot light
		m_spot_light.get_uniform(m_shader);
    }


    void rendering_pass_deferred::spot_light_shader::draw(g_buffer& gbuffer,
                                                          context_texture* ssao,
													      entity::ptr e_camera,
                                                          const vec4& ambient_light,
                                                          render_queues& queues,
													      mesh::ptr square)
    {
        m_shader->bind();
        //uniform g_buffer and etc...
        m_position->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION));
        m_normal->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL));
        m_albedo->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO));
        m_occlusion->set_value(ssao);
		//uniform
		m_camera.uniform(e_camera->get_component<camera>(),
						 e_camera->get_component<transform>()->get_matrix());
		//depth mode reverse
		render::set_depth_buffer_state({ DT_GREATER_EQUAL, DM_ENABLE_ONLY_READ });
		render::set_cullface_state({ CF_FRONT });
        //uniform all lights
        HCUBE_FOREACH_QUEUE(weak_light, queues.m_cull_light_spot)
        {            
            auto e_light = weak_light->lock();
            auto l_light = e_light->get_component<light>();
            auto t_light = e_light->get_component<transform>();
            
            m_spot_light.uniform
            (
                l_light,
                //light pos
                t_light->get_matrix()
            );
			//transform cone
			m_transform_cone.uniform(t_light);
			//draw cone
			m_cone->draw();
        }
		m_shader->unbind();
    }
	#pragma endregion

	#pragma region "point light"
    void rendering_pass_deferred::point_light_shader::init(resources_manager& rsmanager)
    {
        //compile
        m_shader    = shader::snew();
        m_shader->load(rsmanager.get_shader_path("deferred_point_light"),std::vector<std::string>{ "POINT_LIGHTS" });
        
        m_position  = m_shader->get_uniform("g_position");
        m_normal    = m_shader->get_uniform("g_normal");
        m_albedo    = m_shader->get_uniform("g_albedo_spec");
        m_occlusion = m_shader->get_uniform("g_occlusion");
		m_camera.get_uniform(m_shader);
		//get mesh
		auto root_sphere = rsmanager.get_prefab("sphere")->instantiate();
		//get sphere
		auto e_sphere = root_sphere->get_childs().begin()->second;
		//get component
		m_sphere = std::static_pointer_cast<mesh>( e_sphere->get_component<renderable>()->copy() );
        //get light
        m_point_light.get_uniform(m_shader);
    }
    
    
	void rendering_pass_deferred::point_light_shader::draw(g_buffer& gbuffer,
		context_texture* ssao,
		entity::ptr e_camera,
		const vec4& ambient_light,
		render_queues& queues,
		mesh::ptr square)
	{
		m_shader->bind();
		//uniform g_buffer and etc...
		m_position->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION));
		m_normal->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL));
		m_albedo->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO));
		m_occlusion->set_value(ssao);
		//ref
		auto c_camera = e_camera->get_component<camera>();
		auto t_camera = e_camera->get_component<transform>();
		//uniform
		m_camera.uniform(c_camera, t_camera->get_matrix());
		//depth mode reverse
		render::set_depth_buffer_state({  DT_GREATER_EQUAL, DM_ENABLE_ONLY_READ });
		render::set_cullface_state({ CF_FRONT });
        //uniform all lights
        HCUBE_FOREACH_QUEUE(weak_light, queues.m_cull_light_point)
        {
            
            auto e_light = weak_light->lock();
            auto l_light = e_light->get_component<light>();
            auto t_light = e_light->get_component<transform>();
            
            m_point_light.uniform
            (
				 l_light,
				 //light pos
				 t_light->get_matrix()
            );
			//cullface
			m_sphere->draw();
			//break;
        }
		m_shader->unbind();
    }
	#pragma endregion

	#pragma region "direction light"
	void rendering_pass_deferred::direction_light_shader::init(resources_manager& rsmanager)
	{
		//compile
		m_shader = shader::snew();
		m_shader->load(rsmanager.get_shader_path("deferred_direction_light"), std::vector<std::string>{ "DIRECTION_LIGHTS" });

		m_position = m_shader->get_uniform("g_position");
		m_normal = m_shader->get_uniform("g_normal");
		m_albedo = m_shader->get_uniform("g_albedo_spec");
		m_occlusion = m_shader->get_uniform("g_occlusion");
		m_camera.get_uniform(m_shader);
		//get geometry

		//get light
		m_direction_light.get_uniform(m_shader);

	}
	
	void rendering_pass_deferred::direction_light_shader::draw(g_buffer& gbuffer,
															   context_texture* ssao,
															   entity::ptr e_camera,
															   const vec4& ambient_light,
															   render_queues& queues,
															   mesh::ptr square)
	{
		m_shader->bind();
		//uniform g_buffer and etc...
		m_position->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION));
		m_normal->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL));
		m_albedo->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO));
		m_occlusion->set_value(ssao);
		//uniform
		m_camera.uniform(e_camera->get_component<camera>(),
						 e_camera->get_component<transform>()->get_matrix());
		//uniform all lights
		HCUBE_FOREACH_QUEUE(weak_light, queues.m_cull_light_direction)
		{
			auto e_light = weak_light->lock();
			auto l_light = e_light->get_component<light>();
			auto t_light = e_light->get_component<transform>();

			m_direction_light.uniform
			(
				l_light,
				//light pos
				t_light->get_matrix()
			);
			square->draw();
		}
		m_shader->unbind();
	}

	#pragma endregion

    rendering_pass_deferred::rendering_pass_deferred
	(
		const ivec2& w_size, 
		resources_manager& resources
	)
    {
        m_q_size = w_size;
        m_g_buffer.init(w_size);
        m_ssao.init(w_size, resources);
		m_ssao.clear();
		m_enable_ambient_occlusion = false;
        
        m_square = basic_meshs::square3D({ 2.0,2.0 }, true);
        m_ambient_light.init(resources);
        m_spot_lights.init(resources);
		m_point_lights.init(resources);
		m_direction_lights.init(resources);
    }
    
	void rendering_pass_deferred::set_ambient_occlusion(ambient_occlusion_param param)
	{
		if (m_enable_ambient_occlusion && !param.m_enable)
		{
			m_ssao.clear();
		}
		m_enable_ambient_occlusion = param.m_enable;
		m_ssao.set_kernel_size(param.m_kernel_size < m_ssao.m_max_kernel_size ? param.m_kernel_size : m_ssao.m_max_kernel_size);
		m_ssao.set_radius(param.m_range);
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
		const vec4&   viewport = c_camera->get_viewport();
		//buffer
		render::enable_render_target(m_g_buffer.get_geometry_render_target());
		//set state
		render::set_viewport_state({ viewport });
		render::set_clear_color_state(vec4{ clear_color.r, clear_color.g, clear_color.b,1.0 });
		render::clear();
		//save state
		auto render_state = render::get_render_state();
		//draw
		HCUBE_FOREACH_QUEUE(weak_element, queues.m_cull_opaque)
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
				if (mat_technique) for (auto& pass : *mat_technique)
				{
					pass.bind(
						c_camera,
						t_camera,
						t_entity,
						e_material->get_parameters()
					);
					//pass ambient light
					if (pass.m_uniform_ambient_light)
					{
						pass.m_uniform_ambient_light->set_value(ambient_color);
					}
					//draw
					entity->get_component<renderable>()->draw();
					//end
					pass.unbind();
				}
			}
		}
		//unbind
		render::disable_render_target(m_g_buffer.get_geometry_render_target());
		////////////////////////////////////////////////////////////////////////////////
		//ssao pass
		if (m_enable_ambient_occlusion)
		{
			m_ssao.applay(e_camera, m_g_buffer, m_square);
		}
		////////////////////////////////////////////////////////////////////////////////
		render::enable_render_target(m_g_buffer.get_lights_render_target());
		///////////////////////////////////////////////////////////////////////////////
		//clear color buffer
		render::clear(false);
		///////////////////////////////////////////////////////////////////////////////
		render::set_blend_state({ BLEND_ONE,BLEND_ONE });
		render::set_depth_buffer_state({ DM_DISABLE });
        //draw
        ////////////////////////////////////////////////////////////////////////////////
        //AMBIENT LIGHTS
        m_ambient_light.uniform
		(
			m_g_buffer,
			m_ssao.get_texture(), 
			ambient_color
		);
        m_square->draw();
        m_ambient_light.unbind();
        //SPOT LIGHTS
		if (queues.m_cull_light_spot)
		{
			m_spot_lights.draw
			(
				m_g_buffer, 
				m_ssao.get_texture(), 
				e_camera, 
				ambient_color, 
				queues,
				m_square
			);
		}
        //POINT LIGHTS
		if (queues.m_cull_light_point)
		{
			m_point_lights.draw
			(
				m_g_buffer, 
				m_ssao.get_texture(), 
				e_camera, 
				ambient_color, 
				queues, 
				m_square
			);
		}
		//DIRECTION LIGHTS
		if (queues.m_cull_light_direction)
		{
			m_direction_lights.draw
			(
				m_g_buffer, 
				m_ssao.get_texture(), 
				e_camera, 
				ambient_color, 
				queues,
				m_square
			);
		}
		render::disable_render_target(m_g_buffer.get_lights_render_target());
		////////////////////////////////////////////////////////////////////////////////
		//like shader, copy color buffer
		vec4 g_area{ 0,0, (vec2)m_g_buffer.get_size() };
		render::copy_target_to_target
		(
			g_area,
			m_g_buffer.get_lights_render_target(),
			g_area,
			0,
			RT_COLOR
		);
        ////////////////////////////////////////////////////////////////////////////////
        //reset state
        render::set_render_state(render_state);
	}
}