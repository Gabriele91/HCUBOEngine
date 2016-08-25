#pragma once
#include <shader.h>
#include <mesh.h>
#include <g_buffer.h>
#include <ssao_technique.h>
#include <rendering_system.h>
#include <resources_manager.h>
#include <vector_math.h>

namespace hcube
{

	class rendering_pass_deferred : public rendering_pass,
		public smart_pointers< rendering_pass_deferred >
	{
	public:
		//limits
		const unsigned  m_max_lights = 32;

		//pass
		rendering_pass_deferred(const ivec2& w_size,
                                resources_manager& resources);

		//overload draw
		virtual void draw_pass(vec4&  clear_color,
                               vec4&  ambient_color,
                               entity::ptr camera,
                               render_queues& queues);


		void set_ambient_occlusion(bool enable = true);

		bool is_enable_ambient_occlusion() const;

	protected:

		g_buffer          m_g_buffer;
		ssao_technique    m_ssao;

		ivec2  m_q_size;
		mesh::ptr   m_square;
		shader::ptr	m_shader;
		uniform*    m_position;
		uniform*    m_normal;
		uniform*    m_albedo;
		uniform*    m_occlusion;
		uniform*    m_ambient_light;

		bool m_enable_ambient_occlusion{ true };

		//uniform lights
		std::vector < uniform_light > m_uniform_lights;

		//size lights
		uniform* m_uniform_n_lights_used;

	};
}