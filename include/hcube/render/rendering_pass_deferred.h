#pragma once
#include <hcube/config.h>
#include <hcube/math/vector_math.h>
#include <hcube/core/resources_manager.h>
#include <hcube/render/shader.h>
#include <hcube/render/g_buffer.h>
#include <hcube/render/ssao_technique.h>
#include <hcube/render/rendering_system.h>
#include <hcube/components/mesh.h>

namespace hcube
{

	class HCUBE_API rendering_pass_deferred :
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
                               render_scene& rscene,
							   rendering_system& rsystem);

		struct ambient_occlusion_param
		{
			bool			m_enable;
			unsigned char   m_kernel_size;
			float			m_range;
		};

		void set_ambient_occlusion(ambient_occlusion_param param);

		bool is_enable_ambient_occlusion() const;

	protected:
        
        class HCUBE_API ambient_light_shader
        {
        public:
            shader::ptr        m_shader;
            context_uniform*           m_position;
            context_uniform*           m_normal;
            context_uniform*           m_albedo;
			context_uniform*           m_occlusion;
            context_uniform*           m_ambient_light;
            
            void init(resources_manager& resources);
            void uniform(g_buffer& gbuffer,
						 context_texture* ssao,
                         const vec4& ambient_light);
            void unbind();
        };
        
        class HCUBE_API spot_light_shader
        {
        public:            
            shader::ptr        m_shader;
			uniform_camera	   m_camera;
            context_uniform*           m_position;
            context_uniform*           m_normal;
            context_uniform*           m_albedo;
            context_uniform*           m_occlusion;            
            uniform_light_spot m_spot_light;
			//geometry
			uniform_transform  m_transform_cone;
			mesh::ptr		   m_cone;
            
            void init(resources_manager& resources);
            void draw(g_buffer& gbuffer,
                      context_texture* ssao,
					  entity::ptr t_camera,
                      const vec4& ambient_light,
					  render_scene& rscene,
					  render_scene_queue_type type,
					  rendering_system& rsystem);
        };
        
        class HCUBE_API point_light_shader
        {
        public:
            shader::ptr         m_shader;
			uniform_camera	    m_camera;
            context_uniform*            m_position;
            context_uniform*            m_normal;
            context_uniform*            m_albedo;
            context_uniform*            m_occlusion;
            uniform_light_point m_point_light;
			//geometry
			mesh::ptr			m_sphere;
            
            void init(resources_manager& resources);
            void draw(g_buffer& gbuffer,
                      context_texture* ssao,
					  entity::ptr t_camera,
                      const vec4& ambient_light,
					  render_scene& rscene,
					  render_scene_queue_type type,
					  rendering_system& rsystem);
        };

		class HCUBE_API direction_light_shader
		{
		public:

			shader::ptr        m_shader;
			uniform_camera	   m_camera;
			context_uniform*           m_position;
			context_uniform*           m_normal;
			context_uniform*           m_albedo;
			context_uniform*           m_occlusion;
			uniform_light_direction m_direction_light;

			void init(resources_manager& resources);
			void draw(g_buffer& gbuffer,
					  context_texture* ssao,
					  entity::ptr t_camera,
					  const vec4& ambient_light,
					  render_scene& rscene,
					  render_scene_queue_type type,
					  mesh::ptr square,
					  rendering_system& rsystem);
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
		context_uniform* m_uniform_n_lights_used;

	};
}