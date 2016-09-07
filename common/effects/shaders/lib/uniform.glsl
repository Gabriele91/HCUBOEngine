
struct __CAMERA__
{
	vec4 m_viewport;
	mat4 m_projection;
	mat4 m_view;
	mat4 m_model;
	vec3 m_position;
};
uniform __CAMERA__ camera;

struct __TRANSFORM__
{
	mat4 m_model;
	vec3 m_position;
	mat4 m_rotation;
	vec3 m_scale;
};
uniform __TRANSFORM__ transform;