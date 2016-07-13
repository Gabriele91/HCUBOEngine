//
//  collection_materials.h
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <material.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <smart_pointers.h>
#include <resources_manager.h>

class base_material : public material, public smart_pointers<base_material>
{
public:
    
    base_material()
    {
    }
    
    base_material(resources_manager& resources)
    {
        init(resources);
    }
    
    base_material(shader::ptr base_shader)
    {
        init(base_shader);
    }
    
    void init(resources_manager& resources)
    {
        init(resources.get_shader("base"));
    }
    
    void init(shader::ptr base_shader)
    {
        //save pr shader
        m_shader = base_shader;
        //get color uniform
        m_uniform_color = m_shader->get_shader_uniform_vec4("in_color");
        //matrix
        m_uniform_projection = m_shader->get_shader_uniform_mat4("projection");
        m_uniform_view       = m_shader->get_shader_uniform_mat4("view");
        m_uniform_model      = m_shader->get_shader_uniform_mat4("model");
    }
    
    void set_color(const glm::vec4& color)
    {
        m_color = color;
    }
    
    virtual void bind(camera* cam,const glm::mat4& model)
    {
        m_shader->bind();
        m_uniform_color->set_value(m_color);
        m_uniform_projection->set_value(cam->get_projection());
        m_uniform_view->set_value(cam->get_view());
        m_uniform_model->set_value(model);
    }
    
    virtual void unbind()
    {
        m_shader->unbind();
    }
    
private:
    
    shader::ptr       m_shader{ nullptr };
    glm::vec4         m_color;
    uniform_vec4::ptr m_uniform_color{ nullptr };
    //model info
    uniform_mat4::ptr m_uniform_projection{ nullptr };
    uniform_mat4::ptr m_uniform_view{ nullptr };
    uniform_mat4::ptr m_uniform_model{ nullptr };
};

class points_material : public material, public smart_pointers< points_material >
{
public:
    
    points_material()
    {
    }
    
    points_material(resources_manager& resources)
    {
        init(resources);
    }
    
    points_material(shader::ptr points_shader)
    {
        init(points_shader);
    }
    
    void init(resources_manager& resources)
    {
        init(resources.get_shader("points"));
    }
    
    void init(shader::ptr points_shader)
    {
        //save pr shader
        m_shader = points_shader;
        //get color uniform
        m_uniform_color_center = m_shader->get_shader_uniform_vec4("in_color_center");
        m_uniform_color_border = m_shader->get_shader_uniform_vec4("in_color_border");
        //get geometry uniform
        m_uniform_size       = m_shader->get_shader_uniform_vec2("size");
        m_uniform_projection = m_shader->get_shader_uniform_mat4("projection");
        m_uniform_view       = m_shader->get_shader_uniform_mat4("view");
        m_uniform_model      = m_shader->get_shader_uniform_mat4("model");
    }
    
    void set_color_center(const glm::vec4& color_center)
    {
        m_color_center = color_center;
    }
    
    void set_color_border(const glm::vec4& color_border)
    {
        m_color_border = color_border;
    }
    
    void set_point_size(const glm::vec2& size)
    {
        m_size = size;
    }
    
    virtual void bind(camera* cam,const glm::mat4& model)
    {
        bind_state();
        m_shader->bind();
        m_uniform_color_center->set_value(m_color_center);
        m_uniform_color_border->set_value(m_color_border);
        m_uniform_size->set_value(m_size);
        m_uniform_projection->set_value(cam->get_projection());
        m_uniform_view->set_value(cam->get_view());
        m_uniform_model->set_value(model);
    }
    
    virtual void unbind()
    {
        m_shader->unbind();
        unbind_state();
    }
    
private:
    
    shader::ptr       m_shader{ nullptr };
    //color info
    glm::vec4         m_color_center;
    uniform_vec4::ptr m_uniform_color_center{ nullptr };
    glm::vec4         m_color_border;
    uniform_vec4::ptr m_uniform_color_border{ nullptr };
    //model info
    uniform_mat4::ptr m_uniform_projection{ nullptr };
    uniform_mat4::ptr m_uniform_view{ nullptr };
    uniform_mat4::ptr m_uniform_model{ nullptr };
    //size point
    glm::vec2         m_size;
    uniform_vec2::ptr m_uniform_size;
};


class triangle_lines_material : public material, public smart_pointers< triangle_lines_material >
{
    
public:
    
