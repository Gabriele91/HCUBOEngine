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
	rendering_pass_deferred(const glm::ivec2& w_size,
							resources_manager& resources);

	//overload draw
	virtual void draw_pass(glm::vec4&  clear_color,
						   glm::vec4&  ambient_color,
						   entity::ptr camera,
						   render_queues& queues);

    
    void set_ambient_occlusion(bool enable = true);
    
    bool is_enable_ambient_occlusion() const;
    
protected:

	g_buffer          m_g_buffer;
	ssao_technique    m_ssao;

	glm::ivec2  m_q_size;
	mesh::ptr   m_square;
	shader::ptr	m_shader;
	uniform*    m_position;
	uniform*    m_normal;
	uniform*    m_albedo;
	uniform*    m_occlusion;
	uniform*    m_ambient_light;

    bool m_enable_ambient_occlusion{ true };

	struct uniform_light
	{
		uniform* m_uniform_type;
        
		uniform* m_uniform_position;
		uniform* m_uniform_direction;
        
		uniform* m_uniform_diffuse;
		uniform* m_uniform_specular;
        
		uniform* m_uniform_constant;
		uniform* m_uniform_linear;
		uniform* m_uniform_quadratic;

		uniform* m_uniform_inner_cut_off;
		uniform* m_uniform_outer_cut_off;

		void get_uniform(int i, shader::ptr shader);
        void uniform(light_wptr light,const glm::mat4& view,const glm::mat4& model);
        
	};
	std::vector < uniform_light > m_uniform_lights;
	//size lights
	uniform* m_uniform_n_lights_used;

};