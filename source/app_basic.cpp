//
//  app_basic.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 07/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <app_basic.h>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <basic_meshs.h>
#include <regex>
#include <OpenGL4.h>
#include <rendering_pass_deferred.h>
#include <gameobject.h>
#include <transform.h>


void app_basic::key_event(application& app,int key, int scancode, int action, int mods)
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
		p_deferred->set_ambient_occlusion(true);
	}
	else if (key == GLFW_KEY_K)
	{
		rendering_system*	r_system = m_systems.get_system<rendering_system>();
		rendering_pass_ptr	d_pass = r_system->get_rendering_pass()[0];
		auto p_deferred = std::static_pointer_cast<rendering_pass_deferred>(d_pass);
		p_deferred->set_ambient_occlusion(false);
	}
	else if(key == GLFW_KEY_UP)          m_camera->get_component<transform>()->translation({0,0,1});
	else if(key == GLFW_KEY_DOWN)        m_camera->get_component<transform>()->translation({0,0,-1});
	else if(key == GLFW_KEY_LEFT)        m_camera->get_component<transform>()->translation({1,0,0});
	else if(key == GLFW_KEY_RIGHT)       m_camera->get_component<transform>()->translation({-1,0,0});
	else if (key == GLFW_KEY_PAGE_UP)    m_camera->get_component<transform>()->translation({ 0,1,0 });
	else if (key == GLFW_KEY_PAGE_DOWN)  m_camera->get_component<transform>()->translation({ 0,-1,0 });
	else if (key == GLFW_KEY_W)    m_model->get_component<transform>()->translation({ 0,0,1 });
	else if (key == GLFW_KEY_S)    m_model->get_component<transform>()->translation({ 0,0,-1 });
	else if (key == GLFW_KEY_A)    m_model->get_component<transform>()->translation({ 1,0,0 });
	else if (key == GLFW_KEY_D)    m_model->get_component<transform>()->translation({ -1,0,0 });
	else if (key == GLFW_KEY_R)    m_model->get_component<transform>()->translation({ 0,1,0 });
	else if (key == GLFW_KEY_F)    m_model->get_component<transform>()->translation({ 0,-1,0 });
	else if((mods   == GLFW_MOD_SUPER   ||
             mods   == GLFW_MOD_CONTROL)&&
             action == GLFW_PRESS)
    {
		if (key == GLFW_KEY_F)
		{
#if !defined(__APPLE__)
            if (is_fullscreen(app)) go_to_window_mode(app);
            else                    go_to_fullscreen(app);
#endif
		}
    }
}

void app_basic::cursor_position_event(application& app,const glm::dvec2& pos)
{
}

void app_basic::mouse_button_event(application& app,int button, int action, int mods)
{
}

void app_basic::scroll_event(application& application,const glm::dvec2& scroll_offset)
{

}

void app_basic::resize_event(application& application,const glm::ivec2& size)
{
    m_aspect = float(size.x) / float(size.y);
    //viewport
    m_camera->get_component<camera>()->set_viewport(glm::ivec4{0, 0, size.x, size.y});
    //new perspective
    m_camera->get_component<camera>()->set_perspective(m_fov, m_aspect, 0.1, 1000.0);
}

