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
#define OPENGL_VIEW //todo render api fix

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
        const hcube::mat4 m4_look_at = hcube::look_at(-eye, center, up);
        const hcube::quat q_look_at  = hcube::to_quat(m4_look_at);
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

	vec3 transform::get_global_position()
	{
		return vec3(get_matrix()[3]);
	}

	quat transform::get_global_rotation()
	{
		mat3 rot_scale(get_matrix());
		//len
		vec3 scale(length(rot_scale[0]),
				   length(rot_scale[1]),
			       length(rot_scale[2]));
		//normalize 
		rot_scale[0] /= scale[0];
		rot_scale[1] /= scale[1];
		rot_scale[2] /= scale[2];
		//must to be a ortogonal matrix
		return  quat_cast(traspose(inverse(rot_scale)));
	}

	vec3 transform::get_global_scale()
	{
		mat3 rot_scale(get_matrix());

		return vec3(length(rot_scale[0]),
					length(rot_scale[1]),
					length(rot_scale[2]));
	}

	mat4 const& transform::get_local_matrix()
	{
		compute_matrix();
		return m_model_local;
	}

	mat4 const& transform::get_matrix()
	{
		compute_matrix();
		return m_model_global;
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
			get_entity()->send_message_to_components_downwards({ MSG_DIRTY }, true);
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
		cpytransform->m_model_global = m_model_global;
		cpytransform->m_tranform.m_dirty = true;
		return cpytransform;
	}

	void transform::compute_matrix()
	{
		if (m_tranform.m_dirty)
		{
			//T*R*S
			m_model_local  = hcube::translate(mat4(1.0f), m_tranform.m_position);
			m_model_local *= hcube::to_mat4(  m_tranform.m_rotation );
            m_model_local  = hcube::scale(m_model_local, m_tranform.m_scale);
			//global
			if (get_entity() && get_entity()->get_parent())
			{
				m_model_global = get_entity()->get_parent()->get_component<transform>()->get_matrix() * m_model_local;
			}
			else
			{
				m_model_global = m_model_local;
			}
			m_tranform.m_dirty = false;
		}
	}



	void uniform_transform::get_uniform(shader::ptr shader)
	{
		m_uniform_model    = shader->get_uniform("transform.m_model");
		m_uniform_position = shader->get_uniform("transform.m_position");
		m_uniform_rotation = shader->get_uniform("transform.m_rotation");
		m_uniform_scale    = shader->get_uniform("transform.m_scale");

		m_valid =
			   m_uniform_model
			|| m_uniform_position
			|| m_uniform_rotation
			|| m_uniform_scale;
	}

	void uniform_transform::uniform(transform_wptr ptr_transform) const
	{
		auto c_transform = ptr_transform.lock();
		if (m_uniform_model)    m_uniform_model->set_value(c_transform->get_matrix());
		if (m_uniform_position) m_uniform_position->set_value(c_transform->get_global_position());
		if (m_uniform_rotation) m_uniform_rotation->set_value(mat4_cast(c_transform->get_global_rotation()));
		if (m_uniform_scale)    m_uniform_scale->set_value(c_transform->get_global_scale());
	}

	bool uniform_transform::is_valid() const
	{
		return m_valid;
	}

}