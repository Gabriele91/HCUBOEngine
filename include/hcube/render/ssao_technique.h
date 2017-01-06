#pragma once
#include <hcube/config.h>
#include <hcube/core/entity.h>
#include <hcube/core/resources_manager.h>
#include <hcube/render/shader.h>
#include <hcube/render/g_buffer.h>
#include <hcube/components/mesh.h>


namespace hcube
{
	//Forward declaration
	class rendering_system;
	//Renderable
	class HCUBE_API ssao_technique
	{
	public:

		const unsigned int m_max_kernel_size = 128;

		virtual ~ssao_technique() { destoy(); }

		void init(const ivec2& w_size, resources_manager& resources);

		void destoy();

		void applay(rendering_system& rsystem,entity::ptr e_camera, g_buffer& buffer, mesh::ptr square);

		void set_texture(int n_text = 0);
		
		context_texture* get_texture() const;

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

		context_uniform* m_uniform_noise_scale;
		context_uniform* m_uniform_projection;
		context_uniform* m_uniform_view;
		context_uniform* m_uniform_kernel_size;
		context_uniform* m_uniform_radius;

		context_uniform* m_position;
		context_uniform* m_normal;
		context_uniform* m_noise;
		//////////////////////////////////////////////////////////////////////
		context_render_target* m_fbo_blur{ nullptr };
		context_texture*       m_ssao_blur_texture{ nullptr };
		shader::ptr			   m_shader_blur;
		context_uniform*       m_uniform_ssoa_input;
		//////////////////////////////////////////////////////////////////////

	};
}