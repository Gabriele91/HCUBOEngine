#include <basic_meshs.h>
#include <glm/glm.hpp>

namespace basic_meshs
{
	mesh::ptr cube(const glm::vec3& cube_size,bool use_uvmap)
	{
		const glm::vec3& cube_size_h = cube_size * 0.5f;
		//cube
		mesh::ptr mesh_cube = mesh::snew();
        //data
        mesh::mesh_layout  layout;
        //buffer
        std::vector< mesh::byte > vbuffer;
        std::vector< unsigned int > ibuffer;
		//build
        if(use_uvmap)
        {
            struct vertex
            {
                glm::vec3 m_vertex;
                glm::vec3 m_normal;
                glm::vec2 m_uvmap;
            };
            
            layout = mesh::mesh_layout
            {
                mesh::input_layout
                {
                    mesh::input{ 0, sizeof(vertex), 3, offsetof(vertex, m_vertex) },
                    mesh::input{ 1, sizeof(vertex), 3, offsetof(vertex, m_normal) },
                    mesh::input{ 2, sizeof(vertex), 2, offsetof(vertex, m_uvmap)  }
                },
                GL_TRIANGLES,
                GL_STATIC_DRAW
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
            //resize
            vbuffer.resize(vertices.size()*sizeof(vertex));
            //copy buffer
            std::memcpy(&vbuffer[0], vertices.data(), vertices.size()*sizeof(vertex));
        }
        else
        {
            
            struct vertex
            {
                glm::vec3 m_vertex;
                glm::vec3 m_normal;
            };
            
            layout = mesh::mesh_layout
            {
                mesh::input_layout
                {
                    mesh::input{ 0, sizeof(vertex), 3, offsetof(vertex, m_vertex) },
                    mesh::input{ 1, sizeof(vertex), 3, offsetof(vertex, m_normal) }
                },
                GL_TRIANGLES,
                GL_STATIC_DRAW
            };
            
            ibuffer = std::vector< unsigned int >
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
            for(vertex& v : vertices)
            {
                v.m_normal = glm::normalize(v.m_vertex);
            }
            
            //resize
            vbuffer.resize(vertices.size()*sizeof(vertex));
            //copy buffer
            std::memcpy(&vbuffer[0], vertices.data(), vertices.size()*sizeof(vertex));
        }
        //build gpu cube
        if(ibuffer.size()) mesh_cube->build(layout, ibuffer, vbuffer);
        else               mesh_cube->build(layout,vbuffer);
		//return cube
		return mesh_cube;
	}
	mesh::ptr square3D(const glm::vec2& square_size, bool use_uvmap)
	{
		const glm::vec2& square_size_h = square_size * 0.5f;
		//cube
		mesh::ptr mesh_square = mesh::snew();
		//data
		mesh::mesh_layout  layout;
		//buffer
		std::vector< mesh::byte > vbuffer;
		//build
		if (use_uvmap)
		{
			struct vertex
			{
				glm::vec3 m_vertex;
				glm::vec3 m_normal;
				glm::vec2 m_uvmap;
			};

			layout = mesh::mesh_layout
			{
				mesh::input_layout
				{
					mesh::input{ 0, sizeof(vertex), 3, offsetof(vertex, m_vertex) },
					mesh::input{ 1, sizeof(vertex), 3, offsetof(vertex, m_normal) },
					mesh::input{ 2, sizeof(vertex), 2, offsetof(vertex, m_uvmap) }
				},
				GL_TRIANGLE_STRIP,
				GL_STATIC_DRAW
			};

			std::vector< vertex > vertices
			{
				{{-square_size_h.x,  square_size_h.y, 0.0f  },{ 0,0,1.0 }, {0.0f, 1.0f } },
				{{-square_size_h.x, -square_size_h.y, 0.0f  },{ 0,0,1.0 }, {0.0f, 0.0f } },
				{{ square_size_h.x,  square_size_h.y, 0.0f  },{ 0,0,1.0 }, {1.0f, 1.0f } },
				{{ square_size_h.x, -square_size_h.y, 0.0f  },{ 0,0,1.0 }, {1.0f, 0.0f } }
			};
			//resize
			vbuffer.resize(vertices.size() * sizeof(vertex));
			//copy buffer
			std::memcpy(&vbuffer[0], vertices.data(), vertices.size() * sizeof(vertex));
		}
		else
		{

			struct vertex
			{
				glm::vec3 m_vertex;
				glm::vec3 m_normal;
			};

			layout = mesh::mesh_layout
			{
				mesh::input_layout
				{
					mesh::input{ 0, sizeof(vertex), 3, offsetof(vertex, m_vertex) },
					mesh::input{ 1, sizeof(vertex), 3, offsetof(vertex, m_normal) }
				},
				GL_TRIANGLE_STRIP,
				GL_STATIC_DRAW
			};

			std::vector< vertex > vertices
			{
				{ { -square_size_h.x,  square_size_h.y, 0.0f },{ 0,0,1.0 } },
				{ { -square_size_h.x, -square_size_h.y, 0.0f },{ 0,0,1.0 } },
				{ {  square_size_h.x,  square_size_h.y, 0.0f },{ 0,0,1.0 } },
				{ {  square_size_h.x, -square_size_h.y, 0.0f },{ 0,0,1.0 } }
			};
			
			//resize
			vbuffer.resize(vertices.size() * sizeof(vertex));
			//copy buffer
			std::memcpy(&vbuffer[0], vertices.data(), vertices.size() * sizeof(vertex));
		}
		//build gpu quad
		mesh_square->build(layout, vbuffer);
		//return cube
		return mesh_square;



	}
}