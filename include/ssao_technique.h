#pragma once
#include <shader.h>
#include <entity.h>
#include <g_buffer.h>
#include <resources_manager.h>

class ssao_technique
{
public:

	const unsigned int m_max_kernel_size = 64;

	virtual ~ssao_technique() { destoy(); }

	void init(entity::ptr camera, resources_manager& resources);

	void destoy();

	void applay(entity::ptr e_camera, g_buffer& buffer, mesh::ptr square);
	
	void set_texture(GLenum n_text = 0);

	void clear();

	void disable_texture();

	void set_kernel_size(unsigned int kernel_size=64);

private:


	//////////////////////////////////////////////////////////////////////
	GLuint		m_fbo          { 0 };
	GLuint      m_ssao_texture { 0 };
	GLuint      m_noise_texture{ 0 };
	GLuint		m_kernel_size  { 0 };


	std::vector< glm::vec3 > m_kernel;

	shader::ptr	m_shader;

	uniform_array_vec3::ptr m_uniform_kernel;
	uniform_vec2::ptr       m_uniform_noise_scale;
	uniform_vec2::ptr       m_uniform_near_far;
	uniform_mat4::ptr       m_uniform_projection;
	uniform_int::ptr        m_uniform_kernel_size;

	uniform_int::ptr  m_position;
	uniform_int::ptr  m_normal;
	uniform_int::ptr  m_noise;
	//////////////////////////////////////////////////////////////////////
	GLuint				m_fbo_blur{ 0 };
	GLuint				m_ssao_blur_texture{ 0 };

	shader::ptr			m_shader_blur;
	uniform_int::ptr    m_uniform_ssoa_input;
	//////////////////////////////////////////////////////////////////////
	GLenum	m_last_n_text = 0;

};
