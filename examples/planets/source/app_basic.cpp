//
//  app_basic.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 07/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <regex>
#include <iostream>
#include <hcube/math/vector_math.h>
#include <hcube/core/filesystem.h>
#include <hcube/geometries/frustum.h>
#include <hcube/render/rendering_pass_deferred.h>
#include <hcube/render/rendering_pass_forward.h>
#include <hcube/render/rendering_pass_shadow.h>
#include <hcube/components/transform.h>
#include <hcube/components/renderable.h>
#include <hcube/utilities/gameobject.h>
#include <hcube/utilities/basic_meshs.h>
#include <tuple>
#include <hcube/data/property.h>
#include <hcube/data/dump/properties_dump.h>
#include <hcube/data/parser/properties_parser.h>
#include <app_basic.h>


namespace hcube
{
	enum 
	{
		SW_LOW,
		SW_MID,
		SW_HIG,
		SW_VERY_HIG,
	};
	const vec2 shadow_quality[] =
	{
		{ 256,256 },
		{ 512,512 },
		{ 1024,1024 },
		{ 2048,2048 },
	};


	void app_basic::key_event(application& app, int key, int scancode, int action, int mods)
	{
		//draw move
		float move_vel   = m_planet_draw_state == PDRAW_ON_GROUND ? 0.125 : 5.0f;
        
		if (key == GLFW_KEY_ESCAPE)
		{
			m_loop = false;
			return;
        }
		else if (key == GLFW_KEY_O)
		{
			rendering_system*	r_system = m_systems.get_system<rendering_system>();
			rendering_pass_ptr	d_pass = r_system->get_rendering_pass()[0];
			auto p_deferred = std::static_pointer_cast<rendering_pass_deferred>(d_pass);
			p_deferred->set_ambient_occlusion({ true, 16, 0.3f });
		}
		else if (key == GLFW_KEY_K)
		{
			rendering_system*	r_system = m_systems.get_system<rendering_system>();
			rendering_pass_ptr	d_pass = r_system->get_rendering_pass()[0];
			auto p_deferred = std::static_pointer_cast<rendering_pass_deferred>(d_pass);
			p_deferred->set_ambient_occlusion({false,0,0});
		}
		else if (key == GLFW_KEY_U)
		{
			rendering_system*	r_system = m_systems.get_system<rendering_system>();
			r_system->stop_update_frustum(false);
		}
		else if (key == GLFW_KEY_H)
		{
			rendering_system*	r_system = m_systems.get_system<rendering_system>();
			r_system->stop_update_frustum(true);
		}
		else if (key == GLFW_KEY_I)
		{
			rendering_system*	r_system = m_systems.get_system<rendering_system>();
			r_system->stop_frustum_culling(false);
		}
		else if (key == GLFW_KEY_J)
		{
			rendering_system*	r_system = m_systems.get_system<rendering_system>();
			r_system->stop_frustum_culling(true);
		}
		else if (key == GLFW_KEY_W)    m_camera->get_component<transform>()->move({ 0,0,move_vel });
		else if (key == GLFW_KEY_S)    m_camera->get_component<transform>()->move({ 0,0,-move_vel });
		else if (key == GLFW_KEY_A)    m_camera->get_component<transform>()->move({-move_vel,0,0 });
		else if (key == GLFW_KEY_D)    m_camera->get_component<transform>()->move({ move_vel,0,0 });
		else if (key == GLFW_KEY_R)    m_camera->get_component<transform>()->move({ 0,move_vel,0 });
		else if (key == GLFW_KEY_F)    m_camera->get_component<transform>()->move({ 0,-move_vel,0 });
		else if ((mods == GLFW_MOD_SUPER ||
				  mods == GLFW_MOD_CONTROL) &&
				  action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_M)
			{
#if !defined(__APPLE__)
				if (app.is_fullscreen()) go_to_window_mode(app);
				else                     go_to_fullscreen(app);
#endif
			}
		}
	}

	void app_basic::cursor_position_event(application& app, const dvec2& in_mouse_pos)
	{
		camera_look_around(app);
	}

	void app_basic::mouse_button_event(application& app, int button, int action, int mods)
	{
	}

	void app_basic::scroll_event(application& application, const dvec2& scroll_offset)
	{

	}

	void app_basic::resize_event(application& application, const ivec2& size)
	{
		m_aspect = float(size.x) / float(size.y);
		//viewport
		m_camera->get_component<camera>()->set_viewport(ivec4{ 0, 0, size.x, size.y });
		//new perspective
		m_camera->get_component<camera>()->set_perspective(m_fov, m_aspect, 0.1, 500.0);
	}

	void app_basic::camera_look_around(application& app)
    {
		const float speed = 1500.0f;
		//center
		dvec2 mouse_center = (dvec2)app.get_window_size()*0.5;
		//mouse pos
		dvec2 mouse_pos = app.get_mouse_position();
		//direction
		dvec2 mouse_dir = mouse_pos - mouse_center;
		//compute speed
		double mouse_speed = length(mouse_dir / (dvec2)app.get_window_size()) * speed;
		//do not
		if (mouse_speed <= 0.49) return;
		//norm direction
		dvec2 norm_mouse_dir = normalize(mouse_dir);
		//angle
		m_camera_angle += norm_mouse_dir * mouse_speed * app.get_last_delta_time();
		//rote
		m_camera->get_component<transform>()->rotation(quat{ vec3{ m_camera_angle.y, m_camera_angle.x,0.0} });
		//reset pos
		app.set_mouse_position(mouse_center);
	}

	void app_basic::set_planet_material(planet_draw_state dstate)
	{
		if (m_planet_draw_state != dstate)
		{
			switch (dstate)
			{
			case PDRAW_IN_SPACE:
				m_planet->get_component<renderable>()->set_material(m_material[MAT_GROUND_FROM_S]);
				m_sky->get_component<renderable>()->set_material(m_material[MAT_SKY_FROM_S]);
				break;
			case PDRAW_ON_GROUND:
				m_planet->get_component<renderable>()->set_material(m_material[MAT_GROUND_FROM_P]);
				m_sky->get_component<renderable>()->set_material(m_material[MAT_SKY_FROM_P]);
			break;
			default: break;
			}
			m_planet_draw_state = dstate;
		}
	}


