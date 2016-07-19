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

#include <g_buffer.h>
class rendering_pass_deferred : public rendering_pass, public smart_pointers< rendering_pass_deferred >
{
public:

	g_buffer          m_g_buffer;
	mesh::ptr         m_square;
	shader::ptr	      m_shader;
	uniform_int::ptr  m_vertex;
	uniform_int::ptr  m_normal;
	uniform_int::ptr  m_albedo;


	rendering_pass_deferred(camera::ptr camera, resources_manager& resources)
	{
		m_g_buffer.init(camera->get_viewport_size());
		m_square = basic_meshs::square3D({ 2.0,2.0 }, true);
		m_shader = resources.get_shader("deferred_light");
		m_vertex = m_shader->get_shader_uniform_int("g_vertex");
		m_normal = m_shader->get_shader_uniform_int("g_normal");
		m_albedo = m_shader->get_shader_uniform_int("g_albedo_spec");
	}
	

	virtual void draw_pass(camera::ptr camera, std::vector< entity::ptr >& entities)
	{
		const glm::vec4& viewport = camera->get_viewport();
		glViewport(viewport.x, viewport.y, viewport.z, viewport.w);

		m_g_buffer.bind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (entity::ptr& entity : entities)
		{
			entity->m_material->bind(camera.get(), entity->m_model);
			entity->m_renderable->draw();
			entity->m_material->unbind();
		}

		m_g_buffer.unbind();

		m_shader->bind();
		//set texture
		m_g_buffer.set_texture_buffer(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION);
		m_g_buffer.set_texture_buffer(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL);
		m_g_buffer.set_texture_buffer(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO);
		//set uniform id
		m_vertex->setValue(0);
		m_normal->setValue(1);
		m_albedo->setValue(2);
		//draw squere
		m_square->draw();
		//bind
		m_shader->unbind();
	}

};

void app_basic::start(application& app)
{
	//get info about window
	m_window_mode_info = window_info
	{
		app.get_window_size(),
		app.get_window_position()
	};
    //clear color
    glClearColor(0,0,0,0);
    //do clear
    app.clear();
    //load shader
	m_resources.add_directory("assets/textures");
    //camera
    m_camera       = camera::snew();
    glm::vec2 size = app.get_window_size();
    m_aspect       = float(size.x) / float(size.y);
    m_camera->set_viewport(glm::ivec4{0, 0, size.x, size.y});
    m_camera->look_at(glm::vec3{ 0.0f, 0.0f, 5.0f  },
                      glm::vec3{ 0.0f, 0.0f, 0.0f   },
                      glm::vec3{ 0.0f, 1.0f, 0.0f   });
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
	m_render.add_rendering_pass(rendering_pass_deferred::snew(m_camera, m_resources));
#endif
    /////// /////// /////// /////// /////// /////// /////// /////// ///////
    // build scene
    {
        //material
        base_texture_material::ptr base_mat = base_texture_material::snew(m_resources);
        base_mat->set_texture(m_resources.get_texture("box"));
        base_mat->set_color({1,1,1,1});
        //mesh
        mesh::ptr  cube_mesh = basic_meshs::cube( { 1.,1., 1. }, true );
        //add to render
        m_render.add_entity(entity::snew(cube_mesh,base_mat));
    }
}

bool app_basic::run(application& app,double delta_time)
{
	//////////////////////////////////////////////////////////
    //clear screen
    app.clear();
    //rote
    glm::mat4& model0 = m_render.get_entities()[0]->m_model;
    model0 = glm::rotate(model0, float( glm::radians(45.f) * delta_time), glm::vec3( 0.0, 1.0, 0.0 ));
    //////////////////////////////////////////////////////////
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
                             , monitor_mode->width
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

