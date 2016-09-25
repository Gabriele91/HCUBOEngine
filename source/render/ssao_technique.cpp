#include <hcube/render/OpenGL4.h>
#include <hcube/render/ssao_technique.h>
#include <hcube/components/transform.h>
#include <cmath>
#include <cstdlib>

namespace hcube
{
	void ssao_technique::init(const ivec2& w_size, resources_manager& resources)
	{
		//load shader ssao
		m_shader = resources.get_shader("ssao_pass");
        m_uniform_noise_scale = m_shader->get_uniform("noise_scale");
        m_uniform_projection = m_shader->get_uniform("projection");
        m_uniform_view = m_shader->get_uniform("view");
		m_uniform_kernel_size = m_shader->get_uniform("kernel_size");
		m_uniform_radius = m_shader->get_uniform("radius");
		m_position = m_shader->get_uniform("g_position");
		m_normal = m_shader->get_uniform("g_normal");
		m_noise = m_shader->get_uniform("t_noise");
		//load shader blur ssao
		m_shader_blur = resources.get_shader("ssao_blur");
		m_uniform_ssoa_input = m_shader_blur->get_uniform("g_ssao_input");
		////////////////////////////////////////////////////////////////////////////////////////////////////
		m_ssao_texture =
		render::create_texture
		(
			{ 
				TF_R8,
				(unsigned int)w_size.x,
				(unsigned int)w_size.y,
				nullptr,
				TT_RGB,
				TTF_FLOAT 
			},
			{
				TMIN_NEAREST,
				TMAG_NEAREST,
				TEDGE_REPEAT,
				TEDGE_REPEAT,
				false 
			}
		);
		//create frame buffer texture
		m_fbo =
			render::create_render_target({
				target_field{ m_ssao_texture, RT_COLOR }
		});
		////////////////////////////////////////////////////////////////////////////////////////////////////
		m_ssao_blur_texture =
		render::create_texture
		(
			{
				TF_R8,
				(unsigned int)w_size.x,
				(unsigned int)w_size.y,
				nullptr,
				TT_RGB,
				TTF_FLOAT 
			},
			{
				TMIN_NEAREST,
				TMAG_NEAREST,
				TEDGE_REPEAT,
				TEDGE_REPEAT,
				false
			}
		);
		m_fbo_blur =
			render::create_render_target({
				target_field{ m_ssao_blur_texture, RT_COLOR }
		});
		////////////////////////////////////////////////////////////////////////////////////////////////////
		//clear
		clear();
		////////////////////////////////////////////////////////////////////////////////////////////////////
		//build kernel
		set_kernel_size(m_max_kernel_size);
		////////////////////////////////////////////////////////////////////////////////////////////////////
		//noise texture
		std::vector<vec3> noise_buffer(16);
		for (size_t i = 0; i != noise_buffer.size(); ++i)
		{
			noise_buffer[i] = vec3
			{
				((float)std::rand() / RAND_MAX) * 2.0 - 1.0,
				((float)std::rand() / RAND_MAX) * 2.0 - 1.0,
				0.0f
			};
		}
		//build noise texture
		m_noise_texture =
		render::create_texture
		(
			{ 
				TF_RGB16F,
				4,
				4,
				(const unsigned char*)noise_buffer.data(),
				TT_RGB,
				TTF_FLOAT
			},
			{
				TMIN_NEAREST,
				TMAG_NEAREST,
				TEDGE_REPEAT,
				TEDGE_REPEAT,
				false 
			}
		);
		////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	void ssao_technique::clear()
	{
		if (m_fbo)
		{
			//clear
			auto temp_color_clear_state = render::get_clear_color_state();
			render::set_clear_color_state(clear_color_state(vec4{ 1.,1.,1.,1. }));

			render::enable_render_target(m_fbo);
			render::clear(CLEAR_COLOR|CLEAR_DEPTH);
			render::disable_render_target(m_fbo);

            render::enable_render_target(m_fbo_blur);
			render::clear(CLEAR_COLOR|CLEAR_DEPTH);
			render::disable_render_target(m_fbo_blur);

			render::set_clear_color_state(temp_color_clear_state);
		}
	}

	void ssao_technique::applay(entity::ptr e_camera, g_buffer& buffer, mesh::ptr square)
	{
		//clear
		auto temp_cullface = render::get_cullface_state();
		auto temp_color_clear_state = render::get_clear_color_state();
		auto temp_depth_buffer_state = render::get_depth_buffer_state();
		//disable depth test
		render::set_cullface_state(CF_BACK);
		render::set_depth_buffer_state({ DM_DISABLE });
		render::set_clear_color_state(clear_color_state(vec4{ 1.,1.,1.,1. }));
        //draw
        camera::ptr   c_camera = e_camera->get_component<camera>();
		//enable fbo
		render::enable_render_target(m_fbo);
#if 0
        //clear buffer not necessary (?)
        render::set_clear_color_state(vec4{1.,1.,1.,1.});
		render::clear(CLEAR_COLOR);
#endif
		//bind shader
		m_shader->bind();
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//bind kernel/proj/scale noise
		if(m_uniform_projection) m_uniform_projection->set_value(c_camera->get_projection());
        if(m_uniform_view)       m_uniform_view->set_value(c_camera->get_view());
        
		m_uniform_noise_scale->set_value((vec2)c_camera->get_viewport_size() / vec2(4, 4));
		m_uniform_kernel_size->set_value((int)m_kernel_size);
		m_uniform_radius->set_value(m_radius);
		//set uniform id
		m_position->set_value(buffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION));
		m_normal->set_value(buffer.get_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL));
		m_noise->set_value(m_noise_texture);
		//////////////////////////////////////////////////////////////////////////////////////////////////////	
		square->draw();
		//////////////////////////////////////////////////////////////////////////////////////////////////////	
		//unbind
		m_shader->unbind();
		//disable fbo
		render::disable_render_target(m_fbo);
		//////////////////////////////////////
		//   ____   __     _    _   ____    //
		//  | |\ \ | |    | |  | | | |\ \   //
		//	| |/ / | |    | |  | | | |/ /   //
		//  | |\ \ | |__  | |__| | | |\ \   //
		//  |_|/_/ |____| |______| |_| \_\  //
		//                                  //
		//////////////////////////////////////	
		//enable fbo
		render::enable_render_target(m_fbo_blur);
		//bind shader
		m_shader_blur->bind();
		//uniform texture
		m_uniform_ssoa_input->set_value(m_ssao_texture);
		//draw
		square->draw();
		//unbind
		m_shader_blur->unbind();
		//disable fbo
		render::disable_render_target(m_fbo_blur);
		//reset state
		render::set_cullface_state(temp_cullface);
		render::set_clear_color_state(temp_color_clear_state);
		render::set_depth_buffer_state(temp_depth_buffer_state);

	}

	void ssao_technique::destoy()
	{
		if (m_shader) m_shader = nullptr;

		if (m_shader) m_shader_blur = nullptr;

		if (m_fbo) render::delete_render_target(m_fbo);

		if (m_fbo_blur)  render::delete_render_target(m_fbo_blur);

		if (m_ssao_texture) render::delete_texture(m_ssao_texture);

		if (m_ssao_blur_texture) render::delete_texture(m_ssao_blur_texture);

		if (m_noise_texture) render::delete_texture(m_noise_texture);

		m_uniform_noise_scale = nullptr;
		m_uniform_projection = nullptr;
		m_uniform_kernel_size = nullptr;
		m_uniform_ssoa_input = nullptr;
		m_position = nullptr;
		m_normal = nullptr;
		m_noise = nullptr;

		m_kernel_size = m_max_kernel_size;
	}

	void ssao_technique::set_texture(int n_tex)
	{
		render::bind_texture(m_ssao_blur_texture, n_tex);
	}

	void ssao_technique::disable_texture()
	{
		render::unbind_texture(m_ssao_blur_texture);
	}

	context_texture* ssao_technique::get_texture() const
	{
		return m_ssao_blur_texture;
	}

	void ssao_technique::set_kernel_size(unsigned int kernel_size)
	{
		m_kernel_size = kernel_size < m_max_kernel_size ? kernel_size : m_max_kernel_size;
	}

	void ssao_technique::set_radius(float radius)
	{
		m_radius = radius;
	}
}