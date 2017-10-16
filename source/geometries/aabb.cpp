#include <hcube/geometries/aabb.h>
namespace hcube
{
	aabb::aabb(const vec3& min, const vec3 max)
	{
		m_min = min;
		m_max = max;
	}

	const vec3& aabb::get_min() const
	{
		return m_min;
	}

	const vec3& aabb::get_max() const
	{
		return m_max;
	}

	vec3 aabb::get_center() const
	{
		return (m_max + m_min) / 2.0f;
	}

	vec3 aabb::get_extension() const
	{
		return abs(m_max - get_center());
	}

	std::vector< vec3 > aabb::get_bounding_box() const
	{
		std::vector< vec3 > p;
		p.resize(8);
		
		vec3  r(1, 0, 0);
		vec3  u(0, 1, 0);
		vec3  f(0, 0, 1);
		const vec3& extension = get_extension();
		const vec3& position  = get_center();

		p[0] = position - r*extension[0] - u*extension[1] - f*extension[2];
		p[1] = position + r*extension[0] - u*extension[1] - f*extension[2];
		p[2] = position + r*extension[0] - u*extension[1] + f*extension[2];
		p[3] = position - r*extension[0] - u*extension[1] + f*extension[2];
		p[4] = position - r*extension[0] + u*extension[1] - f*extension[2];
		p[5] = position + r*extension[0] + u*extension[1] - f*extension[2];
		p[6] = position + r*extension[0] + u*extension[1] + f*extension[2];
		p[7] = position - r*extension[0] + u*extension[1] + f*extension[2];

		return p;

	}

	std::vector< vec3 > aabb::get_bounding_box(const mat4& model) const
	{
		auto output = get_bounding_box();
		for (vec3& point : output) point = vec3(model * vec4(point, 1.0));
		return output;
	}

	void aabb::applay(const mat4& model)
	{
		//info aabb
		vec3 center     = get_center();
		vec3 extension  = m_max - center;
		//info transormation
		mat3 scale_rotation(model);
		vec3 position(model[3]);
		//compute new coors
		vec3 new_center    = center*scale_rotation + position;
		vec3 new_extension = extension*scale_rotation;
		//save new aabb
		m_max = new_center + new_extension;
		m_min = new_center - new_extension;
	}

	aabb  aabb::operator*  (const mat4& model) const
	{
		aabb out; out.applay(model); return out;
	}

	aabb& aabb::operator*= (const mat4& model)
	{
		this->applay(model); return *this;
	}

	vec3 aabb::closest_point(const vec3& point) const
	{
		vec3 result;

		     if (point.x > m_max.x) result.x = m_max.x;
	    else if (point.x < m_min.x) result.x = m_min.x;
		else  result.x = point.x;

			 if (point.y > m_max.y) result.y = m_max.y;
		else if (point.y < m_min.y) result.y = m_min.y;
		else  result.y = point.y;

			 if (point.z > m_max.z) result.z = m_max.z;
		else if (point.z < m_min.z) result.z = m_min.z;
		else  result.z = point.z;

		return result;

	}
}