//
//  transform.h
//  HCubo
//
//  Created by Gabriele on 23/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <component.h>
#include <vector_math.h>

namespace hcube
{
	class transform : public component
	{

		HCUBE_COMPONENT_DEC(transform)

	public:

		enum message_ids
		{
			MSG_DIRTY = 1,
		};

		void on_attach(entity& ent);
		void on_detach();
		void on_message(const message& message);


		void look_at(const vec3& eye, const vec3& center, const vec3& up);
		void translation(const vec3& vector);
		void turn(const quat& rot);

		void position(const vec3& pos);
		void rotation(const quat& rot);
		void scale(const vec3& pos);

		vec3 get_position() const;
		quat get_rotation() const;
		vec3 get_scale() const;

		vec3 get_global_position();
		quat get_global_rotation();
		vec3 get_global_scale();

		mat4 const& get_local_matrix();
		mat4 const& get_local_matrix_inv();

		mat4 const& get_matrix();
		mat4 const& get_matrix_inv();

		inline operator const mat4& ()
		{
			return get_matrix();
		}

		inline bool is_dirty() const
		{
			return m_tranform.m_dirty;
		}

		virtual component_ptr copy() const;

	private:
		//info
		struct local_tranform
		{
			bool      m_dirty;
			vec3 m_position;
			quat m_rotation;
			vec3 m_scale{ 1.0f, 1.0f, 1.0f };
		}
		m_tranform;
		//send dirty
		void    set_dirty();
		void    send_dirty();
		//matrix
		mat4  m_model_local;
		mat4  m_model_local_inv;
		mat4  m_model_global;
		mat4  m_model_global_inv;
		//compute
		void compute_matrix();

	};

	using transform_ptr = std::shared_ptr< transform >;
	using transform_uptr = std::unique_ptr< transform >;
	using transform_wptr = std::weak_ptr< transform >;

	template < class... Args >
	static inline transform_ptr transform_snew(Args&&... args)
	{
		return std::make_shared< transform >(args...);
	}
}