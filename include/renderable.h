//
//  renderable.h
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <component.h>
#include <smart_pointers.h>
#include <material.h>
#include <obb.h>

namespace hcube
{
	class renderable : public component
	{

		HCUBE_COMPONENT_DEC(renderable)

	public:

		renderable() {}

		virtual ~renderable() {};

		virtual void draw()
		{
			assert(0);
		};

		virtual void on_attach(entity& e)
		{
			set_obb_has_dirty();
		}

		virtual void on_detach()
		{
			set_obb_has_dirty();
		}

		bool on_activate()
		{
			set_obb_has_dirty();
			return true;
		}

		void set_support_culling(bool culling)
		{
			m_support_culling = culling;
		}

		bool has_support_culling() const
		{
			return m_support_culling;
		}

		virtual const obb& get_bounding_box()
		{
			update_obb();
			return m_global_bounding_box;
		}

		void set_bounding_box(const obb& in_obb)
		{
			m_bounding_box = in_obb;
			set_obb_has_dirty();
		}

		material_ptr get_material() const
		{
			return m_material;
		}

		void set_material(material_ptr material)
		{
			m_material = material;
		}

		virtual void on_message(const message& msg)
		{
			if (msg.m_id == transform::MSG_DIRTY)
			{
				set_obb_has_dirty();
			}
		}

		void update_obb()
		{
			if (m_obb_dirty)
			{
				//to void
				m_obb_dirty = false;
				m_global_bounding_box = obb();
				//test
				if (get_entity())
				if (auto t_entity = get_entity()->get_component<transform>())
				{
					m_global_bounding_box = m_bounding_box;
					m_global_bounding_box.applay(t_entity->get_matrix());
				}

			}
		}

	private:

		obb			 m_bounding_box;
		obb			 m_global_bounding_box;
		material_ptr m_material;
		bool		 m_support_culling{ false };
		bool		 m_obb_dirty{ true };

		void set_obb_has_dirty()
		{
			m_obb_dirty = true;
		}

	};

	using renderable_ptr = std::shared_ptr< renderable >;
	using renderable_uptr = std::unique_ptr< renderable >;
	using renderable_wptr = std::weak_ptr  < renderable >;
}