#define SET_PARAM_IF_EXIST(param,n) { auto p = param; if(p) p->set_value(n);  }
	const float scale_planet = 100.0;
	const float scale_depth  = 0.25;
	const float size_planet  = (1.0) * scale_planet;
	const float size_sky     = (1.025) * scale_planet;

	void app_basic::start(application& app)
	{
		//set mouse at center
		app.set_mouse_position((dvec2)app.get_window_size()*0.5);
		//commond assets
		m_resources.add_resources_file("common.rs");
		m_resources.add_resources_file("planet_assets/assets.rs");
		//get info about window
		m_window_mode_info = window_info
		{
			app.get_window_size(),
			app.get_window_position()
		};
		//alloc all systems
		rendering_system::ptr m_rendering = rendering_system::snew();
		//add shadow support
		m_rendering->add_rendering_pass(rendering_pass_shadow::snew(m_resources));
		//add default render
		m_rendering->add_rendering_pass(rendering_pass_deferred::snew(app.get_window_size(), m_resources));
        //translucent
        m_rendering->add_rendering_pass(rendering_pass_forward::snew(RQ_TRANSLUCENT));
		//add into system
		m_systems.add_system(m_rendering);
		//ambient color
		m_rendering->set_clear_color(vec4{ 0.0, 0.0, 0.0, 0.0 });
		m_rendering->set_ambient_color(vec4{ 1.0, 1.0, 1.0, 1.0 });
		//scene
		{
			//camera
			m_camera = gameobject::camera_new();
			auto c_camera = m_camera->get_component<camera>();
			auto t_camera = m_camera->get_component<transform>();
			vec2 size = app.get_window_size();
			m_aspect = float(size.x) / float(size.y);
			c_camera->set_viewport(ivec4{ 0, 0, size.x, size.y });
			c_camera->set_perspective(m_fov, m_aspect, 0.01, 1000.0);
			t_camera->look_at(
				vec3{ 0.0f, 0.0f, -scale_planet*2.5f },
				vec3{ 0.0f, 0.0f, 0.0f },
				vec3{ 0.0f, 1.0f, 0.0f }
			);
			//set camera
			m_systems.add_entity(m_camera);
			//materials
			m_material[MAT_GROUND_FROM_S] = m_resources.get_material("earth_ground_from_space");
			m_material[MAT_SKY_FROM_S]    = m_resources.get_material("earth_sky_from_space");
			m_material[MAT_GROUND_FROM_P] = m_resources.get_material("earth_ground_from_ground");
			m_material[MAT_SKY_FROM_P]    = m_resources.get_material("earth_sky_from_ground");
			//set params
			for (material_ptr& p_mat : m_material)
			{
				SET_PARAM_IF_EXIST(p_mat->get_parameter_by_name("atmosphere_radius"), size_sky);
				SET_PARAM_IF_EXIST(p_mat->get_parameter_by_name("planetary_radius"), size_planet);
				SET_PARAM_IF_EXIST(p_mat->get_parameter_by_name("fScaleDepth"), scale_depth);
				SET_PARAM_IF_EXIST(p_mat->get_parameter_by_name("sun_pos"), vec3(30.0, 0, 0.0));
			}
			//planet
			m_sky = gameobject::node_new(basic_meshs::icosphere(size_sky, 5, true));
			m_sky->set_name("planet_sky");
			//planet ground
			m_planet = gameobject::node_new(basic_meshs::icosphere(size_planet, 5, true));
			m_planet->set_name("planet_ground");
			//set material
			set_planet_material(PDRAW_IN_SPACE);

			//test terrain
            m_systems.add_entity(m_planet);
			m_systems.add_entity(m_sky);
		}
		
	}

	bool app_basic::run(application& app, double delta_time)
    {
		//////////////////////////////////////////////////////////
		//update
		float camera_dist_from_planet = distance(
			m_camera->get_component<transform>()->get_global_position(),
			m_planet->get_component<transform>()->get_global_position()
		);
		//set materials
		if (camera_dist_from_planet < size_sky)
			set_planet_material(PDRAW_ON_GROUND);
		else
			set_planet_material(PDRAW_IN_SPACE);

		//
#if 1
		SET_PARAM_IF_EXIST(
			m_planet
			->get_component<renderable>()
			->get_material()
			->get_parameter_by_name("camera_height"), camera_dist_from_planet
		);
		SET_PARAM_IF_EXIST(
			m_sky
			->get_component<renderable>()
			->get_material()
			->get_parameter_by_name("camera_height"), camera_dist_from_planet
		);
#endif
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//draw
		m_systems.update(delta_time);
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//do loop
		return m_loop;
	}

	bool app_basic::end(application& app)
	{
		return true;
	}

	void app_basic::go_to_fullscreen(application& app)
	{
		//get info
		m_window_mode_info = window_info
		{
			app.get_window_size(),
			app.get_window_position()
		};
		//go to fullscreen mode
		app.set_fullscreen_size(app.get_screen_size());
	}

	void app_basic::go_to_window_mode(application& app)
	{
		app.set_window_size(m_window_mode_info.m_position, m_window_mode_info.m_size);
	}
	
}
