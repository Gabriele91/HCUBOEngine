#include <rendering_pass_deferred.h>
#include <basic_meshs.h>
#include <transform.h>


namespace hcube
{
    void rendering_pass_deferred::ambient_light_shader::init(const std::string& path)
    {
        //compile
        m_shader    = shader::snew();
        m_shader->load(path,std::vector<std::string>{ "AMBIENT_LIGHTS" });
        //color
        m_albedo    = m_shader->get_uniform("g_albedo_spec");
        //get all spot lights
        m_ambient_light = m_shader->get_uniform("ambient_light");
        
    }
    
    
    void rendering_pass_deferred::ambient_light_shader::uniform(g_buffer& gbuffer,const vec4& ambient_light)
    {
        m_shader->bind();
        //uniform g_buffer and etc...
        m_albedo->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO));
        //ambient light
        m_ambient_light->set_value(ambient_light);
    }
    
    void rendering_pass_deferred::ambient_light_shader::unbind()
    {
        m_shader->unbind();
    }
    
    void rendering_pass_deferred::spot_light_shader::init(const std::string& path)
    {
        //compile
        m_shader    = shader::snew();
        m_shader->load(path,std::vector<std::string>{ "SPOT_LIGHTS" });
        
        m_position  = m_shader->get_uniform("g_position");
        m_normal    = m_shader->get_uniform("g_normal");
        m_albedo    = m_shader->get_uniform("g_albedo_spec");
        m_occlusion = m_shader->get_uniform("g_occlusion");
        m_view      = m_shader->get_uniform("view");
        //lights
        m_n_spot_lights = m_shader->get_uniform("n_spot_lights");
        //get all spot lights
        for (unsigned i = 0; i != s_max_sport_lights; ++i)
        {
            m_spot_lights[i].get_uniform(i, m_shader);
        }

    }

    
    void rendering_pass_deferred::spot_light_shader::uniform(g_buffer& gbuffer,
                                                              context_texture* ssao,
                                                              const mat4& view,
                                                              const vec4& ambient_light,
                                                              render_queues& queues)
    {
        m_shader->bind();
        //uniform g_buffer and etc...
        m_position->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION));
        m_normal->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL));
        m_albedo->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO));
        m_occlusion->set_value(ssao);
        m_view->set_value(view);
        //counter
        int i_light_count = 0;
        //uniform all lights
        HCUBE_FOREACH_QUEUE(weak_light, queues.m_cull_light_spot)
        {
            if(i_light_count >= s_max_sport_lights) break;
            
            auto e_light = weak_light->lock();
            auto l_light = e_light->get_component<light>();
            auto t_light = e_light->get_component<transform>();
            
            m_spot_lights[i_light_count++].uniform
            (
                l_light,
                //shadow view
                t_light->get_matrix_inv(),
                //camera view
                view,
                //light pos
                t_light->get_matrix()
            );
        }
        //uniform number of lights used
        m_n_spot_lights->set_value(i_light_count);
    }
    
    void rendering_pass_deferred::spot_light_shader::unbind()
    {
        m_shader->unbind();
    }
    
    void rendering_pass_deferred::point_light_shader::init(const std::string& path)
    {
        //compile
        m_shader    = shader::snew();
        m_shader->load(path,std::vector<std::string>{ "POINT_LIGHTS" });
        
        m_position  = m_shader->get_uniform("g_position");
        m_normal    = m_shader->get_uniform("g_normal");
        m_albedo    = m_shader->get_uniform("g_albedo_spec");
        m_occlusion = m_shader->get_uniform("g_occlusion");
        m_view      = m_shader->get_uniform("view");
        //lights
        m_n_point_lights = m_shader->get_uniform("n_point_lights");
        //get all spot lights
        for (unsigned i = 0; i != s_max_point_lights; ++i)
        {
            m_point_lights[i].get_uniform(i, m_shader);
        }
        
    }
    
    
    void rendering_pass_deferred::point_light_shader::uniform(g_buffer& gbuffer,
                                                              context_texture* ssao,
                                                              const mat4& view,
                                                              const vec4& ambient_light,
                                                              render_queues& queues)
    {
        m_shader->bind();
        //uniform g_buffer and etc...
        m_position->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION));
        m_normal->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL));
        m_albedo->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO));
        m_occlusion->set_value(ssao);
        m_view->set_value(view);
        //counter
        int i_light_count = 0;
        //uniform all lights
        HCUBE_FOREACH_QUEUE(weak_light, queues.m_cull_light_point)
        {
            if(i_light_count >= s_max_point_lights) break;
            
            auto e_light = weak_light->lock();
            auto l_light = e_light->get_component<light>();
            auto t_light = e_light->get_component<transform>();
            
            m_point_lights[i_light_count++].uniform
            (
             l_light,
             //camera view
             view,
             //light pos
             t_light->get_matrix()
             );
        }
        //uniform number of lights used
        m_n_point_lights->set_value(i_light_count);
    }
    
    void rendering_pass_deferred::point_light_shader::unbind()
    {
        m_shader->unbind();
    }

	void rendering_pass_deferred::direction_light_shader::init(const std::string& path)
	{
		//compile
		m_shader = shader::snew();
		m_shader->load(path, std::vector<std::string>{ "DIRECTION_LIGHTS" });

		m_position = m_shader->get_uniform("g_position");
		m_normal = m_shader->get_uniform("g_normal");
		m_albedo = m_shader->get_uniform("g_albedo_spec");
		m_occlusion = m_shader->get_uniform("g_occlusion");
		m_view = m_shader->get_uniform("view");
		//lights
		m_n_direction_lights = m_shader->get_uniform("n_direction_lights");
		//get all spot lights
		for (unsigned i = 0; i != s_max_direction_lights; ++i)
		{
			m_direction_lights[i].get_uniform(i, m_shader);
		}

	}
	
	void rendering_pass_deferred::direction_light_shader::uniform(g_buffer& gbuffer,
															      context_texture* ssao,
															      const mat4& view,
															      const vec4& ambient_light,
															      render_queues& queues)
	{
		m_shader->bind();
		//uniform g_buffer and etc...
		m_position->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION));
		m_normal->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL));
		m_albedo->set_value(gbuffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO));
		m_occlusion->set_value(ssao);
		m_view->set_value(view);
		//counter
		int i_light_count = 0;
		//uniform all lights
		HCUBE_FOREACH_QUEUE(weak_light, queues.m_cull_light_direction)
		{
			if (i_light_count >= s_max_direction_lights) break;

			auto e_light = weak_light->lock();
			auto l_light = e_light->get_component<light>();
			auto t_light = e_light->get_component<transform>();

			m_direction_lights[i_light_count++].uniform
			(
				l_light,
				//camera view
				view,
				//light pos
				t_light->get_matrix()
			);
		}
		//uniform number of lights used
		m_n_direction_lights->set_value(i_light_count);
	}

	void rendering_pass_deferred::direction_light_shader::unbind()
	{
		m_shader->unbind();
	}


    
    rendering_pass_deferred::rendering_pass_deferred(const ivec2& w_size, resources_manager& resources)
    {
        m_q_size = w_size;
        m_g_buffer.init(w_size);
        m_ssao.init(w_size, resources);
        m_ssao.set_kernel_size(8);
        m_ssao.set_radius(2.0);
        
        m_square = basic_meshs::square3D({ 2.0,2.0 }, true);
        m_ambient_light.init(resources.get_shader_path("deferred_ambient_light"));
        m_spot_lights.init(resources.get_shader_path("deferred_spot_light"));
		m_point_lights.init(resources.get_shader_path("deferred_point_light"));
		m_direction_lights.init(resources.get_shader_path("deferred_direction_light"));
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
		const vec4&   viewport = c_camera->get_viewport();
		//buffer
		m_g_buffer.bind();
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
				if(mat_technique) for (auto& pass : *mat_technique)
				{
					pass.bind(
						viewport,
						c_camera->get_projection(),
						t_camera->get_matrix_inv(),
						t_entity->get_matrix(),
						e_material->get_parameters()
					);
                    //pass ambient light
                    if(pass.m_uniform_ambient_light)
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
        render::set_blend_state({ BLEND_ONE,BLEND_ONE });
        render::set_depth_buffer_state({false});
        //draw
        ////////////////////////////////////////////////////////////////////////////////
        //AMBIENT LIGHTS
        m_ambient_light.uniform(m_g_buffer, ambient_color);
        m_square->draw();
        m_ambient_light.unbind();
        //SPOT LIGHTS
		if (queues.m_cull_light_spot)
		{
			m_spot_lights.uniform(m_g_buffer, m_ssao.get_texture(), t_camera->get_matrix_inv(), ambient_color, queues);
			m_square->draw();
			m_spot_lights.unbind();
		}
        //POINT LIGHTS
		if (queues.m_cull_light_point)
		{
			m_point_lights.uniform(m_g_buffer, m_ssao.get_texture(), t_camera->get_matrix_inv(), ambient_color, queues);
			m_square->draw();
			m_point_lights.unbind();
		}
		//DIRECTION LIGHTS
		if (queues.m_cull_light_direction)
		{
			m_direction_lights.uniform(m_g_buffer, m_ssao.get_texture(), t_camera->get_matrix_inv(), ambient_color, queues);
			m_square->draw();
			m_direction_lights.unbind();
		}
        ////////////////////////////////////////////////////////////////////////////////
        //reset state
        render::set_render_state(render_state);
	}
}