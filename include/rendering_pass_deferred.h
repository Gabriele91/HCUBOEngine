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
		virtual void draw_pass(vec4&  clear_color,
                               vec4&  ambient_color,
                               entity::ptr camera,
                               render_queues& queues);


		void set_ambient_occlusion(bool enable = true);

		bool is_enable_ambient_occlusion() const;

	protected:
        
        class ambient_light_shader
        {
        public:
            static const int s_max_sport_lights = 12;
            
            shader::ptr        m_shader;
            
            uniform*           m_position;
            uniform*           m_normal;
            uniform*           m_albedo;
            uniform*           m_ambient_light;
            
            void init(const std::string& path);
            void uniform(g_buffer& gbuffer,
                         const vec4& ambient_light);
            void unbind();
        };
        
        class spot_light_shader
        {
        public:
            static const int s_max_sport_lights = 12;
            
            shader::ptr        m_shader;
            
            uniform*           m_position;
            uniform*           m_normal;
            uniform*           m_albedo;
            uniform*           m_occlusion;
            uniform*           m_view;
            uniform*           m_n_spot_lights;
            
            uniform_light_spot m_spot_lights[s_max_sport_lights];
            
            void init(const std::string& path);
            void uniform(g_buffer& gbuffer,
                         context_texture* ssao,
                         const mat4& view,
                         const vec4& ambient_light,
                         render_queues& queues);
            void unbind();
        };
        
        class point_light_shader
        {
        public:
            static const int s_max_point_lights = 12;
            
            shader::ptr        m_shader;
            
            uniform*           m_position;
            uniform*           m_normal;
            uniform*           m_albedo;
            uniform*           m_occlusion;
            uniform*           m_view;
            uniform*           m_n_point_lights;
            
            uniform_light_point m_point_lights[s_max_point_lights];
            
            void init(const std::string& path);
            void uniform(g_buffer& gbuffer,
                         context_texture* ssao,
                         const mat4& view,
                         const vec4& ambient_light,
                         render_queues& queues);
            void unbind();
        };

		class direction_light_shader
		{
		public:
			static const int s_max_direction_lights = 12;

			shader::ptr        m_shader;

			uniform*           m_position;
			uniform*           m_normal;
			uniform*           m_albedo;
			uniform*           m_occlusion;
			uniform*           m_view;
			uniform*           m_n_direction_lights;

			uniform_light_direction m_direction_lights[s_max_direction_lights];

			void init(const std::string& path);
			void uniform(g_buffer& gbuffer,
						 context_texture* ssao,
						 const mat4& view,
						 const vec4& ambient_light,
						 render_queues& queues);
			void unbind();
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