#include <hcube/math/vector_math.h>
#include <hcube/math/tangent_space_calculation.h>
#include <hcube/components/basic_meshs.h>

namespace hcube
{
	namespace basic_meshs
	{
		mesh::ptr cube(const vec3& cube_size, bool use_uvmap)
		{
			const vec3& cube_size_h = cube_size * 0.5f;
			//cube
			mesh::ptr mesh_cube = mesh::snew();
			//data
			mesh::mesh_layout  layout;
			mesh::draw_range   range;
			//build
			if (use_uvmap)
			{
				struct vertex
				{
					vec3 m_position;
					vec3 m_normal;
					vec2 m_uvmap;
					vec3 m_tangent;
					vec3 m_bitangent;

					vertex(const vec3& in_vertex,
						const vec3& in_normal,
						const vec2& in_uvmap)
					{
						m_position = in_vertex;
						m_normal = in_normal;
						m_uvmap = in_uvmap;
					}

					vertex(const vec3& in_vertex,
						const vec3& in_normal,
						const vec2& in_uvmap,
						const vec3& in_tangent,
						const vec3& in_bitangent)
					{
						m_position = in_vertex;
						m_normal = in_normal;
						m_uvmap = in_uvmap;
						m_tangent = in_tangent;
						m_bitangent = in_bitangent;
					}
				};

				layout = mesh::mesh_layout
				{
					render::create_IL({
										sizeof(vertex),
										{
											 attribute{ ATT_POSITIONT, AST_FLOAT3, offsetof(vertex, m_position) },
											 attribute{ ATT_NORMAL0, AST_FLOAT3,   offsetof(vertex, m_normal)   },
											 attribute{ ATT_TEXCOORD0, AST_FLOAT2, offsetof(vertex, m_uvmap)    },
											 attribute{ ATT_TANGENT0, AST_FLOAT3,  offsetof(vertex, m_tangent)  },
											 attribute{ ATT_BINORMAL0, AST_FLOAT3, offsetof(vertex, m_bitangent)}
										}
									  }),
					DRAW_TRIANGLES
				};

				std::vector< vertex > vertices
				{
					// bottom
					vertex{ { -cube_size_h.x,-cube_size_h.y,-cube_size_h.z }, {0,-1.0,0}, { 0.0f, 0.0f } },
					vertex{ { cube_size_h.x,-cube_size_h.y,-cube_size_h.z  }, {0,-1.0,0}, { 1.0f, 0.0f } },
					vertex{ { -cube_size_h.x,-cube_size_h.y, cube_size_h.z }, {0,-1.0,0}, { 0.0f, 1.0f } },
					vertex{ { cube_size_h.x,-cube_size_h.y,-cube_size_h.z  }, {0,-1.0,0}, { 1.0f, 0.0f } },
					vertex{ { cube_size_h.x,-cube_size_h.y, cube_size_h.z  }, {0,-1.0,0}, { 1.0f, 1.0f } },
					vertex{ { -cube_size_h.x,-cube_size_h.y, cube_size_h.z }, {0,-1.0,0}, { 0.0f, 1.0f } },

					// top
					vertex{ {-cube_size_h.x, cube_size_h.y,-cube_size_h.z }, {0,1.0,0}, { 0.0f, 0.0f} },
					vertex{ {-cube_size_h.x, cube_size_h.y, cube_size_h.z }, {0,1.0,0}, { 0.0f, 1.0f} },
					vertex{ { cube_size_h.x, cube_size_h.y,-cube_size_h.z }, {0,1.0,0}, { 1.0f, 0.0f} },
					vertex{ { cube_size_h.x, cube_size_h.y,-cube_size_h.z }, {0,1.0,0}, { 1.0f, 0.0f} },
					vertex{ {-cube_size_h.x, cube_size_h.y, cube_size_h.z }, {0,1.0,0}, { 0.0f, 1.0f} },
					vertex{ { cube_size_h.x, cube_size_h.y, cube_size_h.z }, {0,1.0,0}, { 1.0f, 1.0f} },

					// front
					vertex{ { -cube_size_h.x,-cube_size_h.y, cube_size_h.z}, {0,0,1.0}, { 1.0f, 0.0f} },
					vertex{ {  cube_size_h.x,-cube_size_h.y, cube_size_h.z}, {0,0,1.0}, { 0.0f, 0.0f} },
					vertex{ { -cube_size_h.x, cube_size_h.y, cube_size_h.z}, {0,0,1.0}, { 1.0f, 1.0f} },
					vertex{ {  cube_size_h.x,-cube_size_h.y, cube_size_h.z}, {0,0,1.0}, { 0.0f, 0.0f} },
					vertex{ {  cube_size_h.x, cube_size_h.y, cube_size_h.z}, {0,0,1.0}, { 0.0f, 1.0f} },
					vertex{ { -cube_size_h.x, cube_size_h.y, cube_size_h.z}, {0,0,1.0}, { 1.0f, 1.0f} },

					// back
					vertex{ { -cube_size_h.x,-cube_size_h.y,-cube_size_h.z}, {0,0,-1.0}, { 0.0f, 0.0f} },
					vertex{ { -cube_size_h.x, cube_size_h.y,-cube_size_h.z}, {0,0,-1.0}, { 0.0f, 1.0f} },
					vertex{ {  cube_size_h.x,-cube_size_h.y,-cube_size_h.z}, {0,0,-1.0}, { 1.0f, 0.0f} },
					vertex{ {  cube_size_h.x,-cube_size_h.y,-cube_size_h.z}, {0,0,-1.0}, { 1.0f, 0.0f} },
					vertex{ { -cube_size_h.x, cube_size_h.y,-cube_size_h.z}, {0,0,-1.0}, { 0.0f, 1.0f} },
					vertex{ {  cube_size_h.x, cube_size_h.y,-cube_size_h.z}, {0,0,-1.0}, { 1.0f, 1.0f} },

					// left
					vertex{ { -cube_size_h.x,-cube_size_h.y, cube_size_h.z}, {-1.0,0,0}, { 0.0f, 1.0f} },
					vertex{ { -cube_size_h.x, cube_size_h.y,-cube_size_h.z}, {-1.0,0,0}, { 1.0f, 0.0f} },
					vertex{ { -cube_size_h.x,-cube_size_h.y,-cube_size_h.z}, {-1.0,0,0}, { 0.0f, 0.0f} },
					vertex{ { -cube_size_h.x,-cube_size_h.y, cube_size_h.z}, {-1.0,0,0}, { 0.0f, 1.0f} },
					vertex{ { -cube_size_h.x, cube_size_h.y, cube_size_h.z}, {-1.0,0,0}, { 1.0f, 1.0f} },
					vertex{ { -cube_size_h.x, cube_size_h.y,-cube_size_h.z}, {-1.0,0,0}, { 1.0f, 0.0f} },

					// right
					vertex{ {  cube_size_h.x,-cube_size_h.y, cube_size_h.z}, {1.0,0,0}, { 1.0f, 1.0f} },
					vertex{ {  cube_size_h.x,-cube_size_h.y,-cube_size_h.z}, {1.0,0,0}, { 1.0f, 0.0f} },
					vertex{ {  cube_size_h.x, cube_size_h.y,-cube_size_h.z}, {1.0,0,0}, { 0.0f, 0.0f} },
					vertex{ {  cube_size_h.x,-cube_size_h.y, cube_size_h.z}, {1.0,0,0}, { 1.0f, 1.0f} },
					vertex{ {  cube_size_h.x, cube_size_h.y,-cube_size_h.z}, {1.0,0,0}, { 0.0f, 0.0f} },
					vertex{ {  cube_size_h.x, cube_size_h.y, cube_size_h.z}, {1.0,0,0}, { 0.0f, 1.0f} },
				};

				//compute tangent per vertex
				tangent_space_calculation::compute_tangent_fast<vertex>(vertices);
				//set size
				range.m_max = (unsigned int)vertices.size();
				//resize
				//build mesh
				mesh_cube->build(layout, range,
					(const mesh::byte*)vertices.data(), vertices.size() * sizeof(vertex));
			}
			else
			{

				struct vertex
				{
					vec3 m_position;
					vec3 m_normal;
				};

				layout = mesh::mesh_layout
				{
					render::create_IL({
						sizeof(vertex),
						{
							attribute{ ATT_POSITIONT, AST_FLOAT3, offsetof(vertex, m_position) },
							attribute{ ATT_NORMAL0,   AST_FLOAT3, offsetof(vertex, m_normal)   }
						}
					}),
					DRAW_TRIANGLES
				};

				std::vector< unsigned int > ibuffer
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
				};

				std::vector< vertex > vertices
				{
					{ { -cube_size_h.x, -cube_size_h.y,   cube_size_h.z }, {0,0,0} },
					{ {  cube_size_h.x, -cube_size_h.y,   cube_size_h.z }, {0,0,0} },
					{ {  cube_size_h.x,  cube_size_h.y,   cube_size_h.z }, {0,0,0} },
					{ { -cube_size_h.x,  cube_size_h.y,   cube_size_h.z }, {0,0,0} },

					{ { -cube_size_h.x, -cube_size_h.y,  -cube_size_h.z }, {0,0,0} },
					{ {  cube_size_h.x, -cube_size_h.y,  -cube_size_h.z }, {0,0,0} },
					{ {  cube_size_h.x,  cube_size_h.y,  -cube_size_h.z }, {0,0,0} },
					{ { -cube_size_h.x,  cube_size_h.y,  -cube_size_h.z }, {0,0,0} },
				};

				//compute normals
				for (vertex& v : vertices)
				{
					v.m_normal = normalize(v.m_position);
				}
				//build mesh
				mesh_cube->build(layout,
					ibuffer.data(), ibuffer.size(),
					(const mesh::byte*)vertices.data(), vertices.size() * sizeof(vertex));
			}
			//set obb
			mesh_cube->set_bounding_box(obb(mat3(1), vec3(0, 0, 0), cube_size_h));
			//return cube
			return mesh_cube;
		}
		mesh::ptr square3D(const vec2& square_size, bool use_uvmap)
		{
			const vec2& square_size_h = square_size * 0.5f;
			//cube
			mesh::ptr mesh_square = mesh::snew();
			//data
			mesh::mesh_layout  layout;
			mesh::draw_range   range;
			//build
			if (use_uvmap)
			{
				struct vertex
				{
					vec3 m_position;
					vec3 m_normal;
					vec2 m_uvmap;
				};


				layout = mesh::mesh_layout
				{
					render::create_IL({
						sizeof(vertex),
						{
							attribute{ ATT_POSITIONT, AST_FLOAT3, offsetof(vertex, m_position) },
							attribute{ ATT_NORMAL0,   AST_FLOAT3, offsetof(vertex, m_normal)   },
							attribute{ ATT_TEXCOORD0, AST_FLOAT2, offsetof(vertex, m_uvmap)    }
						}
					}),
					DRAW_TRIANGLE_STRIP
				};

				std::vector< vertex > vertices
				{
					{{-square_size_h.x,  square_size_h.y, 0.0f  },{ 0,0,1.0 }, {0.0f, 1.0f } },
					{{ square_size_h.x,  square_size_h.y, 0.0f  },{ 0,0,1.0 },{ 1.0f, 1.0f } },
					{{-square_size_h.x, -square_size_h.y, 0.0f  },{ 0,0,1.0 }, {0.0f, 0.0f } },
					{{ square_size_h.x, -square_size_h.y, 0.0f  },{ 0,0,1.0 }, {1.0f, 0.0f } }
				};
				//set size
				range.m_max = (unsigned int)vertices.size();
				//build
				mesh_square->build(layout, range, (mesh::byte*)vertices.data(), vertices.size() * sizeof(vertex));
			}
			else
			{

				struct vertex
				{
					vec3 m_position;
					vec3 m_normal;
				};


				layout = mesh::mesh_layout
				{
					render::create_IL({
						sizeof(vertex),
						{
							attribute{ ATT_POSITIONT, AST_FLOAT3, offsetof(vertex, m_position) },
							attribute{ ATT_NORMAL0,   AST_FLOAT3, offsetof(vertex, m_normal)   }
						}
					}),
					DRAW_TRIANGLE_STRIP
				};

				std::vector< vertex > vertices
				{
					{ { -square_size_h.x,  square_size_h.y, 0.0f },{ 0,0,1.0 } },
					{ {  square_size_h.x,  square_size_h.y, 0.0f },{ 0,0,1.0 } },
					{ { -square_size_h.x, -square_size_h.y, 0.0f },{ 0,0,1.0 } },
					{ {  square_size_h.x, -square_size_h.y, 0.0f },{ 0,0,1.0 } }
				};
				//set size
				range.m_max = (unsigned int)vertices.size();
				//build
				mesh_square->build(
					layout, 
					range,
					(const mesh::byte*)vertices.data(), 
					vertices.size() * sizeof(vertex)
				);
			}
			//set obb
			mesh_square->set_bounding_box(obb(mat3(1), vec3(0, 0, 0), vec3(square_size_h,0.001f)));
			//return cube
			return mesh_square;



		}
	}
}