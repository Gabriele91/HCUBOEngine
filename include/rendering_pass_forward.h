#pragma once
#include <vector_math.h>
#include <smart_pointers.h>
#include <rendering_system.h>

namespace hcube
{

	class rendering_pass_forward : public rendering_pass, public smart_pointers< rendering_pass_forward >
	{
	public:
        
        rendering_pass_forward(render_queue_type type = RQ_OPAQUE)
        :rendering_pass(RPT_RENDER)
        ,m_queue_type(type)
        {
        }
        
		virtual void draw_pass
		(
            int n_pass,
			vec4&  clear_color,
			vec4&  ambient_color,
			entity::ptr e_camera,
			render_scene& rscene
		);
        
    protected:
        
        render_queue_type m_queue_type;
    
    };
}