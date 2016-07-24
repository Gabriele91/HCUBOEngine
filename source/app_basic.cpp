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
    
    if(key == GLFW_KEY_UP)   m_camera->get_component<transform>()->translation({0,0,1});
    if(key == GLFW_KEY_DOWN) m_camera->get_component<transform>()->translation({0,0,-1});
    if(key == GLFW_KEY_LEFT) m_camera->get_component<transform>()->translation({1,0,0});
    if(key == GLFW_KEY_RIGHT)m_camera->get_component<transform>()->translation({-1,0,0});
    if(key == GLFW_KEY_W)    m_model->get_component<transform>()->translation({0,0,1});
    if(key == GLFW_KEY_S)    m_model->get_component<transform>()->translation({0,0,-1});
    if(key == GLFW_KEY_A)    m_model->get_component<transform>()->translation({1,0,0});
    if(key == GLFW_KEY_D)    m_model->get_component<transform>()->translation({-1,0,0});

    if((mods   == GLFW_MOD_SUPER   ||
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
    m_camera->get_component<camera>()->set_perspective(m_fov, m_aspect, 0.01, 100.0);
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
    c_camera->set_perspective(m_fov, m_aspect, 0.01, 100.0);
	t_camera->look_at(glm::vec3{ 0.0f, 6.9f, -45.0f },
				  	  glm::vec3{ 0.0f,-1.0f, 0.0f },
					  glm::vec3{ 0.0f, 1.0f, 0.0f });
    //set camera
    m_systems.add_entity(m_camera);
    //deferred alloc
    m_resources.add_directory("assets/shaders/deferred");
    auto rendering_pass = rendering_pass_deferred::snew(m_camera, m_resources);
    m_rendering->add_rendering_pass(rendering_pass);
    //load assets
    m_resources.add_directory("assets/textures");
	m_resources.add_directory("assets/materials");
	m_resources.add_directory("assets/ship");
	m_resources.add_directory("assets/asteroid"); 
    /////// /////// /////// /////// /////// /////// /////// /////// ///////
    // build scene
    {
             m_model = gameobject::node_new(m_resources.get_static_model("ship"));
        auto t_model = m_model->get_component<transform>();
        //set info
        t_model->position({ 0.0f, -5.0f, 0.0f });
        t_model->rotation(glm::quat({glm::radians(15.0), glm::radians(180.0), 0.0}));
        t_model->scale({ 0.2f, 0.2f, 0.2f });
        //add to render
        m_systems.add_entity(m_model);
        
        //lights
        light_ptr light0 = light_snew();
        light_ptr light1 = light_snew();
        light_ptr light2 = light_snew();
        
        light0->m_diffuse   = { 0.0f, 1.0f, 0.0f, 1.0f };
        light0->m_linear    = 0.01;
        light0->m_const     = 0.45;
        light0->m_quadratic = 0.01;
        
        light1->m_diffuse   = { 1.0f, 0.0f, 0.0f, 1.0f };
        light1->m_linear	= 0.01;
        light1->m_const     = 0.45;
        light1->m_quadratic = 0.01;
        
        light2->m_diffuse   = { 0.0f, 0.0f, 1.0f, 1.0f };
        light2->m_linear    = 0.01;
        light2->m_const     = 0.45;
        light2->m_quadratic = 0.01;
        
        //add to render
        m_systems.add_entity(gameobject::node_new(light0));
        m_systems.add_entity(gameobject::node_new(light1));
        m_systems.add_entity(gameobject::node_new(light2));
        
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
    m_model->get_component<transform>()->turn(glm::quat{{0.0, glm::radians(-10.0*delta_time), 0.0}});
    //for all
    for(int i=2;i!=5;++i)
    {
        auto model_light = m_systems.get_entities()[i]->get_component<transform>();
        //applay translation
        model_light->position({
                                 std::sin((glm::pi<float>()*0.66)*i+ glm::radians(angle*20.0))*15.,
                                 0.0,
                                 std::cos((glm::pi<float>()*0.66)*i+ glm::radians(angle*20.0))*15.,
                              });
    }
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

