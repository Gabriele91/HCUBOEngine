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
		//center
		dvec2 mouse_center = (dvec2)app.get_window_size()*0.5;
		//mouse pos
		dvec2 mouse_pos = app.get_mouse_position();
		//direction
		dvec2 mouse_dir = mouse_pos - mouse_center;
		//compute speed
		double mouse_speed = length(mouse_dir / (dvec2)app.get_window_size()) * 100.0f;
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

	void app_basic::start(application& app)
	{
		//set mouse at center
		app.set_mouse_position((dvec2)app.get_window_size()*0.5);
		//commond assets
		m_resources.add_resources_file("common.rs");
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
		//add into system
		m_systems.add_system(m_rendering);
		m_rendering->add_rendering_pass(rendering_pass_deferred::snew(app.get_window_size(), m_resources));
        //translucent
        m_rendering->add_rendering_pass(rendering_pass_forward::snew(RQ_TRANSLUCENT));
		/////// /////// /////// /////// /////// /////// /////// /////// ///////
		// build scene
		{
			//camera
			m_camera = gameobject::camera_new();
			auto c_camera = m_camera->get_component<camera>();
			auto t_camera = m_camera->get_component<transform>();
			vec2 size = app.get_window_size();
			m_aspect = float(size.x) / float(size.y);
			c_camera->set_viewport(ivec4{ 0, 0, size.x, size.y });
			c_camera->set_perspective(m_fov, m_aspect, 0.1, 500.0);
			t_camera->look_at(vec3{ 0.0f, 6.9f, -45.0f },
							  vec3{ 0.0f, 1.0f, 0.0f },
							  vec3{ 0.0f, 1.0f, 0.0f });
			//set camera
			m_systems.add_entity(m_camera);
            
			//cube floor
            {
                auto cube_floor = gameobject::node_new(
                     basic_meshs::cube({ 50,50,50 }, true)
                );
                cube_floor->get_component<renderable>()->set_material(
                    m_resources.get_material("sphere_DefaultMaterial")
                ); 
                cube_floor->get_component<transform>()->translation(
                    vec3{0, -36, 0}
                );
                cube_floor->set_name("cube_floor");
                //
                m_systems.add_entity(cube_floor);
            }
			//sphere
            {
                m_model = gameobject::node_new(
                    basic_meshs::icosphere(5.0f, 2)
                );
                m_model->get_component<renderable>()->set_material(
                    m_resources.get_material("sphere_DefaultMaterial")
                );
                m_model->get_component<transform>()->translation(
                    vec3{0,-5,0}
                );
                m_model->set_name("sphere");
                //
                m_systems.add_entity(m_model);
            }
			//lights
			entity::ptr e_lights[3] =
			{
				gameobject::light_new(),
				gameobject::light_new(),
				gameobject::light_new()
			};
			light_ptr l_lights[3] =
			{
				e_lights[0]->get_component<light>(),
				e_lights[1]->get_component<light>(),
				e_lights[2]->get_component<light>()
			};
			transform_ptr t_lights[3] =
			{
				e_lights[0]->get_component<transform>(),
				e_lights[1]->get_component<transform>(),
				e_lights[2]->get_component<transform>()
			};
			for (short i = 0; i != 3; ++i)
			{
				l_lights[i]->set_radius(
					8.0,
					33.0
				);
				l_lights[i]->set_shadow(shadow_quality[SW_LOW]);
			}

			l_lights[0]->set_color({ 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
			e_lights[0]->set_name("light_green");

			l_lights[1]->set_color({ 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f });
			e_lights[1]->set_name("light_red");

			l_lights[2]->set_color({ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f });
			e_lights[2]->set_name("light_blue");


			for (int i = 1; i != 4; ++i)
			{
				t_lights[i - 1]->position({
                    std::sin((constants::pi<float>()*0.66)*i)*15.,
					5.0,
                    std::cos((constants::pi<float>()*0.66)*i)*15.,
				});
			}

			//node lights
			m_lights = gameobject::node_new();
			m_lights->add_child(e_lights[0]);
			m_lights->add_child(e_lights[1]);
			m_lights->add_child(e_lights[2]);

			//add to render
			m_systems.add_entity(m_lights);

			//ambient color
			m_rendering->set_ambient_color(vec4{ 0.16, 0.16, 0.16, 1.0 });

		}
	}

	bool app_basic::run(application& app, double delta_time)
    {
		//////////////////////////////////////////////////////////
		//update
		m_model->get_component<transform>()->turn(quat{ {0.0, radians(5.0*delta_time), 0.0} });
		//for all lights
		m_lights
			->get_component<transform>()
			->turn(quat{ {0.0, radians(20.0*delta_time), 0.0} });
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