void app_basic::start(application& app)
{
	//get info about window
	m_window_mode_info = window_info
	{
		app.get_window_size(),
		app.get_window_position()
	};
    //alloc all systems
    rendering_system::ptr m_rendering= rendering_system::snew();
    //add into system
    m_systems.add_system(m_rendering);
    //camera
         m_camera = gameobject::camera_new();
    auto c_camera = m_camera->get_component<camera>();
    auto t_camera = m_camera->get_component<transform>();
    glm::vec2 size = app.get_window_size();
    m_aspect       = float(size.x) / float(size.y);
    c_camera->set_viewport(glm::ivec4{0, 0, size.x, size.y});
    c_camera->set_perspective(m_fov, m_aspect, 0.1, 1000.0);
	t_camera->look_at(glm::vec3{ 0.0f, 6.9f, -45.0f },
				  	  glm::vec3{ 0.0f,-1.0f, 0.0f },
					  glm::vec3{ 0.0f, 1.0f, 0.0f });
    //set camera
    m_systems.add_entity(m_camera);
    //deferred alloc
    m_resources.add_directory("assets/shaders/deferred");
    auto rendering_pass = rendering_pass_deferred::snew(m_camera, m_resources);
	rendering_pass->set_ambient_occlusion(true);
    m_rendering->add_rendering_pass(rendering_pass);
    //load assets
    m_resources.add_directory("assets/textures");
	m_resources.add_directory("assets/materials");
	m_resources.add_directory("assets/ship");
	m_resources.add_directory("assets/asteroid");
	m_resources.add_directory("assets/sponza");
    /////// /////// /////// /////// /////// /////// /////// /////// ///////
    // build scene
    {
             m_model = gameobject::node_new(m_resources.get_static_model("ship"));
        auto t_model = m_model->get_component<transform>();
        //set info
        t_model->position({ 0.0f, -5.0f, 0.0f });
        t_model->rotation(glm::quat({glm::radians(15.0), glm::radians(180.0), 0.0}));
        t_model->scale({ 0.2f, 0.2f, 0.2f });
        
        auto e_model_light = gameobject::light_new();
        auto l_model_light = e_model_light->get_component<light>();
        auto t_model_light = e_model_light->get_component<transform>();
        t_model_light->position(glm::vec3{0,6.0f,-80});
        l_model_light->m_diffuse   = { 1.0f, 0.8f, 0.1f, 1.0f };
        l_model_light->m_intensity = 1.5;
        l_model_light->m_min_radius= 0.1;
        l_model_light->m_max_radius= 3.0;
        
        auto e_model_light1 = gameobject::light_new();
        auto l_model_light1 = e_model_light1->get_component<light>();
        auto t_model_light1 = e_model_light1->get_component<transform>();
        t_model_light1->position(glm::vec3{-16.4,9.0f,18.0});
        l_model_light1->spot({ 1.0f, 0.8f, 0.1f, 1.0f },
                             1.0,
                             0.5,
                             5.0,
                             glm::radians(45.0),0.005);
        
        auto e_model_light2 = gameobject::light_new();
        auto l_model_light2 = e_model_light2->get_component<light>();
        auto t_model_light2 = e_model_light2->get_component<transform>();
        t_model_light2->position(glm::vec3{16.4,8.5f,19});
        l_model_light2->spot({ 1.0f, 0.8f, 0.1f, 1.0f },
                             1.0,
                             0.5,
                             5.0,
                             glm::radians(50.0),0.005);
        //append to model
        m_model->add_child(e_model_light1);
        m_model->add_child(e_model_light2);
        m_model->add_child(e_model_light);
        //add to render
        m_systems.add_entity(m_model);
        //cube
#if 1
        auto e_cube = gameobject::cube_new({1,1,1});
             e_cube->add_component(m_resources.get_material("w_box_mat"));
		auto t_cube = e_cube->get_component<transform>();
		t_cube->position({ 0.,-10.0,0. });
		t_cube->scale({ 70.,1.0,70. });
		//add to render
		m_systems.add_entity(e_cube);
#else
		auto e_sponza = gameobject::node_new(m_resources.get_static_model("sponza"));
		auto t_sponza = e_sponza->get_component<transform>();
		t_sponza->position({ 5.,-10.0, -99. });
		t_sponza->scale({ 0.12,0.12,0.12 });
		t_sponza->rotation(glm::quat({ 0, glm::radians(90.0), 0.0 }));
		//add to render
		m_systems.add_entity(e_sponza);
#endif
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
        
        l_lights[0]->m_diffuse   = { 0.0f, 1.0f, 0.0f, 1.0f };
        l_lights[1]->m_diffuse   = { 1.0f, 0.0f, 0.0f, 1.0f };
        l_lights[2]->m_diffuse   = { 0.0f, 0.0f, 1.0f, 1.0f };
        
        for(short i=0;i!=3; ++i)
        {
            l_lights[i]->m_intensity  = 0.62;
            l_lights[i]->m_min_radius = 1.0;
            l_lights[i]->m_max_radius = 70.0;
        }
        
        for(int i = 1; i!=4 ; ++i)
        {
            t_lights[i-1]->position({
                std::sin((glm::pi<float>()*0.66)*i)*15.,
                0.0,
                std::cos((glm::pi<float>()*0.66)*i)*15.,
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
        m_rendering->set_ambient_color(glm::vec4{ 0.26, 0.26, 0.26, 1.0 });
        
    }
}

bool app_basic::run(application& app,double delta_time)
{
	//////////////////////////////////////////////////////////
    //angle
    static float angle = 0;
    //update angle
    angle += 1.0f * delta_time;
    //////////////////////////////////////////////////////////
    //update
    m_model->get_component<transform>()->turn(glm::quat{{0.0, glm::radians(5.0*delta_time), 0.0}});
    //for all lights
    m_lights->get_component<transform>()->turn(glm::quat{{0.0, glm::radians(20.0*delta_time), 0.0}});
    //draw
    m_systems.update(delta_time);
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
    int m_count = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&m_count);
    if (m_count)
    {
        //get info
        m_window_mode_info = window_info
        {
            app.get_window_size(),
            app.get_window_position()
        };
        //go to fullscreen mode
        const GLFWvidmode* monitor_mode = glfwGetVideoMode(monitors[0]);
        glfwSetWindowMonitor(
                             app.get_window()
                             , monitors[0]
                             , 0
                             , 0
                             , monitor_mode->width
                             , monitor_mode->height
                             , 60
                             );
    }
}

void app_basic::go_to_window_mode(application& app)
{    
    //go to window mode
    glfwSetWindowMonitor(
                         app.get_window()
                         , nullptr
                         , m_window_mode_info.m_position.x
                         , m_window_mode_info.m_position.y
                         , m_window_mode_info.m_size.x
                         , m_window_mode_info.m_size.y
                         , 60
                         );
}

bool app_basic::is_fullscreen(application& app) const
{
    return glfwGetWindowMonitor(app.get_window()) != nullptr;
}

