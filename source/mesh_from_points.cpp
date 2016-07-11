//
//  mesh_from_points.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <point.h>
#include <mesh_from_points.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#if 0
bool mesh_from_h_cubo_points(input_file_points& input,
                             meshs_h_cubo_list& output,
                             bool recompute_electrons_pos)
{
    //only if is open
    if(input.is_open())
    {
        //vertex for meshs
        std::vector < glm::vec3 > mesh_vertex_protons;
        std::vector < glm::vec3 > mesh_vertex_electrons;
        //points buffers
        points_t protons;
        points_t electrons;
        //loop read
        while(input.remaning())
        {
            unsigned int iteration{ 0 };
            unsigned int size     { 0 };
            //resize
            protons.resize(0);
            electrons.resize(0);
            //read
            input.read(iteration,
                       protons,
                       electrons,
                       size);
            //reset
            mesh_vertex_protons.resize(0);
            mesh_vertex_electrons.resize(0);
            //pre alloc
            mesh_vertex_protons.reserve(size);
            mesh_vertex_electrons.reserve(size);
            //convert points into vector< vec3 >
            for(point_t& proton : protons) mesh_vertex_protons.push_back({ proton.x,proton.y,proton.z });
            for(point_t& electron : electrons) mesh_vertex_electrons.push_back({ electron.x,electron.y,electron.z });
            //relative to world
            if(recompute_electrons_pos)
                for(size_t i = 0; i!=size; ++i) mesh_vertex_electrons[i] += mesh_vertex_protons[i];
            //build mesh
            output.resize(output.size()+1);
            output.back().m_iteration = iteration;
            output.back().m_size = size;
            output.back().m_protons = mesh::snew();
            output.back().m_protons->set_draw_info({ GL_POINTS });
            output.back().m_protons->build(mesh_vertex_protons);
            output.back().m_electrons = mesh::snew();
            output.back().m_electrons->set_draw_info({ GL_POINTS });
            output.back().m_electrons->build(mesh_vertex_electrons);
        }
        return true;
    }
    return false;
}

bool mesh_from_h_cubo_points(input_file_points& input,
                             meshs_h_cubo_list& output,
                             const glm::vec3& camera_pos,
                             bool recompute_electrons_pos)
{
    //only if is open
    if(input.is_open())
    {
        auto sort_function =
        [camera_pos]
        (const glm::vec3& left,const glm::vec3& right) -> bool
        {
            return glm::distance(left,camera_pos) < glm::distance(right,camera_pos);
        };
        //vertex for meshs
        std::vector < glm::vec3 > mesh_vertex_protons;
        std::vector < glm::vec3 > mesh_vertex_electrons;
        //points buffers
        points_t protons;
        points_t electrons;
        //loop read
        while(input.remaning())
        {
            unsigned int iteration{ 0 };
            unsigned int size     { 0 };
            //resize
            protons.resize(0);
            electrons.resize(0);
            //read
            input.read(iteration,
                       protons,
                       electrons,
                       size);
            //reset
            mesh_vertex_protons.resize(0);
            mesh_vertex_electrons.resize(0);
            //pre alloc
            mesh_vertex_protons.reserve(size);
            mesh_vertex_electrons.reserve(size);
            //convert points into vector< vec3 >
            for(point_t& proton : protons) mesh_vertex_protons.push_back({ proton.x,proton.y,proton.z });
            for(point_t& electron : electrons) mesh_vertex_electrons.push_back({ electron.x,electron.y,electron.z });
            //relative to world
            if(recompute_electrons_pos)
                for(size_t i = 0; i!=size; ++i) mesh_vertex_electrons[i] += mesh_vertex_protons[i];
            //sort
            std::sort(mesh_vertex_protons.begin(),mesh_vertex_protons.end(), sort_function);
            std::sort(mesh_vertex_electrons.begin(),mesh_vertex_electrons.end(), sort_function);
            //build mesh
            output.resize(output.size()+1);
            output.back().m_iteration = iteration;
            output.back().m_size = size;
            output.back().m_protons = mesh::snew();
            output.back().m_protons->set_draw_info({ GL_POINTS });
            output.back().m_protons->build(mesh_vertex_protons);
            output.back().m_electrons = mesh::snew();
            output.back().m_electrons->set_draw_info({ GL_POINTS });
            output.back().m_electrons->build(mesh_vertex_electrons);
        }
        return true;
    }
    return false;
}
#endif