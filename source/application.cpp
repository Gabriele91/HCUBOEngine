#include <string>
#include <iostream>
#include <memory>
#include <algorithm>
#include <application.h>
#include <query_performance.h>
#include <render.h>

namespace hcube
{
	application::application()
	{
		glfwInit();
	}

	application::~application()
	{
		glfwTerminate();
	}

	void application::clear() const
	{
		//surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void application::swap() const
	{
		glfwSwapBuffers(m_window);
	}


	bool application::is_resizable() const
	{
		return m_is_resizable;
	}

	glm::ivec2 application::get_screen_size() const
	{
		//get screen size
		int n_monitors = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&n_monitors);
		//info monitor/screen
		const GLFWvidmode* monitor_mode = glfwGetVideoMode(monitors[0]);
		//return size
		return glm::ivec2
		{
			(int)monitor_mode->width,
			(int)monitor_mode->height
		};
	}

	glm::ivec2 application::get_window_size() const
	{

		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);
		return glm::ivec2{ width, height };
	}

	glm::ivec2 application::get_window_position() const
	{
		int x, y;
		glfwGetWindowPos(m_window, &x, &y);
		return glm::ivec2{ x, y };
	}

	glm::dvec2 application::get_mouse_position() const
	{
		double xpos, ypos;
		glfwGetCursorPos(m_window, &xpos, &ypos);
		return glm::dvec2{ xpos, ypos };
	}

	void application::set_mouse_position(const glm::dvec2& pos) const
	{
		glfwSetCursorPos(m_window, pos.x, pos.y);
	}

	double application::get_last_delta_time() const
	{
		return m_last_delta_time;
	}

	//get attr
	instance* application::get_instance()
	{
		return m_instance;
	}

	GLFWwindow* application::get_window()
	{
		return m_window;
	}

	window_size_pixel::window_size_pixel(const glm::ivec2& size)
	{
		m_size = size;
	}

	glm::ivec2 window_size_pixel::get_size(GLFWmonitor* monitor) const
	{
		return m_size;
	}

	window_size_percentage::window_size_percentage(const glm::dvec2& size)
	{
		m_size = size;
	}

	glm::ivec2 window_size_percentage::get_size(GLFWmonitor* monitor) const
	{
		const GLFWvidmode* monitor_mode = glfwGetVideoMode(monitor);
		return glm::ivec2
		{
			(int)((double)monitor_mode->width * m_size.x / 100.0f),
			(int)((double)monitor_mode->height * m_size.y / 100.0f)
		};
	}

	bool application::execute(const window_size& size,
		int resizable,
		int major_gl_ctx,
		int minor_gl_ctx,
		const std::string& app_name,
		instance* app)
	{
		//save
		m_instance = app;
		//resize ?
		m_is_resizable = resizable ? true : false;
		//set context version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_gl_ctx);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_gl_ctx);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);
		//get screen size
		int n_monitors = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&n_monitors);
		//size
		glm::ivec2 window_size = size.get_size(monitors[0]);
		//create window
		m_window = glfwCreateWindow(window_size.x, window_size.y, app_name.c_str(), NULL, NULL);
		//center
		const GLFWvidmode* monitor_mode = glfwGetVideoMode(monitors[0]);
		//center
		glfwSetWindowPos(m_window, (monitor_mode->width - window_size.x) / 2, (monitor_mode->height - window_size.y) / 2);
		//set window is connected to instance
		glfwSetWindowUserPointer(m_window, this);
		//test
		if (!m_window) return false;
		//disable cursor
		//glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//make current
		glfwMakeContextCurrent(m_window);
		glfwSwapInterval(1);
		render::print_errors();
		//init
		render::init();
		//show info
		render::print_info();
		//disable vSync
		glfwSwapInterval(0);
		//enable capture ctrl
		glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GLFW_TRUE);
		glfwSetInputMode(m_window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
		//set events
		glfwSetKeyCallback(m_window, []
		(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			//context
			application* l_app = (application*)glfwGetWindowUserPointer(window);
			//call
			l_app->get_instance()->key_event(*l_app, key, scancode, action, mods);
		});
		glfwSetCursorPosCallback(m_window, []
		(GLFWwindow* window, double xpos, double ypos)
		{
			//context
			application* l_app = (application*)glfwGetWindowUserPointer(window);
			//call
			l_app->get_instance()->cursor_position_event(*l_app, glm::dvec2{ xpos,ypos });
		});
		glfwSetMouseButtonCallback(m_window, []
		(GLFWwindow* window, int button, int action, int mods)
		{
			//context
			application* l_app = (application*)glfwGetWindowUserPointer(window);
			//call
			l_app->get_instance()->mouse_button_event(*l_app, button, action, mods);
		});
		glfwSetScrollCallback(m_window, []
		(GLFWwindow* window, double xoffset, double yoffset)
		{
			//context
			application* l_app = (application*)glfwGetWindowUserPointer(window);
			//call
			l_app->get_instance()->scroll_event(*l_app, glm::dvec2{ xoffset,yoffset });
		});
		glfwSetWindowSizeCallback(m_window, []
		(GLFWwindow* window, int width, int height)
		{
			//context
			application* l_app = (application*)glfwGetWindowUserPointer(window);
			//call
			l_app->get_instance()->resize_event(*l_app, glm::ivec2{ width,height });
		});
		//start
		m_instance->start(*this);
		render::print_errors();
		//time
		double old_time = 0;
		double last_time = query_performance::get_time();
		//loop
		while (!glfwWindowShouldClose(m_window))
		{
			//compute delta time
			old_time = last_time;
			last_time = query_performance::get_time();
			//update delta time
			m_last_delta_time = std::max(last_time - old_time, 0.0001);
			//update
			if (!m_instance->run(*this, m_last_delta_time)) break;
			//print
			render::print_errors();
			//swap
			glfwSwapBuffers(m_window);
			glfwPollEvents();
		}
		bool end_state = m_instance->end(*this);
		//dealloc
		delete m_instance;
		m_instance = nullptr;
		//delete context
		render::close();
		//delete window
		glfwDestroyWindow(m_window);
		//to 0
		m_window = nullptr;
		m_instance = nullptr;
		//return status
		return end_state;
	}
}