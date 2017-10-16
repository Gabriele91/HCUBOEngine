#include <hcube/math/vector_math.h>
#include <hcube/math/tangent_space_calculation.h>
#include <hcube/utilities/basic_meshs.h>

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
		mesh::ptr sphere(const float sphere_radius, const bool use_uvmap)
		{
			return sphere(sphere_radius, 100, 50, use_uvmap);
		}
		mesh::ptr sphere(const float sphere_radius, const int rings, const int sectors, const bool use_uvmap)
		{
			//cube
			mesh::ptr mesh_sphere = mesh::snew();
			//data
			mesh::mesh_layout  layout;
			//if uv
			if (use_uvmap)
			{
				struct vertex
				{
					vec3 m_position;
					vec3 m_normal;
					vec2 m_uvmap;
					vec3 m_tangent;
					vec3 m_bitangent;
				};

				layout = mesh::mesh_layout
				{
					render::create_IL({
					sizeof(vertex),
					{
						attribute{ ATT_POSITIONT, AST_FLOAT3, offsetof(vertex, m_position) },
						attribute{ ATT_NORMAL0, AST_FLOAT3,   offsetof(vertex, m_normal) },
						attribute{ ATT_TEXCOORD0, AST_FLOAT2, offsetof(vertex, m_uvmap) },
						attribute{ ATT_TANGENT0, AST_FLOAT3,  offsetof(vertex, m_tangent) },
						attribute{ ATT_BINORMAL0, AST_FLOAT3, offsetof(vertex, m_bitangent) }
					}
					}),
					DRAW_TRIANGLES
				};
				//set vertices
				const size_t vertices_size = rings*sectors;
				const size_t indices_size = (rings - 1)*(sectors - 1) * 6;
				float const R = 1.0f / (float)(rings - 1);
				float const S = 1.0f / (float)(sectors - 1);
				float const RADIUS = sphere_radius;
				int idv = 0;
				//alloc
				std::vector< vertex >       vertices(vertices_size);
				std::vector< unsigned int > indices(indices_size);
				//set vertex
				for (int r = 0; r<rings; ++r)
				{
					for (int s = 0; s<sectors; ++s)
					{
						float const y = sin(-0.5*constants::pi<float>() + constants::pi<float>() * r * R);
						float const x = cos(2 * constants::pi<float>() * s * S) * sin(constants::pi<float>() * r * R);
						float const z = sin(2 * constants::pi<float>() * s * S) * sin(constants::pi<float>() * r * R);
						/* calc vertex */
						vertices[idv].m_position.x = -x * RADIUS;
						vertices[idv].m_position.y = -y * RADIUS;
						vertices[idv].m_position.z = -z * RADIUS;
						/* calc normal */
						vertices[idv].m_normal.x = -x;
						vertices[idv].m_normal.y = -y;
						vertices[idv].m_normal.z = -z;
						/* calc uv */
						vertices[idv].m_uvmap.x = s*S;
						vertices[idv].m_uvmap.y = 1.0f - r*R;
						//
						++idv;
					}
				}
				//set indices
				int i = -1;
				for (int r = 0; r < rings - 1; r++)
				{
					for (int s = 0; s < sectors - 1; s++)
					{
						//1 2 3
						indices[++i] = r * sectors + s;
						indices[++i] = r * sectors + (s + 1);
						indices[++i] = (r + 1) * sectors + (s + 1);
						//1 3 4
						indices[++i] = r * sectors + s;
						indices[++i] = (r + 1) * sectors + (s + 1);
						indices[++i] = (r + 1) * sectors + s;
					}
				}
				//compute tangent per vertex
				tangent_space_calculation::compute_tangent_fast<vertex>(indices, vertices);
				//build mesh
				mesh_sphere->build(
					layout,
					indices.data(),
					indices.size(),
					(const mesh::byte*)vertices.data(),
					vertices.size() * sizeof(vertex)
				);
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
						attribute{ ATT_NORMAL0, AST_FLOAT3,   offsetof(vertex, m_normal) }
					}
					}),
					DRAW_TRIANGLES
				};
				//set vertices
				const size_t vertices_size = rings*sectors;
				const size_t indices_size = (rings - 1)*(sectors - 1) * 6;
				float const R = 1.0f / (float)(rings - 1);
				float const S = 1.0f / (float)(sectors - 1);
				float const RADIUS = sphere_radius;
				int idv = 0;
				//alloc
				std::vector< vertex >       vertices(vertices_size);
				std::vector< unsigned int > indices(indices_size);
				//set vertex
				for (int r = 0; r<rings; ++r)
				{
					for (int s = 0; s<sectors; ++s)
					{
						float const y = sin(-0.5*constants::pi<float>() + constants::pi<float>() * r * R);
						float const x = cos(2 * constants::pi<float>() * s * S) * sin(constants::pi<float>() * r * R);
						float const z = sin(2 * constants::pi<float>() * s * S) * sin(constants::pi<float>() * r * R);
						/* calc vertex */
						vertices[idv].m_position.x = x * RADIUS;
						vertices[idv].m_position.y = y * RADIUS;
						vertices[idv].m_position.z = z * RADIUS;
						/* calc normal */
						vertices[idv].m_normal.x = x;
						vertices[idv].m_normal.y = y;
						vertices[idv].m_normal.z = z;
						//
						++idv;
					}
				}
				//set indices
				int i = -1;
				for (int r = 0; r < rings - 1; r++)
				{
					for (int s = 0; s < sectors - 1; s++)
					{
						//1 2 3
						indices[++i] = r * sectors + s;
						indices[++i] = r * sectors + (s + 1);
						indices[++i] = (r + 1) * sectors + (s + 1);
						//1 3 4
						indices[++i] = r * sectors + s;
						indices[++i] = (r + 1) * sectors + (s + 1);
						indices[++i] = (r + 1) * sectors + s;
					}
				}
				//build mesh
				mesh_sphere->build(
					layout,
					indices.data(),
					indices.size(),
					(const mesh::byte*)vertices.data(),
					vertices.size() * sizeof(vertex)
				);
			}

			return mesh_sphere;
		}
		
		//helper class
		template < class VERTEX >
		struct icosphere_helper
		{
			float						  m_radius;
			std::vector < VERTEX >		  m_vecs;
			std::vector < unsigned int >  m_idxs;

			icosphere_helper(float radius) : m_radius(radius)
			{
				//const t fector (icosphere)
				const float t = (1.0f + sqrt(5.0f)) / 2.0f;

				//add vertex
				add_vertex(vec3(-1, t, 0));
				add_vertex(vec3(1, t, 0));
				add_vertex(vec3(-1, -t, 0));
				add_vertex(vec3(1, -t, 0));

				add_vertex(vec3(0, -1, t));
				add_vertex(vec3(0, 1, t));
				add_vertex(vec3(0, -1, -t));
				add_vertex(vec3(0, 1, -t));

				add_vertex(vec3(t, 0, -1));
				add_vertex(vec3(t, 0, 1));
				add_vertex(vec3(-t, 0, -1));
				add_vertex(vec3(-t, 0, 1));

				//add indexs
				add_index3(0, 11, 5);
				add_index3(0, 5, 1);
				add_index3(0, 1, 7);
				add_index3(0, 7, 10);
				add_index3(0, 10, 11);

				add_index3(1, 5, 9);
				add_index3(5, 11, 4);
				add_index3(11, 10, 2);
				add_index3(10, 7, 6);
				add_index3(7, 1, 8);

				add_index3(3, 9, 4);
				add_index3(3, 4, 2);
				add_index3(3, 2, 6);
				add_index3(3, 6, 8);
				add_index3(3, 8, 9);

				add_index3(4, 9, 5);
				add_index3(2, 4, 11);
				add_index3(6, 2, 10);
				add_index3(8, 6, 7);
				add_index3(9, 8, 1);

			}

			inline void add_vertex(const vec3& in_v)
			{
				auto norm_v = normalize(in_v);
				m_vecs.push_back({ norm_v * m_radius, norm_v });
			}

			inline void add_index3(unsigned int i0, unsigned int i1, unsigned int i2)
			{
				m_idxs.push_back(i0);
				m_idxs.push_back(i1);
				m_idxs.push_back(i2);
			}

			void tesselate(unsigned int tesselation_level)
			{
				//create
				std::vector< unsigned int > replacement_idxs;
				//for all levels
				for (unsigned int l = 0; l != tesselation_level; ++l)
				{
					//alloc
					replacement_idxs.clear();
					replacement_idxs.reserve(m_idxs.size()*4);
					//count of tris
					unsigned int n_tris = (unsigned int)(m_idxs.size() / 3);
					//compute
					for (unsigned int j = 0; j != n_tris; j++)
					{
						unsigned int a = add_mid_point_and_get_index(m_idxs[j * 3 + 0], m_idxs[j * 3 + 1]);
						unsigned int b = add_mid_point_and_get_index(m_idxs[j * 3 + 1], m_idxs[j * 3 + 2]);
						unsigned int c = add_mid_point_and_get_index(m_idxs[j * 3 + 2], m_idxs[j * 3 + 0]);
						
						//1 tri x a c
						replacement_idxs.push_back(m_idxs[j * 3 + 0]);
						replacement_idxs.push_back(a);
						replacement_idxs.push_back(c);

						//2 tri a y b
						replacement_idxs.push_back(a);
						replacement_idxs.push_back(m_idxs[j * 3 + 1]);
						replacement_idxs.push_back(b);

						//3 tri c b z
						replacement_idxs.push_back(c);
						replacement_idxs.push_back(b);
						replacement_idxs.push_back(m_idxs[j * 3 + 2]);

						//4 tri c a b
						replacement_idxs.push_back(c);
						replacement_idxs.push_back(a);
						replacement_idxs.push_back(b);
					}
					//add new level
					m_idxs = replacement_idxs;
				}
			}

			unsigned int add_mid_point_and_get_index(const unsigned int a, const unsigned int b)
			{
				auto mid_point = (m_vecs[a].m_position + m_vecs[b].m_position) / 2.0f;
				unsigned int i = m_vecs.size();
				add_vertex(mid_point);
				return i;
			}

			static vec2 get_latitude_longitude_UV(const vec3& position)
			{
				vec2 uv;
				vec3 norm_position = normalize(position);
				uv.x = 0.5f + std::atan2(norm_position.z, norm_position.x) / (float)(2.0f*constants::pi<float>());
				uv.y = 0.5f + norm_position.y * 0.5;
				return uv;
			}

			void compute_uvmap()
			{
				for (auto& v : m_vecs)
				{
					v.m_uvmap = get_latitude_longitude_UV(v.m_position);
				}
			}

			void fix_uvmap()
			{
				//count of tris
				unsigned int n_tris = (unsigned int)(m_idxs.size() / 3);
				const float factor_to_xto1 = 0.75;
				const float factor_to_xto0 = 0.25;
				//for all tris
				for (unsigned int j = 0; j != n_tris; j++)
				{
					//vector
					auto a = m_vecs[m_idxs[j * 3 + 0]];
					auto b = m_vecs[m_idxs[j * 3 + 1]];
					auto c = m_vecs[m_idxs[j * 3 + 2]];
					//test  f1 < v.x < 1.	
					bool a_more = a.m_uvmap.x > factor_to_xto1;
					bool b_more = b.m_uvmap.x > factor_to_xto1;
					bool c_more = c.m_uvmap.x > factor_to_xto1;
					int  abc_more = (a_more + b_more + c_more);
					//bad case
					if (!abc_more || abc_more == 3) continue;

					//test  0. < v.x < f0	
					if (a.m_uvmap.x < factor_to_xto0)
					{
						m_idxs[j * 3 + 0] = (unsigned int)(m_vecs.size());
						m_vecs.push_back(a);
						m_vecs[m_idxs[j * 3 + 0]].m_uvmap.x += 1.0;
					}

					if (b.m_uvmap.x < factor_to_xto0)
					{
						m_idxs[j * 3 + 1] = (unsigned int)(m_vecs.size());
						m_vecs.push_back(b);
						m_vecs[m_idxs[j * 3 + 1]].m_uvmap.x += 1.0;
					}

					if (c.m_uvmap.x < factor_to_xto0)
					{
						m_idxs[j * 3 + 2] = (unsigned int)(m_vecs.size());
						m_vecs.push_back(c);
						m_vecs[m_idxs[j * 3 + 2]].m_uvmap.x += 1.0;
					}		
				}
			}

			void compute_tangent()
			{
				//compute tangent per vertex
				tangent_space_calculation::compute_tangent_fast<VERTEX>(m_idxs,m_vecs);
			}
		};

		mesh::ptr icosphere(const float sphere_radius, const bool use_uvmap)
		{
			return icosphere(sphere_radius, 3, use_uvmap);
		}

		mesh::ptr icosphere(const float sphere_radius, const int lod, const bool use_uvmap)
		{
			//cube
			mesh::ptr mesh_icosphere = mesh::snew();
			//data
			mesh::mesh_layout  layout;
			//uv
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
						   const vec3& in_normal)
					{
						m_position = in_vertex;
						m_normal = in_normal;
					}

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
						attribute{ ATT_NORMAL0, AST_FLOAT3,   offsetof(vertex, m_normal) },
						attribute{ ATT_TEXCOORD0, AST_FLOAT2, offsetof(vertex, m_uvmap) },
						attribute{ ATT_TANGENT0, AST_FLOAT3,  offsetof(vertex, m_tangent) },
						attribute{ ATT_BINORMAL0, AST_FLOAT3, offsetof(vertex, m_bitangent) }
					}
					}),
					DRAW_TRIANGLES
				};
				//init geometry
				icosphere_helper<vertex> is_helper(sphere_radius);
				//LOD
				if(lod) is_helper.tesselate(lod);
				//compute uv
				is_helper.compute_uvmap();
				//compute tangent per vertex
				is_helper.compute_tangent();
				//fix edge (the vertex near 0.0 point now point to 1.0+offset vertex)
				is_helper.fix_uvmap();
				//build mesh
				mesh_icosphere->build(
					layout,
					is_helper.m_idxs.data(),
					is_helper.m_idxs.size(),
					(const mesh::byte*)is_helper.m_vecs.data(),
					is_helper.m_vecs.size() * sizeof(vertex)
				);
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
						attribute{ ATT_NORMAL0, AST_FLOAT3,   offsetof(vertex, m_normal) }
					}
					}),
					DRAW_TRIANGLES
				};

				//init geometry
				icosphere_helper<vertex> is_helper(sphere_radius);
				//LOD
				if (lod) is_helper.tesselate(lod);
				//build mesh
				mesh_icosphere->build(
					layout,
					is_helper.m_idxs.data(),
					is_helper.m_idxs.size(),
					(const mesh::byte*)is_helper.m_vecs.data(),
					is_helper.m_vecs.size() * sizeof(vertex)
				);
			}

			return mesh_icosphere;
		}
	}
}
