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
	enum player_type
	{
		PR_WOMAN,
		PR_MEN
	};

	class player
	{
	public:
		//values
		player_type				 m_type{ PR_WOMAN };
		float					 m_life;
		std::string				 m_name;
		int					     m_age;
		std::vector<std::string> m_nicknames;
		vec2				     m_hub{ 0, 1 };
		vec3					 m_pos{ 0, 1, 0 };
		vec4					 m_times{ 0, 1, 0, 1 };
		mat3					 m_rot{ 1. };
		mat4					 m_view{ 2. };
		//wat?
		void set_name(const std::string& s)
		{
			m_name = s;
		}
		std::string& get_name()
		{
			return m_name;
		}
		//return
		HCUBE_DEFINE_PROPERTIES(
			make_property_member(&player::m_life, "life"),
			make_property_member(&player::m_nicknames, "nicknames"),
			make_property_get_set(&player::get_name, &player::set_name, "name"),
			make_property_member(&player::m_age, "age"),
			make_property_member(&player::m_hub, "hub"),
			make_property_member(&player::m_pos, "position"),
			make_property_member(&player::m_rot, "rotation"),
			make_property_member(&player::m_view, "view"),
			make_property_enum_str(
			&player::m_type,
			{
				{ std::string("men")  , PR_MEN   },
				{ std::string("woman"), PR_WOMAN }
			},
			"type")
		)
	};

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
		else if (key == GLFW_KEY_W)    m_camera->get_component<transform>()->move({ 0,0,1 });
		else if (key == GLFW_KEY_S)    m_camera->get_component<transform>()->move({ 0,0,-1 });
		else if (key == GLFW_KEY_A)    m_camera->get_component<transform>()->move({-1,0,0 });
		else if (key == GLFW_KEY_D)    m_camera->get_component<transform>()->move({ 1,0,0 });
		else if (key == GLFW_KEY_R)    m_camera->get_component<transform>()->move({ 0,1,0 });
		else if (key == GLFW_KEY_F)    m_camera->get_component<transform>()->move({ 0,-1,0 });
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
		//ambient color
		m_rendering->set_ambient_color(vec4{ 0.16, 0.16, 0.16, 1.0 });
		//scene
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
		}
		//parser test
		player in_mario;
		parser::properties_parser().parse(
		"name string(\"Mario\")\n "
		"age int(18)           \n"
		"life float(10.5)      \n"
		"nicknames string[]    \n"
		"{                     \n"
		"    \"Dr. Mario\",    \n"
		"    \"Super Mario\"   \n"
		"}"
		"type string(\"men\")", 
		in_mario);
		std::cout << dump::variant_dump(variant("\"\'mario\\\\")) << std::endl;
		std::vector<std::string> vstr{ "mario1", "mario2\"hello\"", "lol" };
		std::cout << dump::variant_dump(variant(vstr)) << std::endl;
		std::cout << dump::properties_dump(in_mario) << std::endl;
	}

	bool app_basic::run(application& app, double delta_time)
    {
		//////////////////////////////////////////////////////////
		//update
		//
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