//
//  application.h
//  OGLHCubeView
//
//  Created by Gabriele on 28/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <string>
#include <tuple>
#include <OpenGL4.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
class application;
class instance;


struct window_size
{
	virtual glm::ivec2 get_size(GLFWmonitor* monitor) const = 0;
};

struct window_size_pixel : window_size
{
	glm::ivec2 m_size;

	window_size_pixel(const glm::ivec2& size);
	virtual glm::ivec2 get_size(GLFWmonitor* monitor) const;
};

struct window_size_percentage : window_size
{
	glm::dvec2 m_size;

	window_size_percentage(const glm::dvec2& size);
	virtual glm::ivec2 get_size(GLFWmonitor* monitor) const;
};

class instance
{
public:
    virtual void key_event(application& application,int key, int scancode, int action, int mods) { }
    virtual void cursor_position_event(application& application,const glm::dvec2& pos) { }
    virtual void mouse_button_event(application& application,int button, int action, int mods) { }
    virtual void scroll_event(application& application,const glm::dvec2& scroll_offset) { }
    virtual void resize_event(application& application,const glm::ivec2& size){ }
    virtual void start(application& application) = 0;
    virtual bool run(application& application,double delta_time) = 0;
    virtual bool end(application& application) = 0;
};


class application
{
    GLFWwindow* m_window   { nullptr };
    instance*   m_instance { nullptr };
	double		m_last_delta_time{ 0 };
	bool		m_is_resizable{ false };

public:
    
    application();
    ~application();
    //actions
    void clear() const;
    void swap() const;
    //info
	bool       is_resizable() const;
	glm::ivec2 get_screen_size() const;
	glm::ivec2 get_window_size() const;
	glm::ivec2 get_window_position() const;
	glm::dvec2 get_mouse_position() const;
	double get_last_delta_time() const;
	//set
	void set_mouse_position(const glm::dvec2& pos) const;
    //get attr
    instance* get_instance();
    GLFWwindow* get_window();


    bool execute(const window_size& size,
                 int resizable,
                 int major_gl_ctx,
                 int minor_gl_ctx,
                 const std::string& app_name,
                 instance* app);
    
};
