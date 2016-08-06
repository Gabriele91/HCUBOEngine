#include <OpenGL4.h>
#include <ssao_technique.h>
#include <cmath>
#include <cstdlib>

void ssao_technique::init(const glm::ivec2& w_size, resources_manager& resources)
{
	//load shader ssao
	m_shader = resources.get_shader("ssao_pass");
	m_uniform_noise_scale = m_shader->get_shader_uniform_vec2("noise_scale");
	m_uniform_projection = m_shader->get_shader_uniform_mat4("projection");
	m_uniform_kernel_size = m_shader->get_shader_uniform_int("kernel_size");
	m_uniform_radius = m_shader->get_shader_uniform_float("radius");
	m_position = m_shader->get_shader_uniform_int("g_position");
	m_normal = m_shader->get_shader_uniform_int("g_normal");
	m_noise = m_shader->get_shader_uniform_int("t_noise");
	//load shader blur ssao
	m_shader_blur = resources.get_shader("ssao_blur");
	m_uniform_ssoa_input = m_shader->get_shader_uniform_int("g_ssao_input");
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//fbo
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	//gen texture
	glGenTextures(1, &m_ssao_texture);
	glBindTexture(GL_TEXTURE_2D, m_ssao_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w_size.x, w_size.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//create frame buffer texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssao_texture, 0);
	//clear
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//fbo_blur
	glGenFramebuffers(1, &m_fbo_blur);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_blur);
	//gen texture
	glGenTextures(1, &m_ssao_blur_texture);
	glBindTexture(GL_TEXTURE_2D, m_ssao_blur_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w_size.x, w_size.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//create frame buffer texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssao_blur_texture, 0);
	//clear
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//build kernel
	set_kernel_size(m_max_kernel_size);
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//noise texture
	std::vector<glm::vec3> noise_buffer(16);
	for (size_t i = 0; i != noise_buffer.size(); ++i)
	{
		noise_buffer[i] = glm::vec3
		{
			((float)std::rand() / RAND_MAX) * 2.0 - 1.0,
			((float)std::rand() / RAND_MAX) * 2.0 - 1.0,
			0.0f
		};
	}
	//build noise texture
	glGenTextures(1, &m_noise_texture);
	glBindTexture(GL_TEXTURE_2D, m_noise_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, noise_buffer.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////////
}

void ssao_technique::clear()
{
	if (m_fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		//clear
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		//unbind
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//clear blurred texture
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_blur);
		//clear
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		//unbind
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void ssao_technique::applay(entity::ptr e_camera, g_buffer& buffer, mesh::ptr square)
{
	//disabl depth test
	glDisable(GL_DEPTH_TEST);
	//draw
	camera::ptr   c_camera = e_camera->get_component<camera>();
	//enable fbo
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	//clear buffer not necessary (?)
#if 1
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
#endif
	//bind shader
	m_shader->bind();
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//bind kernel/proj/scale noise
	m_uniform_projection->set_value(c_camera->get_projection());
	m_uniform_noise_scale->set_value((glm::vec2)c_camera->get_viewport_size() / glm::vec2(4, 4));
	m_uniform_kernel_size->set_value(m_kernel_size);
	m_uniform_radius->set_value(m_radius);
	//set g_buffer 
	buffer.set_texture_buffer(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION);//0
	buffer.set_texture_buffer(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL);  //1
	//set noise												
	glActiveTexture(GL_TEXTURE0 + 2);//2
	glBindTexture(GL_TEXTURE_2D, m_noise_texture);
	//set uniform id
	m_position->set_value(0);
	m_normal->set_value(1);
	m_noise->set_value(2);
	//////////////////////////////////////////////////////////////////////////////////////////////////////	
	square->draw();
	//////////////////////////////////////////////////////////////////////////////////////////////////////	
	//unbind
	m_shader->unbind();
	//disable g_buffer 
	buffer.disable_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION);//0
	buffer.disable_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL);  //1
    //disable noise
	glActiveTexture(GL_TEXTURE0 + 2);//2
	glBindTexture(GL_TEXTURE_2D, 0);
	//disable fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//////////////////////////////////////
	//   ____   __     _    _   ____    //
	//  | |\ \ | |    | |  | | | |\ \   //
	//	| |/ / | |    | |  | | | |/ /   //
	//  | |\ \ | |__  | |__| | | |\ \   //
	//  |_|/_/ |____| |______| |_| \_\  //
	//                                  //
	//////////////////////////////////////	
	//enable fbo
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_blur);
	//clear buffer not necessary
#if 0
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
#endif
	//bind shader
	m_shader_blur->bind();	
	//bind ssao texture
	glActiveTexture(GL_TEXTURE0);//0
	glBindTexture(GL_TEXTURE_2D, m_ssao_texture);
	//uniform id texture
	m_uniform_ssoa_input->set_value(0);
	//draw
	square->draw();
	//unbind
	m_shader_blur->unbind();
	//disable ssao texture
	glActiveTexture(GL_TEXTURE0);//0
	glBindTexture(GL_TEXTURE_2D, 0);
	//disable fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//renable depth test
	glEnable(GL_DEPTH_TEST);
}

void ssao_technique::destoy()
{
	if (m_shader) m_shader = nullptr;

	if (m_shader) m_shader_blur = nullptr;

	if (m_fbo) glDeleteFramebuffers(1, &m_fbo);

	if (m_fbo_blur) glDeleteFramebuffers(1, &m_fbo_blur);

	if (m_ssao_texture) glDeleteTextures(1, &m_ssao_texture);

	if (m_ssao_blur_texture) glDeleteTextures(1, &m_ssao_blur_texture);
	
	if (m_noise_texture) glDeleteTextures(1, &m_noise_texture);

	m_fbo = 0;
	m_fbo_blur = 0;
	m_last_n_text = 0;
	m_ssao_texture = 0;
	m_noise_texture = 0;
	m_ssao_blur_texture = 0;
	m_uniform_noise_scale = nullptr;
	m_uniform_projection = nullptr;
	m_uniform_kernel_size = nullptr;
	m_uniform_ssoa_input = nullptr;
	m_position = nullptr;
	m_normal = nullptr;
	m_noise = nullptr;

	m_kernel_size = m_max_kernel_size;
}

void ssao_technique::set_texture(GLenum n_tex)
{
	m_last_n_text = n_tex;
	glActiveTexture(GL_TEXTURE0 + n_tex);
	glBindTexture(GL_TEXTURE_2D, m_ssao_blur_texture);
}

void ssao_technique::disable_texture()
{
	glActiveTexture(GL_TEXTURE0 + m_last_n_text);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_last_n_text = 0;
}

void ssao_technique::set_kernel_size(unsigned int kernel_size)
{
	m_kernel_size = kernel_size < m_max_kernel_size ? kernel_size : m_max_kernel_size;
}

void ssao_technique::set_radius(float radius)
{
	m_radius = radius;
}