#include <hcube/math/vector_math.h>
#include <hcube/geometries/obb.h>
#include <glm/gtx/matrix_decompose.hpp>

namespace hcube
{
	obb::obb(const mat3& rotation, const vec3& position, const vec3& extension)
	{
		m_rotation = rotation;
		m_position = position;
		m_extension = extension;
	}

	float obb::volume() const
	{
		return 8 * m_extension[0] * m_extension[1] * m_extension[2];
	}

	std::vector< vec3 > obb::get_bounding_box() const
	{
		std::vector< vec3 > p;
		p.resize(8);
		vec3 r(m_rotation[0][0], m_rotation[1][0], m_rotation[2][0]);
		vec3 u(m_rotation[0][1], m_rotation[1][1], m_rotation[2][1]);
		vec3 f(m_rotation[0][2], m_rotation[1][2], m_rotation[2][2]);
		p[0] = m_position - r*m_extension[0] - u*m_extension[1] - f*m_extension[2];
		p[1] = m_position + r*m_extension[0] - u*m_extension[1] - f*m_extension[2];
		p[2] = m_position + r*m_extension[0] - u*m_extension[1] + f*m_extension[2];
		p[3] = m_position - r*m_extension[0] - u*m_extension[1] + f*m_extension[2];
		p[4] = m_position - r*m_extension[0] + u*m_extension[1] - f*m_extension[2];
		p[5] = m_position + r*m_extension[0] + u*m_extension[1] - f*m_extension[2];
		p[6] = m_position + r*m_extension[0] + u*m_extension[1] + f*m_extension[2];
		p[7] = m_position - r*m_extension[0] + u*m_extension[1] + f*m_extension[2];
		return p;
	}

	std::vector< vec3 > obb::get_bounding_box(const mat4& model) const
	{
		std::vector< vec3 > p;
		p.resize(8);
		vec3 r(m_rotation[0][0], m_rotation[1][0], m_rotation[2][0]);
		vec3 u(m_rotation[0][1], m_rotation[1][1], m_rotation[2][1]);
		vec3 f(m_rotation[0][2], m_rotation[1][2], m_rotation[2][2]);
		p[0] = m_position - r*m_extension[0] - u*m_extension[1] - f*m_extension[2];
		p[1] = m_position + r*m_extension[0] - u*m_extension[1] - f*m_extension[2];
		p[2] = m_position + r*m_extension[0] - u*m_extension[1] + f*m_extension[2];
		p[3] = m_position - r*m_extension[0] - u*m_extension[1] + f*m_extension[2];
		p[4] = m_position - r*m_extension[0] + u*m_extension[1] - f*m_extension[2];
		p[5] = m_position + r*m_extension[0] + u*m_extension[1] - f*m_extension[2];
		p[6] = m_position + r*m_extension[0] + u*m_extension[1] + f*m_extension[2];
		p[7] = m_position - r*m_extension[0] + u*m_extension[1] + f*m_extension[2];
		//mul by model matrix
		for (vec3& point : p)
		{
			point = (vec3)(model * vec4(point, 1.0));
		}		
		return p;
	}

	/*
	* Applay a matrix to obb
	*/
	void obb::applay(const mat4& model)
	{
		mat4
		transform = translate(mat4(1), m_position);
		transform*= mat4(m_rotation);
		//new box
		transform = model * transform;
#if 0
		//decompone
		vec3 scale;
		quat rotation;
		vec3 translation;
		vec3 skew;
		vec4 perspective;
		glm::decompose(transform, scale, rotation, translation, skew, perspective);
		//to obb
		m_position  = translation;
		m_rotation  = mat3_cast( rotation );
		m_extension = scale * m_extension;
#else
		//to obb
		m_position   = vec3(transform[3]);
		//rotation
		auto r_scale = mat3(transform);
		auto scale   = vec3(length(r_scale[0]),
							length(r_scale[1]),
							length(r_scale[2]));
		r_scale[0] /= scale[0];
		r_scale[1] /= scale[1];
		r_scale[2] /= scale[2];
		m_rotation = traspose(inverse(r_scale));
		//scale
		m_extension = scale * m_extension;
#endif
	}

	/*
	* Applay a matrix to obb and return the new obb
	*/
	obb  obb::operator*  (const mat4& model) const
	{
		obb new_obb(*this);
		new_obb.applay(model);
		return new_obb;
	}

	/*
	* Applay a matrix to obb and return this
	*/
	obb& obb::operator*= (const mat4& model)
	{
		this->applay(model);
		return *this;
	}

	// The implementation of this function is from Christer Ericson's Real-Time Collision Detection, p.133.
	vec3 obb::closest_point(const vec3& target) const
	{
		// Best: 33.412 nsecs / 89.952 ticks, Avg: 33.804 nsecs, Worst: 34.180 nsecs
		vec3 d = target - m_position;
		// Start at the center point of the OBB.
		vec3 closest_point = m_position; 
		//axis
		const mat3& axis = transpose(m_rotation);
		// Project the target onto the OBB axes and walk towards that point.
		for (int i = 0; i < 3; ++i)
		{
			closest_point += clamp(dot(d, axis[i]), -m_extension[i], m_extension[i]) * axis[i];
		}

		return closest_point;
	}

}