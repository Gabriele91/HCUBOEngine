
#pragma once
#include <vector_math.h>

namespace hcube
{
	class sphere
	{
	public:

		sphere() {}

		sphere(const vec3& in_position, float in_radius)
		{
			m_position = in_position;
			m_radius   = in_radius;
		}

		void position(const vec3& in_position)
		{
			m_position = in_position;
		}

		void radius(float in_radius)
		{
			m_radius = in_radius;
		}

		const vec3& get_position() const
		{
			return m_position;
		}

		float get_radius() const
		{
			return m_radius;
		}

		float volume() const
		{
			return 4.0f * constants::pi<float>() * std::pow(m_radius, 2);
		}

	protected:

		vec3  m_position;
		float m_radius{ 0.0 };

	};
}