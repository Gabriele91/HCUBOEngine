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
#include <mutex>
#include <queue>
#include <thread>
#include <future>
#include <functional>
#include <condition_variable>
#include <GLFW/glfw3.h>
#include <hcube/config.h>
#include <hcube/math/vector_math.h>

namespace hcube
{

	class application;
	class instance;


	struct HCUBE_API window_size
	{
		virtual ivec2 get_size(GLFWmonitor* monitor) const = 0;
	};

	struct HCUBE_API window_size_pixel : window_size
	{
		ivec2 m_size;

		window_size_pixel(const ivec2& size);
		virtual ivec2 get_size(GLFWmonitor* monitor) const;
	};

	struct HCUBE_API window_size_percentage : window_size
	{
		dvec2 m_size;

		window_size_percentage(const dvec2& size);
		virtual ivec2 get_size(GLFWmonitor* monitor) const;
	};

	class HCUBE_API instance
	{
	public:
		virtual void key_event(application& application, int key, int scancode, int action, int mods) { }
		virtual void cursor_position_event(application& application, const dvec2& pos) { }
		virtual void mouse_button_event(application& application, int button, int action, int mods) { }
		virtual void scroll_event(application& application, const dvec2& scroll_offset) { }
		virtual void resize_event(application& application, const ivec2& size) { }
		virtual void start(application& application) = 0;
		virtual bool run(application& application, double delta_time) = 0;
		virtual bool end(application& application) = 0;
		virtual ~instance() {}
	};

	enum class window_mode
	{
		RESIZABLE,
		NOT_RESIZABLE,
		FULLSCREEN
	};

	class HCUBE_API application
	{
	public:
		//type of task
		enum task_type
		{
			MAIN_THREAD,
			OTHERS_THREADS
		};
		//type ok callback
		using task_function = std::function<void(void)>;
		//list of task to do
		using tasks   = std::queue< task_function >;
		using workers = std::vector< std::thread >;
		//application
		application();
		~application();
		//actions
		void clear() const;
		void swap() const;
		//info
		bool   is_fullscreen() const;
		bool   is_resizable() const;
		ivec2  get_screen_size() const;
		ivec2  get_window_size() const;
		ivec2  get_window_position() const;
		dvec2  get_mouse_position() const;
		double get_last_delta_time() const;
		//change size
		void set_window_size(const dvec2& pos, const dvec2& size);
		void set_fullscreen_size(const dvec2& size);
		//set
		void set_mouse_position(const dvec2& pos) const;
		//get attr
		instance* get_instance();
		GLFWwindow* get_window();
		const instance* get_instance() const;
		const GLFWwindow* get_window() const;
		//push task to main thread
		template<class function, class... arguments>
		auto push_task(task_type type, function&& f, arguments&&... args)
			 -> std::future<typename std::result_of<function(arguments...)>::type>;
		//execute a instance
		bool execute
		(
			const window_size& size,
			window_mode mode,
			int major_gl_ctx,
			int minor_gl_ctx,
			const std::string& app_name,
			instance* app,
			size_t n_workers = 4
		);

	private:

		GLFWwindow*	m_window{ nullptr };
		instance*	m_instance{ nullptr };
		double		m_last_delta_time{ 0 };
		bool		m_is_resizable{ false };

		workers					m_workers;
		bool					m_workers_stop;
		std::mutex				m_workers_mutex;
		tasks				    m_workers_tasks;
		std::condition_variable m_workers_condition;

		std::mutex	m_main_mutex;
		tasks		m_main_tasks;

		//execute main task
		void execute_a_main_task();
		void execute_all_main_task();
		//utilities
		void init_threads_pool(size_t n_threads);
		void delete_threads_pool();

	};
	//main task
	inline void application::execute_a_main_task()
	{
		std::function<void()> task;
		{
			//lock main thread
			std::unique_lock<std::mutex> lock(m_main_mutex);
			//exist case
			if (m_main_tasks.empty()) return;
			//get task
			task = std::move(m_main_tasks.front());
			//pop
			m_main_tasks.pop();
		}
		//execute
		task();
	}
	inline void application::execute_all_main_task()
	{
		while (true)
		{
			std::function<void()> task;
			{
				//lock main thread
				std::unique_lock<std::mutex> lock(m_main_mutex);
				//exist case (exit from while!)
				if (m_main_tasks.empty()) return;
				//get task
				task = std::move(m_main_tasks.front());
				//pop
				m_main_tasks.pop();
			}
			//execute
			task();
		}
	}
	//thread pool
	template<class function, class... arguments>
	inline auto application::push_task(task_type type, function&& f, arguments&&... args) -> std::future<typename std::result_of<function(arguments...)>::type>
	{
		using return_type = typename std::result_of<function(arguments...)>::type;
		//type of queue
		if (type == MAIN_THREAD)
		{
			//make task
			auto task = std::make_shared< std::packaged_task<return_type()> >
			(
				std::bind(std::forward<function>(f), std::forward<arguments>(args)...)
			);
			//return future result
			std::future<return_type> res = task->get_future();
			{
				//lock m_woker_tasks
				std::unique_lock<std::mutex> lock(m_main_mutex);
				//push
				m_main_tasks.emplace([task](){ (*task)(); });
			}
			//return promise
			return res;
		}
		else
		{
			//make task
			auto task = std::make_shared< std::packaged_task<return_type()> >
			(
				std::bind(std::forward<function>(f), std::forward<arguments>(args)...)
			);
			//return future result
			std::future<return_type> res = task->get_future();
			{
				//lock m_woker_tasks
				std::unique_lock<std::mutex> lock(this->m_workers_mutex);
				// don't allow enqueueing after stopping the pool
				if (this->m_workers_stop) throw std::runtime_error("push a task on stopped thread pool");
				//push
				this->m_workers_tasks.emplace([task]() { (*task)(); });
			}
			//notify push
			m_workers_condition.notify_one();
			//return promise
			return res;
		}
	}
	inline void application::init_threads_pool(size_t n_threads)
	{
		//enable
		this->m_workers_stop = false;
		//init
		for (size_t i = 0; i < n_threads; ++i)
		{
			m_workers.emplace_back(
				[this]()
				{
					while (true)
					{
						std::function<void()> task;
						{
							//lock thread
							std::unique_lock<std::mutex> lock(this->m_workers_mutex);
							//wait condition
							this->m_workers_condition.wait(lock,
								[this]
								{
									return this->m_workers_stop || !this->m_workers_tasks.empty();
								}
							);
							//exit case
							if (this->m_workers_stop && this->m_workers_tasks.empty())
								return;
							//get task
							task = std::move(this->m_workers_tasks.front());
							//pop
							this->m_workers_tasks.pop();
						}
						//execute
						task();
					}
				}
			);
		}
	}
	inline void application::delete_threads_pool()
	{
		//stop
		{
			std::unique_lock<std::mutex> lock(m_workers_mutex);
			m_workers_stop = true;
		}
		//notify to all
		m_workers_condition.notify_all();
		//wait all
		for (std::thread& worker : m_workers) worker.join();
		//clear workers list
		m_workers.clear();
	}


};