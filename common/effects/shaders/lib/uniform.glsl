
struct __CAMERA__
{
	vec4 viewport;
	mat4 projection;
	mat4 view;
	mat4 model;
	vec3 position;
};

uniform __CAMERA__ camera;

struct __TRANSFORM__
{
	mat4 model;
	vec3 position;
	mat4 rotation;
	vec3 scale;
};

uniform __TRANSFORM__ transform;