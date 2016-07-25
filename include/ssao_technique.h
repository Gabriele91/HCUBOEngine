#pragma once
#include <shader.h>
#include <entity.h>
#include <g_buffer.h>
#include <resources_manager.h>

class ssao_technique
{
public:

	virtual ~ssao_technique() { destoy(); }

	void init(entity::ptr camera, resources_manager& resources);

	void destoy();

	void bind(entity::ptr e_camera, g_buffer& buffer);

	void unbind(entity::ptr e_camera, g_buffer& buffer);

	void set_texture(GLenum n_text = 0);

	void clear();

	void disable_texture();

private:

	shader::ptr	m_shader;

	GLuint		m_fbo;
	GLuint      m_ssao_texture;
	GLuint      m_noise_texture;

	std::vector< glm::vec3 > m_kernel;

	uniform_array_vec3::ptr m_uniform_kernel;
	uniform_vec2::ptr       m_uniform_noise_scale;
	uniform_vec2::ptr       m_uniform_near_far;
	uniform_mat4::ptr       m_uniform_projection;

	uniform_int::ptr  m_position;
	uniform_int::ptr  m_normal;
	uniform_int::ptr  m_noise;
	GLenum			  m_last_n_text = 0;

};
