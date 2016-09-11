//screen position
vec2 projection_space_to_screen_space(in vec4 pos)
{
	vec2 screen_pos = pos.xy / pos.w;
	return (0.5 * (vec2(screen_pos.x, screen_pos.y) + 1.0));
}
//rotation from direction
mat3 rotation_from_direction(in vec3 direction)
{
	//bad case
	if (direction.x == 0 && direction.z == 0)
	{
		if (direction.y < 0)
			return mat3(
				vec3(-1.0, 0.0, 0.0),
				vec3(0.0, -1.0, 0.0),
				vec3(0.0,  0.0, 1.0)
			);
	}
	//good case
	else
	{
		vec3 new_y = normalize(direction);
		vec3 new_z = normalize(cross(new_y, vec3(0.0, 1.0, 0.0)));
		vec3 new_x = normalize(cross(new_y, new_z));

		return mat3(new_x, new_y, new_z);
	}
	//if direction.y >= 0, return I
	return mat3(1.0);
}