    triangle_lines_material()
    {
    }
    
    triangle_lines_material(resources_manager& resources)
    {
        init(resources);
    }
    
    triangle_lines_material(shader::ptr points_shader)
    {
        init(points_shader);
    }
    
    void init(resources_manager& resources)
    {
        init(resources.get_shader("triangle_lines"));
    }
    
    void init(shader::ptr points_shader)
    {
        //save pr shader
        m_shader = points_shader;
        //windows scale
        m_uniform_window_size = m_shader->get_shader_uniform_vec2("window_scale");
        m_uniform_line_size   = m_shader->get_shader_uniform_vec2("line_size");
        //get color uniform
        m_uniform_color_wire = m_shader->get_shader_uniform_vec4("in_color_wire");
        m_uniform_color_fill = m_shader->get_shader_uniform_vec4("in_color_fill");
        //get geometry uniform
        m_uniform_projection = m_shader->get_shader_uniform_mat4("projection");
        m_uniform_view       = m_shader->get_shader_uniform_mat4("view");
        m_uniform_model      = m_shader->get_shader_uniform_mat4("model");
    }
    
    void set_line_size(float line_size)
    {
        m_line_size.x = line_size;
        m_line_size.y = line_size;
    }
    
    void set_line_size(const glm::vec2& line_size)
    {
        m_line_size = line_size;
    }
    
    void set_window_size(const glm::vec2& window_size)
    {
        m_window_size = window_size;
    }
    
    void set_color_fill(const glm::vec4& color_fill)
    {
        m_color_fill = color_fill;
    }
    
    void set_color_wire(const glm::vec4& color_wire)
    {
        m_color_wire = color_wire;
    }
    
    virtual void bind(camera* cam,const glm::mat4& model)
    {
        bind_state();
        m_shader->bind();
        m_uniform_window_size->set_value(m_window_size);
        m_uniform_line_size->set_value(m_line_size);
        m_uniform_color_fill->set_value(m_color_fill);
        m_uniform_color_wire->set_value(m_color_wire);
        m_uniform_projection->set_value(cam->get_projection());
        m_uniform_view->set_value(cam->get_view());
        m_uniform_model->set_value(model);
    }
    
    virtual void unbind()
    {
        m_shader->unbind();
        unbind_state();
    }
    
private:
    
    shader::ptr       m_shader{ nullptr };
    //line size
    glm::vec2         m_line_size{ 1.0, 1.0 };
    uniform_vec2::ptr m_uniform_line_size;
    //window size
    glm::vec2         m_window_size;
    uniform_vec2::ptr m_uniform_window_size;
    //color info
    glm::vec4         m_color_fill;
    uniform_vec4::ptr m_uniform_color_fill{ nullptr };
    glm::vec4         m_color_wire;
    uniform_vec4::ptr m_uniform_color_wire{ nullptr };
    //model info
    uniform_mat4::ptr m_uniform_projection{ nullptr };
    uniform_mat4::ptr m_uniform_view{ nullptr };
    uniform_mat4::ptr m_uniform_model{ nullptr };
};

class wireframe_material : public material, public smart_pointers< wireframe_material >
{
public:
    
    wireframe_material()
    {
    }
    
    wireframe_material(resources_manager& resources)
    {
        init(resources);
    }
    
    wireframe_material(shader::ptr points_shader)
    {
        init(points_shader);
    }
    
    void init(resources_manager& resources)
    {
        init(resources.get_shader("wireframe"));
    }
    
    void init(shader::ptr points_shader)
    {
        //save pr shader
        m_shader = points_shader;
        //windows scale
        m_uniform_window_size = m_shader->get_shader_uniform_vec2("window_scale");
        m_uniform_line_size   = m_shader->get_shader_uniform_vec2("line_size");
        //get color uniform
        m_uniform_color_lines = m_shader->get_shader_uniform_vec4("in_color_lines");
        //get geometry uniform
        m_uniform_projection = m_shader->get_shader_uniform_mat4("projection");
        m_uniform_view       = m_shader->get_shader_uniform_mat4("view");
        m_uniform_model      = m_shader->get_shader_uniform_mat4("model");
    }
    
    void set_line_size(float line_size)
    {
        m_line_size.x = line_size;
        m_line_size.y = line_size;
    }
    
    void set_line_size(const glm::vec2& line_size)
    {
        m_line_size = line_size;
    }
    
