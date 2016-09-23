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

	class rendering_pass_deferred :
		public rendering_pass,
		public smart_pointers< rendering_pass_deferred >
	{
	public:
		//limits
		const unsigned  m_max_lights = 32;

		//pass
		rendering_pass_deferred(const ivec2& w_size,
                                resources_manager& resources);

		//overload draw
		virtual void draw_pass(int n_pass,
                               vec4&  clear_color,
                               vec4&  ambient_color,
                               entity::ptr camera,
                               render_scene& scene);

		struct ambient_occlusion_param
		{
			bool			m_enable;
			unsigned char   m_kernel_size;
			float			m_range;
		};

		void set_ambient_occlusion(ambient_occlusion_param param);

		bool is_enable_ambient_occlusion() const;

	protected:
        
        class ambient_light_shader
        {
        public:
            shader::ptr        m_shader;
            uniform*           m_position;
            uniform*           m_normal;
            uniform*           m_albedo;
			uniform*           m_occlusion;
            uniform*           m_ambient_light;
            
            void init(resources_manager& resources);
            void uniform(g_buffer& gbuffer,
						 context_texture* ssao,
                         const vec4& ambient_light);
            void unbind();
        };
        
        class spot_light_shader
        {
        public:            
            shader::ptr        m_shader;
			uniform_camera	   m_camera;
            uniform*           m_position;
            uniform*           m_normal;
            uniform*           m_albedo;
            uniform*           m_occlusion;            
            uniform_light_spot m_spot_light;
			//geometry
			uniform_transform  m_transform_cone;
			mesh::ptr		   m_cone;
            
            void init(resources_manager& resources);
            void draw(g_buffer& gbuffer,
                      context_texture* ssao,
					  entity::ptr t_camera,
                      const vec4& ambient_light,
					  render_scene& rsceme,
					  render_queue_type type);
        };
        
        class point_light_shader
        {
        public:
            shader::ptr         m_shader;
			uniform_camera	    m_camera;
            uniform*            m_position;
            uniform*            m_normal;
            uniform*            m_albedo;
            uniform*            m_occlusion;
            uniform_light_point m_point_light;
			//geometry
			mesh::ptr			m_sphere;
            
            void init(resources_manager& resources);
            void draw(g_buffer& gbuffer,
                      context_texture* ssao,
					  entity::ptr t_camera,
                      const vec4& ambient_light,
					  render_scene& rsceme,
					  render_queue_type type);
        };

		class direction_light_shader
		{
		public:

			shader::ptr        m_shader;
			uniform_camera	   m_camera;
			uniform*           m_position;
			uniform*           m_normal;
			uniform*           m_albedo;
			uniform*           m_occlusion;
			uniform_light_direction m_direction_light;

			void init(resources_manager& resources);
			void draw(g_buffer& gbuffer,
					  context_texture* ssao,
					  entity::ptr t_camera,
					  const vec4& ambient_light,
					  render_scene& rsceme,
					  render_queue_type type,
					  mesh::ptr square);
		};
        //size
        ivec2    m_q_size;
        //buffer
        g_buffer m_g_buffer;
        //draw buffer elements
        mesh::ptr              m_square;
        ambient_light_shader   m_ambient_light;
        spot_light_shader      m_spot_lights;
        point_light_shader	   m_point_lights;
		direction_light_shader m_direction_lights;
        //effects
        ssao_technique    m_ssao;
        
		bool m_enable_ambient_occlusion{ true };
		//uniform lights
		std::vector < uniform_light_spot >      m_uniform_lights_spot;
		std::vector < uniform_light_point >     m_uniform_lights_point;
		std::vector < uniform_light_direction > m_uniform_lights_direction;

		//size lights
		uniform* m_uniform_n_lights_used;

	};
}