#pragma once
#include <shader.h>
#include <mesh.h>
#include <g_buffer.h>
#include <ssao_technique.h>
#include <rendering_system.h>
#include <resources_manager.h>

class rendering_pass_deferred : public rendering_pass,
                                public smart_pointers< rendering_pass_deferred >
{
public:
	//limits
	const unsigned  m_max_lights = 32;

	//pass
	rendering_pass_deferred(entity::ptr camera, 
							resources_manager& resources);

	//overload draw
	virtual void draw_pass(glm::vec4&  clear_color,
						   glm::vec4&  ambient_color,
						   entity::ptr camera,
						   std::vector< entity::wptr >& lights,
						   std::vector< entity::wptr >& renderables);

	void set_ambient_occlusion(bool enable = true);

	bool is_enable_ambient_occlusion() const;

	void stop_update_frustum(bool stop_update);

	void stop_frustum_culling(bool stop_culling);

protected:

	g_buffer          m_g_buffer;
	ssao_technique    m_ssao;

	mesh::ptr         m_square;
	shader::ptr	      m_shader;
	uniform_int::ptr  m_position;
	uniform_int::ptr  m_normal;
	uniform_int::ptr  m_albedo;
	uniform_int::ptr  m_occlusion;
	uniform_vec4::ptr m_ambient_light;

	bool m_enable_ambient_occlusion{ true };
	bool m_update_frustum{ true };
	bool m_stop_frustum_culling{ false };

	struct uniform_light
	{
        uniform_int::ptr   m_uniform_type;
        
        uniform_vec3::ptr  m_uniform_position;
        uniform_vec3::ptr  m_uniform_direction;
        
        uniform_vec3::ptr  m_uniform_diffuse;
        uniform_vec3::ptr  m_uniform_specular;
        
        uniform_float::ptr m_uniform_constant;
        uniform_float::ptr m_uniform_linear;
        uniform_float::ptr m_uniform_quadratic;

		uniform_float::ptr m_uniform_inner_cut_off;
        uniform_float::ptr m_uniform_outer_cut_off;

		void get_uniform(int i, shader::ptr shader);
        void uniform(light_wptr light,const glm::mat4& view,const glm::mat4& model);
        
	};
	std::vector < uniform_light > m_uniform_lights;
	//size lights
	uniform_int::ptr  m_uniform_n_lights_used;

};