    void set_color_lines(const glm::vec4& color_wire)
    {
        m_color_lines = color_wire;
    }
    
    virtual void bind(camera* cam,const glm::mat4& model)
    {
        bind_state();
        m_shader->bind();
        m_uniform_window_size->set_value(cam->get_viewport_size());
        m_uniform_line_size->set_value(m_line_size);
        m_uniform_color_lines->set_value(m_color_lines);
        m_uniform_projection->set_value(cam->get_projection());
        m_uniform_view->set_value(cam->get_view());
        m_uniform_model->set_value(model);
    }
    
    virtual void unbind()
    {
        m_shader->unbind();
        unbind_state();
    }
    
private:
    
    shader::ptr       m_shader{ nullptr };
    //line size
    glm::vec2         m_line_size{ 1.0, 1.0 };
    uniform_vec2::ptr m_uniform_line_size;
    //color info
    glm::vec4         m_color_lines;
    uniform_vec4::ptr m_uniform_color_lines{ nullptr };
    //window size
    uniform_vec2::ptr m_uniform_window_size;
    //model info
    uniform_mat4::ptr m_uniform_projection{ nullptr };
    uniform_mat4::ptr m_uniform_view{ nullptr };
    uniform_mat4::ptr m_uniform_model{ nullptr };
};

class text_material : public material, public smart_pointers< text_material >
{
public:
    
    text_material()
    {
    }
    
    text_material(resources_manager& resources)
    {
        init(resources);
    }
    
    text_material(shader::ptr points_shader)
    {
        init(points_shader);
    }
    
    void init(resources_manager& map)
    {
        init(map.get_shader("text"));
    }
    
    void init(shader::ptr points_shader)
    {
        //save pr shader
        m_shader = points_shader;
        //uniform texture
        m_uniform_image = m_shader->get_shader_uniform_texture("image");
        //color
        m_uniform_color = m_shader->get_shader_uniform_vec4("in_color");
        //uniform cell info
        m_uniform_cell_size   = m_shader->get_shader_uniform_vec2("cell_size");
        m_uniform_cell_offset = m_shader->get_shader_uniform_vec2("cell_offset");
        //uniform render info
        m_uniform_render_size   = m_shader->get_shader_uniform_vec2("render_size");
        m_uniform_render_origin = m_shader->get_shader_uniform_vec2("render_origin");
    }
    
    void set_image(texture::ptr image)
    {
        m_image = image;
    }
    
    void set_color(const glm::vec4& color)
    {
        m_color = color;
    }
    
    void set_cell_size(const glm::vec2& cell_size)
    {
        m_cell_size = cell_size;
    }
    
    void set_cell_offset(const glm::vec2& cell_offset)
    {
        m_cell_offset = cell_offset;
    }
    
    void set_render_size(const glm::vec2& render_size)
    {
        m_render_size = render_size;
    }
    
    void set_render_origin(const glm::vec2& render_origin)
    {
        m_render_origin = render_origin;
    }
    
    virtual void bind(camera* cam,const glm::mat4& model)
    {
        bind_state();
        m_shader->bind();
        m_uniform_image->set_value(m_image);
        m_uniform_cell_size->set_value(m_cell_size);
        m_uniform_cell_offset->set_value(m_cell_offset);
        m_uniform_render_size->set_value(m_render_size);
        m_uniform_render_origin->set_value(m_render_origin);
        m_uniform_color->set_value(m_color);
    }
    
    virtual void unbind()
    {
        m_shader->unbind();
        unbind_state();
    }
    
private:
    
    shader::ptr          m_shader{ nullptr };
    //cell
    texture::ptr         m_image{ nullptr };
    uniform_texture::ptr m_uniform_image{ nullptr };
    //cell
    glm::vec2         m_cell_size;
    uniform_vec2::ptr m_uniform_cell_size{ nullptr };
    //offset
    glm::vec2         m_cell_offset;
    uniform_vec2::ptr m_uniform_cell_offset{ nullptr };
    //cell
    glm::vec2         m_render_size;
    uniform_vec2::ptr m_uniform_render_size{ nullptr };
    //offset
    glm::vec2         m_render_origin;
    uniform_vec2::ptr m_uniform_render_origin{ nullptr };
    //color
    glm::vec4         m_color;
    uniform_vec4::ptr m_uniform_color{ nullptr };
};