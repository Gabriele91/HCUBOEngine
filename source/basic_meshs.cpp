#include <basic_meshs.h>

namespace basic_meshs
{
	mesh::ptr cube(const glm::vec3& cube_size,bool use_uvmap)
	{
		const glm::vec3& cube_size_h = cube_size * 0.5f;
		//cube
		mesh::ptr mesh_cube = mesh::snew();
		//build
        if(use_uvmap)
        {
            mesh_cube->build (
                              {
                                  // bottom
                                  { -cube_size_h.x,-cube_size_h.y,-cube_size_h.z },
                                  { cube_size_h.x,-cube_size_h.y,-cube_size_h.z  },
                                  { -cube_size_h.x,-cube_size_h.y, cube_size_h.z },
                                  { cube_size_h.x,-cube_size_h.y,-cube_size_h.z  },
                                  { cube_size_h.x,-cube_size_h.y, cube_size_h.z  },
                                  { -cube_size_h.x,-cube_size_h.y, cube_size_h.z },
                                  
                                  // top
                                  {-cube_size_h.x, cube_size_h.y,-cube_size_h.z },
                                  {-cube_size_h.x, cube_size_h.y, cube_size_h.z },
                                  { cube_size_h.x, cube_size_h.y,-cube_size_h.z },
                                  { cube_size_h.x, cube_size_h.y,-cube_size_h.z },
                                  {-cube_size_h.x, cube_size_h.y, cube_size_h.z },
                                  { cube_size_h.x, cube_size_h.y, cube_size_h.z },
                                  
                                  // front
                                  { -cube_size_h.x,-cube_size_h.y, cube_size_h.z},
                                  {  cube_size_h.x,-cube_size_h.y, cube_size_h.z},
                                  { -cube_size_h.x, cube_size_h.y, cube_size_h.z},
                                  {  cube_size_h.x,-cube_size_h.y, cube_size_h.z},
                                  {  cube_size_h.x, cube_size_h.y, cube_size_h.z},
                                  { -cube_size_h.x, cube_size_h.y, cube_size_h.z},
                                  
                                  // back
                                  { -cube_size_h.x,-cube_size_h.y,-cube_size_h.z},
                                  { -cube_size_h.x, cube_size_h.y,-cube_size_h.z},
                                  {  cube_size_h.x,-cube_size_h.y,-cube_size_h.z},
                                  {  cube_size_h.x,-cube_size_h.y,-cube_size_h.z},
                                  { -cube_size_h.x, cube_size_h.y,-cube_size_h.z},
                                  {  cube_size_h.x, cube_size_h.y,-cube_size_h.z},
                                  
                                  // left
                                  { -cube_size_h.x,-cube_size_h.y, cube_size_h.z},
                                  { -cube_size_h.x, cube_size_h.y,-cube_size_h.z},
                                  { -cube_size_h.x,-cube_size_h.y,-cube_size_h.z},
                                  { -cube_size_h.x,-cube_size_h.y, cube_size_h.z},
                                  { -cube_size_h.x, cube_size_h.y, cube_size_h.z},
                                  { -cube_size_h.x, cube_size_h.y,-cube_size_h.z},
                                  
                                  // right
                                  {  cube_size_h.x,-cube_size_h.y, cube_size_h.z},
                                  {  cube_size_h.x,-cube_size_h.y,-cube_size_h.z},
                                  {  cube_size_h.x, cube_size_h.y,-cube_size_h.z},
                                  {  cube_size_h.x,-cube_size_h.y, cube_size_h.z},
                                  {  cube_size_h.x, cube_size_h.y,-cube_size_h.z},
                                  {  cube_size_h.x, cube_size_h.y, cube_size_h.z},
                              },
                            {
                                //  U     V
                                // bottom
                                { 0.0f, 0.0f },
                                { 1.0f, 0.0f },
                                { 0.0f, 1.0f },
                                { 1.0f, 0.0f },
                                { 1.0f, 1.0f },
                                { 0.0f, 1.0f },
                                                  
                                // top
                                { 0.0f, 0.0f},
                                { 0.0f, 1.0f},
                                { 1.0f, 0.0f},
                                { 1.0f, 0.0f},
                                { 0.0f, 1.0f},
                                { 1.0f, 1.0f},
                                                  
                                // front
                                { 1.0f, 0.0f},
                                { 0.0f, 0.0f},
                                { 1.0f, 1.0f},
                                { 0.0f, 0.0f},
                                { 0.0f, 1.0f},
                                { 1.0f, 1.0f},
                                                  
                                // back
                                { 0.0f, 0.0f},
                                { 0.0f, 1.0f},
                                { 1.0f, 0.0f},
                                { 1.0f, 0.0f},
                                { 0.0f, 1.0f},
                                { 1.0f, 1.0f},
                                                  
                                // left
                                { 0.0f, 1.0f},
                                { 1.0f, 0.0f},
                                { 0.0f, 0.0f},
                                { 0.0f, 1.0f},
                                { 1.0f, 1.0f},
                                { 1.0f, 0.0f},
                                                  
                                // right
                                { 1.0f, 1.0f},
                                { 1.0f, 0.0f},
                                { 0.0f, 0.0f},
                                { 1.0f, 1.0f},
                                { 0.0f, 0.0f},
                                { 0.0f, 1.0f}
                            });
        }
        else
        {
            mesh_cube->build (
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
                              });
        }
		//triangles
		mesh_cube->set_draw_info({ GL_TRIANGLES });
		//return cube
		return mesh_cube;
	}
}