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


void app_basic::key_event(application& app,int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_ESCAPE)
	{
		m_loop = false;
		return;
	}


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
    m_camera->set_viewport(glm::ivec4{0, 0, size.x, size.y});
    //new perspective
    m_camera->set_perspective(m_fov, m_aspect, 0.01, 100.0);
}

void app_basic::start(application& app)
{
	//get info about window
	m_window_mode_info = window_info
	{
		app.get_window_size(),
		app.get_window_position()
	};
    //do clear
    app.clear();
    //camera
    m_camera       = camera::snew();
    glm::vec2 size = app.get_window_size();
    m_aspect       = float(size.x) / float(size.y);
    m_camera->set_viewport(glm::ivec4{0, 0, size.x, size.y});
	m_camera->look_at(glm::vec3{ 0.0f, 4.0f, 6.0f },
				  	  glm::vec3{ 0.0f,-1.0f, 0.0f },
					  glm::vec3{ 0.0f, 1.0f, 0.0f });
    m_camera->set_perspective(m_fov, m_aspect, 0.01, 100.0);
    //set camera
    m_render.set_camera(m_camera);
    //set render pass
#if 0
    m_resources.add_directory("assets/shaders/forward");
    m_render.add_rendering_pass(rendering_pass_base::snew());
#else
    //deferred alloc
    m_resources.add_directory("assets/shaders/deferred");
    auto rendering_pass = rendering_pass_deferred::snew(m_camera, m_resources);
    m_render.add_rendering_pass(rendering_pass);
#endif
    //load assets
    m_resources.add_directory("assets/textures");
    m_resources.add_directory("assets/materials");
    /////// /////// /////// /////// /////// /////// /////// /////// ///////
    // build scene
    {
        //material
        material_ptr box_mat = m_resources.get_material("wire_mat");
        //mesh
        mesh::ptr  cube_mesh = basic_meshs::cube( { 2., 2.0, 2. }, true );
        //add to render
        m_render.add_entity(entity::snew(cube_mesh,box_mat));
        
        //lights
        light_ptr light0 = light_snew();
        light_ptr light1 = light_snew();
        light_ptr light2 = light_snew();
        
        light0->m_diffuse   = { 0.0f, 0.0f, 1.0f, 1.0f };
        light0->m_linear    = 0.15;
        light0->m_const     = 0.9;
        light0->m_quadratic = 0.15;
        
        light1->m_diffuse   = { 1.0f, 0.0f, 0.0f, 1.0f };
        light1->m_linear	= 0.15;
        light1->m_const     = 0.9;
        light1->m_quadratic = 0.15;
        
        light2->m_diffuse   = { 0.0f, 1.0f, 0.0f, 1.0f };
        light2->m_linear    = 0.15;
        light2->m_const     = 0.9;
        light2->m_quadratic = 0.15;
        
        //add to render
        m_render.add_entity(entity::snew(light0));
        m_render.add_entity(entity::snew(light1));
        m_render.add_entity(entity::snew(light2));
        
        //ambient color
        m_render.set_ambient_color(glm::vec4{ 0.16, 0.16, 0.16, 1.0 });
        
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
    glm::mat4& model0 = m_render.get_entities()[0]->m_model;
    model0 = glm::translate(glm::mat4(1), { 0.0f, 0.0f, 0.0f });
    model0 = glm::rotate(model0, float(glm::radians(angle*25.0)), glm::vec3(0.2, 0.8, 0.4));
    //for all
    for(int i=1;i!=4;++i)
    {
        glm::mat4& model_light = m_render.get_entities()[i]->m_model;
        //applay translation
        model_light = glm::translate(glm::mat4(1), glm::vec3
        {
			std::sin((glm::pi<float>()*0.66)*i+angle)*2.5,
            0.0,
            std::cos((glm::pi<float>()*0.66)*i+angle)*2.5,
        });
    }
    //draw
    m_render.draw();
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

