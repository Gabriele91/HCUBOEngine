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
                glm::vec3 m_tangent;
                glm::vec3 m_bitangent;
                
                vertex(const glm::vec3& in_vertex,
                       const glm::vec3& in_normal,
                       const glm::vec2& in_uvmap)
                {
                    m_vertex = in_vertex;
                    m_normal = in_normal;
                    m_uvmap  = in_uvmap;
                }
                
                vertex(const glm::vec3& in_vertex,
                       const glm::vec3& in_normal,
                       const glm::vec2& in_uvmap,
                       const glm::vec3& in_tangent,
                       const glm::vec3& in_bitangent)
                {
                    m_vertex    = in_vertex;
                    m_normal    = in_normal;
                    m_uvmap     = in_uvmap;
                    m_tangent   = in_tangent;
                    m_bitangent = in_bitangent;
                }
            };
            
            layout = mesh::mesh_layout
            {
                mesh::input_layout
                {
                    mesh::input{ 0, sizeof(vertex), 3, offsetof(vertex, m_vertex)   },
                    mesh::input{ 1, sizeof(vertex), 3, offsetof(vertex, m_normal)   },
                    mesh::input{ 2, sizeof(vertex), 2, offsetof(vertex, m_uvmap)    },
                    mesh::input{ 3, sizeof(vertex), 3, offsetof(vertex, m_tangent)  },
                    mesh::input{ 4, sizeof(vertex), 3, offsetof(vertex, m_bitangent)}
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
            
            //compute tangent per vertex
            for (unsigned int i=0; i < vertices.size(); i+=3)
            {
                
                // Shortcuts for vertices
                glm::vec3 & v0 = vertices[i+0].m_vertex;
                glm::vec3 & v1 = vertices[i+1].m_vertex;
                glm::vec3 & v2 = vertices[i+2].m_vertex;
                
                // Shortcuts for UVs
                glm::vec2 & uv0 = vertices[i+0].m_uvmap;
                glm::vec2 & uv1 = vertices[i+1].m_uvmap;
                glm::vec2 & uv2 = vertices[i+2].m_uvmap;
                
                // Edges of the triangle : postion delta
                glm::vec3 edge1 = v1-v0;
                glm::vec3 edge2 = v2-v0;
                
                // UV delta
                glm::vec2 delta_uv1 = uv1-uv0;
                glm::vec2 delta_uv2 = uv2-uv0;
                
                //compute tanget & bitangent
                float r = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);
                glm::vec3 tangent   = (edge1 * delta_uv2.y  - edge2 * delta_uv1.y)*r;
                glm::vec3 bitangent = (edge2 * delta_uv1.x  - edge1 * delta_uv2.x)*r;
                
                //add tangent
                vertices[i+0].m_tangent =
                vertices[i+1].m_tangent =
                vertices[i+2].m_tangent = glm::normalize(tangent);
                
                //add bitangent
                vertices[i+0].m_bitangent =
                vertices[i+1].m_bitangent =
                vertices[i+2].m_bitangent = glm::normalize(bitangent);
                
            }
            
            // See "Going Further"
            for (unsigned int i=0; i<vertices.size(); ++i )
            {
                glm::vec3& n = vertices[i].m_normal;
                glm::vec3& t = vertices[i].m_tangent;
                glm::vec3& b = vertices[i].m_bitangent;
                
                // Gram-Schmidt orthogonalize
                t = glm::normalize(t - n * glm::dot(n, t));
                
                // Calculate handedness
                if (glm::dot(glm::cross(n, t), b) < 0.0f)
                {
                    t = t * -1.0f;
                }
                
            }
            
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