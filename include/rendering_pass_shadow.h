//
//  rendering_pass_shadow.h
//  HCubo
//
//  Created by Gabriele on 15/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <smart_pointers.h>
#include <vector_math.h>
#include <effect.h>
#include <rendering_system.h>
#include <resources_manager.h>

namespace hcube
{
    class rendering_pass_shadow : public rendering_pass, public smart_pointers<rendering_pass_shadow>
    {
        
        effect::ptr		   m_effect;
        effect::parameter* m_mask;
        effect::parameter* m_diffuse_map;
        effect::technique* m_technique_shadow_spot;
        effect::technique* m_technique_shadow_point;
        effect::technique* m_technique_shadow_direction;
        //spot light
        uniform*           m_shadow_spot_mask		    { nullptr };
        //point light
        uniform*           m_shadow_point_mask			{ nullptr };
        uniform*           m_shadow_point_light_position{ nullptr };
        uniform*           m_shadow_point_far_plane		{ nullptr };
        //direction light
        uniform*           m_shadow_direction_mask		{ nullptr };
        //view / projection
        uniform*           m_shadow_spot_model      { nullptr };
        uniform*           m_shadow_spot_view		{ nullptr };
        uniform*           m_shadow_spot_projection { nullptr };
        
        uniform*           m_shadow_point_model     { nullptr };
        uniform*           m_shadow_point_view      { nullptr };
        uniform*           m_shadow_point_projection{ nullptr };
        
        uniform*           m_shadow_direction_model	    { nullptr };
        uniform*           m_shadow_direction_view	    { nullptr };
        uniform*           m_shadow_direction_projection{ nullptr };
        
    public:
        
        rendering_pass_shadow(resources_manager& resources);
        
        virtual void draw_pass(
                               int    n_pass,
                               vec4&  clear_color,
                               vec4&  ambient_color,
                               entity::ptr e_camera,
                               render_scene& rscene
                               );
    };
}