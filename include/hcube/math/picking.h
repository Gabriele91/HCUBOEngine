//
//  picking.h
//  OGLHCubeView
//
//  Created by Gabriele on 03/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <camera.h>
#include <OpenGL4.h>
#include <vector_math.h>

namespace hcube
{
	static inline ivec2 pixel_screen_to_viewport(const camera& cam, const ivec2& s_pixel)
	{
		int screen_y = cam.get_viewport().y;
		int screen_height = cam.get_viewport().w;
		int pixel_y = ((screen_height + screen_y) - s_pixel.y);
		return ivec2(s_pixel.x, pixel_y);
	}

	static inline float get_depth_pixel(const camera& cam, const ivec2& s_pixel)
	{
		ivec2 v_pixel = pixel_screen_to_viewport(cam, s_pixel);
		float depth;
		glReadPixels(v_pixel.x, v_pixel.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		return depth;
	}

	struct picking_state
	{
		bool m_background;
		vec3 m_pick;

		operator vec3& ()
		{
			return m_pick;
		}

		operator vec3() const
		{
			return m_pick;
		}
	};
	static inline picking_state camera_picking(camera& cam, const vec2& s_pixel)
	{
		float      d_pixel = get_depth_pixel(cam, s_pixel);
		ivec2 v_pixel = pixel_screen_to_viewport(cam, s_pixel);
		ivec4 viewport = cam.get_viewport();
		//mouse in opengl window space
		vec4  clip_pixel(v_pixel.x - viewport.x, v_pixel.y - viewport.y, d_pixel, 1.0);
		//normalize
		clip_pixel.x /= viewport.z;
		clip_pixel.y /= viewport.w;
		//clip space [-1....+1] (homogenee coords)
		clip_pixel *= 2.0;
		clip_pixel -= 1.0;
		//clip space to view space
		vec4 view_pixel = cam.get_projection_inv()*clip_pixel;
		view_pixel /= view_pixel.w;
		//view space to wolrd space
		vec4 world_pixel = cam.get_view_inv()*view_pixel;
		//return
		return picking_state{ d_pixel >= 1.0f ,  (vec3)world_pixel };
	}
}