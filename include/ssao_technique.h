#pragma once
#include <shader.h>
#include <entity.h>
#include <mesh.h>
#include <g_buffer.h>
#include <resources_manager.h>

namespace hcube
{
	class ssao_technique
	{
	public:

		const unsigned int m_max_kernel_size = 128;

		virtual ~ssao_technique() { destoy(); }

		void init(const ivec2& w_size, resources_manager& resources);

		void destoy();

		void applay(entity::ptr e_camera, g_buffer& buffer, mesh::ptr square);

		void set_texture(int n_text = 0);

		void clear();

		void disable_texture();

		void set_kernel_size(unsigned int kernel_size = 64);

		void set_radius(float radius = 1.0);

	private:


		//////////////////////////////////////////////////////////////////////
		context_render_target* m_fbo{ nullptr };
		context_texture*       m_ssao_texture{ nullptr };
		context_texture*       m_noise_texture{ nullptr };
		unsigned int           m_kernel_size{ 0 };
		float                  m_radius{ 1.0f };

		shader::ptr	m_shader;

		uniform* m_uniform_noise_scale;
		uniform* m_uniform_projection;
		uniform* m_uniform_kernel_size;
		uniform* m_uniform_radius;

		uniform* m_position;
		uniform* m_normal;
		uniform* m_noise;
		//////////////////////////////////////////////////////////////////////
		context_render_target* m_fbo_blur{ nullptr };
		context_texture*       m_ssao_blur_texture{ nullptr };
		shader::ptr			   m_shader_blur;
		uniform*               m_uniform_ssoa_input;
		//////////////////////////////////////////////////////////////////////

	};
}