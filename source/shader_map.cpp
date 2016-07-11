//
//  shader_map.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <shader_map.h>


void shader_map::load_standard_shader()
{
    get_shader("base").load_shader("shader/base/base.vs.glsl",
                                   "shader/base/base.fs.glsl");
    get_shader("cube").load_shader("shader/cube/cube.vs.glsl",
                                   "shader/cube/cube.fs.glsl",
                                   "shader/cube/cube.gs.glsl");
    get_shader("points").load_shader("shader/points/points.vs.glsl",
                                     "shader/points/points.fs.glsl",
                                     "shader/points/points.gs.glsl");
    get_shader("text").load_shader("shader/text/text.vs.glsl",
                                   "shader/text/text.fs.glsl",
                                   "shader/text/text.gs.glsl");
}

shader& shader_map::get_shader(const std::string& name)
{
    return m_map_shaders[name];
}