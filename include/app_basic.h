//
//  app_basic.h
//  OGLHCubeView
//
//  Created by Gabriele on 07/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
//include application instance
#include <application.h>
//include vectorial math
#include <vector_math.h>
//material
#include <material.h>
//camera
#include <camera.h>
//mesh types
#include <mesh.h>
#include <text_mesh.h>
//manager resources
#include <resources_manager.h>
//rendering system
#include <system_manager.h>

namespace hcube
{

	enum class drag_state
	{
		D_OFF,
		D_ON
	};

	enum id_cube_entities
	{
		CUBE_BACK = 0,
		CUBE_PROTONS = 1,
		CUBE_ELECTRONS = 2,
		CUBE_FRONT = 3,
		MAX_ENTITIES
	};

	struct window_info
	{
		ivec2 m_size;
		ivec2 m_position;
	};

	class app_basic : public instance
	{
	public:


		virtual void key_event(application& app, int key, int scancode, int action, int mods);

		virtual void cursor_position_event(application& app, const dvec2& pos);

		virtual void mouse_button_event(application& app, int button, int action, int mods);

		virtual void scroll_event(application& application, const dvec2& scroll_offset);

		virtual void resize_event(application& application, const ivec2& size);

		virtual void start(application& app);

		virtual bool run(application& app, double delta_time);

		virtual bool end(application& app);

	protected:

		//utilities    
		window_info m_window_mode_info;

		void go_to_fullscreen(application& app);

		void go_to_window_mode(application& app);

		bool is_fullscreen(application& app) const;

		void camera_look_around(application& app);

	private:

		//loop 
		bool m_loop{ true };
		//shader map
		resources_manager m_resources;
		//applay
		system_manager m_systems;
		//refs
		entity::ptr  m_camera;
		entity::ptr  m_model;
		entity::ptr  m_lights;
		//move camera info
		dvec2 m_camera_angle;
		//aspect
		float m_fov{ 45.0f };
		float m_aspect{ 0.0f };

	};

}