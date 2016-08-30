//
//  rendering_system.h
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <queue>
#include <camera.h>
#include <entity.h>
#include <effect.h>
#include <smart_pointers.h>
#include <system_manager.h>
#include <vector_math.h>

namespace hcube
{
	class render_queues
	{
	public:

		struct element
		{
			element(entity::wptr ref) : m_ref(ref) {};
			//ref to object
			entity::wptr m_ref;
			//list
			element* m_next{ nullptr };
			float    m_depth{ ~0 };
			//fake lock
			entity::ptr lock()
			{
				return m_ref.lock();
			}
		};

		using queue = std::vector < element >;

		queue m_lights;
		queue m_opaque;
		queue m_translucent;

		//culling
		element* m_cull_light_spot     { nullptr };
		element* m_cull_light_point    { nullptr };
		element* m_cull_light_direction{ nullptr };
		element* m_cull_opaque{ nullptr };
		element* m_cull_translucent{ nullptr };

		void add_call_light_spot(element* e);
		void add_call_light_point(element* e);
		void add_call_light_direction(element* e);
		void add_call_opaque(element* e);
		void add_call_translucent(element* e);

		void clear();
		void push(entity::ptr e);
		void remove(entity::ptr e);
		void reserve(size_t size);


		void compute_light_queue(const frustum& view_frustum);
		void compute_opaque_queue(const frustum& view_frustum);
		void compute_translucent_queue(const frustum& view_frustum);
	};

	#define HCUBE_FOREACH_QUEUE(name,queue)\
		for (render_queues::element* name = queue; name; name = name->m_next)

	class rendering_pass
	{
	public:
		virtual void draw_pass(
			vec4&  clear_color,
			vec4&  ambient_color,
			entity::ptr e_camera,
			render_queues& queues
		) = 0;
	};
	using rendering_pass_ptr = std::shared_ptr< rendering_pass >;
	using rendering_pass_uptr = std::unique_ptr< rendering_pass >;

	class rendering_pass_shadow : public rendering_pass, public smart_pointers<rendering_pass_shadow>
	{
		
		effect::ptr		   m_effect;
		effect::technique* m_technique_shadow_spot;
		effect::technique* m_technique_shadow_point;
		effect::technique* m_technique_shadow_direction;

	public:

		rendering_pass_shadow(resources_manager& resources);

		virtual void draw_pass(
			vec4&  clear_color,
			vec4&  ambient_color,
			entity::ptr e_camera,
			render_queues& queues
		);
	};

	class rendering_system : public system_component, public smart_pointers< rendering_system >
	{

		HCUBE_SYSTEM_COMPONENT_DEC(rendering_system);

	public:


		virtual void on_attach(system_manager&);

		virtual void on_detach();

		virtual void on_add_entity(entity::ptr);

		virtual void on_remove_entity(entity::ptr);

		virtual void on_update(double deltatime);

		void set_clear_color(const vec4& clear_color);

		void set_ambient_color(const vec4& ambient_color);

		void add_rendering_pass(rendering_pass_ptr pass);

		void add_shadow_rendering_pass(rendering_pass_ptr pass);

		void draw();

		const vec4& get_clear_color() const;

		const vec4& get_ambient_color() const;

		entity::ptr get_camera() const;

		const std::vector< rendering_pass_ptr >& get_rendering_pass() const;

		void stop_update_frustum(bool stop_update);

		void stop_frustum_culling(bool stop_culling);

	protected:
		
		bool m_update_frustum{ true };
		bool m_stop_frustum_culling{ false };

		vec4                              m_clear_color;
		vec4                              m_ambient_color;
		entity::ptr						  m_camera;
		render_queues					  m_renderables;
		std::vector< rendering_pass_ptr > m_rendering_pass;
		//light
		rendering_pass_ptr				  m_shadow_pass;
	};
}