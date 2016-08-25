//
//  transform.cpp
//  HCubo
//
//  Created by Gabriele on 23/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <transform.h>
#include <entity.h>
#include <vector_math.h>

namespace hcube
{
	void transform::look_at(const vec3& eye, const vec3& center, const vec3& up)
	{
#if 0
		mat4 l_look_at  = hcube::look_at(eye, center, up);
		quat l_rotation = hcube::quat(mat3(l_look_at));
		rotation(l_rotation);
		position(eye);
#else
        const hcube::mat4 m4_look_at = hcube::look_at(eye, center, up);
        const hcube::quat q_look_at  = hcube::quat_cast(m4_look_at);
		rotation(hcube::conjugate(q_look_at));
		position(eye);
#endif
	}

	void transform::translation(const vec3& vector)
	{
		m_tranform.m_position += vector;
		set_dirty();
	}

	void transform::turn(const quat& rot)
	{
		m_tranform.m_rotation = rot * m_tranform.m_rotation;
		set_dirty();
	}

	void transform::position(const vec3& pos)
	{
		m_tranform.m_position = pos;
		set_dirty();
	}

	void transform::rotation(const quat& rot)
	{
		m_tranform.m_rotation = rot;
		set_dirty();
	}

	void transform::scale(const vec3& scale)
	{
		m_tranform.m_scale = scale;
		set_dirty();
	}

	vec3 transform::get_position() const
	{
		return  m_tranform.m_position;
	}

	quat transform::get_rotation() const
	{
		return m_tranform.m_rotation;
	}

	vec3 transform::get_scale() const
	{
		return m_tranform.m_scale;
	}

	mat4 const& transform::get_local_matrix()
	{
		compute_matrix();
		return m_model_local_inv;
	}

	mat4 const& transform::get_local_matrix_inv()
	{
		compute_matrix();
		return m_model_local_inv;
	}

	mat4 const& transform::get_matrix()
	{
		compute_matrix();
		return m_model_global;
	}

	mat4 const& transform::get_matrix_inv()
	{
		compute_matrix();
		return m_model_global_inv;
	}

	void transform::on_attach(entity& ent)
	{
		set_dirty();
	}

	void transform::on_detach()
	{
		set_dirty();
	}

	void transform::set_dirty()
	{
		if (!m_tranform.m_dirty)
		{
			m_tranform.m_dirty = true;
			send_dirty();
		}
	}

	void transform::send_dirty()
	{
		if (get_entity())
		{
			get_entity()->send_message_to_component_downwards(type(), { MSG_DIRTY });
		}
	}

	void transform::on_message(const message& message)
	{
		if (message.m_id == MSG_DIRTY) m_tranform.m_dirty = true;
	}

	component_ptr transform::copy() const
	{
		auto cpytransform = transform_snew();
		cpytransform->m_tranform = m_tranform;
		cpytransform->m_model_local = m_model_local;
		cpytransform->m_model_local_inv = m_model_local_inv;
		cpytransform->m_model_global = m_model_global;
		cpytransform->m_model_global_inv = m_model_global_inv;
		cpytransform->m_tranform.m_dirty = true;
		return cpytransform;
	}

	void transform::compute_matrix()
	{
		if (m_tranform.m_dirty)
		{
			//T*R*S
			m_model_local  = hcube::translate(mat4(1.0f), m_tranform.m_position);
			m_model_local *= hcube::mat4_cast(m_tranform.m_rotation);
            m_model_local  = hcube::scale(m_model_local, m_tranform.m_scale);
			//inverse
			m_model_local_inv = inverse(m_model_local);
			//global
			if (get_entity() && get_entity()->get_parent())
			{
				m_model_global = get_entity()->get_parent()->get_component<transform>()->get_matrix() * m_model_local;
				//inverse
				m_model_global_inv = inverse(m_model_global);
			}
			else
			{
				m_model_global = m_model_local;
				m_model_global_inv = m_model_local_inv;
			}
		}
	}
}