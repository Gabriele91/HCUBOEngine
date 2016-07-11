#include <basic_meshs.h>

namespace basic_meshs
{
	mesh::ptr cube(const glm::vec3& cube_size)
	{
		glm::vec3& cube_size_h = cube_size * 0.5f;
		//cube
		mesh::ptr mesh_cube = mesh::snew();
		//build
		mesh_cube->build
		(
		{
			// front
			0, 1, 2,
			2, 3, 0,
			// top
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// bottom
			4, 0, 3,
			3, 7, 4,
			// left
			4, 5, 1,
			1, 0, 4,
			// right
			3, 2, 6,
			6, 7, 3,
		},
		{
			{ -cube_size_h.x, -cube_size_h.y,   cube_size_h.z },
			{  cube_size_h.x, -cube_size_h.y,   cube_size_h.z },
			{  cube_size_h.x,  cube_size_h.y,   cube_size_h.z },
			{ -cube_size_h.x,  cube_size_h.y,   cube_size_h.z },

			{ -cube_size_h.x, -cube_size_h.y,  -cube_size_h.z },
			{  cube_size_h.x, -cube_size_h.y,  -cube_size_h.z },
			{  cube_size_h.x,  cube_size_h.y,  -cube_size_h.z },
			{ -cube_size_h.x,  cube_size_h.y,  -cube_size_h.z }
		}
		);
		//triangles
		mesh_cube->set_draw_info({ GL_TRIANGLES });
		//return cube
		return mesh_cube;
	}
}