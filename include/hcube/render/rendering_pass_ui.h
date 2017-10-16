#pragma once
#include <hcube/config.h>
#include <hcube/math/vector_math.h>
#include <hcube/core/smart_pointers.h>
#include <hcube/render/rendering_system.h>

namespace hcube
{
	class HCUBE_API rendering_pass_ui : public rendering_pass, public smart_pointers< rendering_pass_ui >
	{
	public:

		rendering_pass_ui()
		:rendering_pass(RPT_UI)
		, m_queue_type(RQ_UI)
		{
		}

		virtual void draw_pass
		(
			int n_pass,
			vec4&  clear_color,
			vec4&  ambient_color,
			entity::ptr e_camera,
			render_scene& rscene,
			rendering_system& rsystem
		);

	protected:

		render_scene_queue_type m_queue_type;

	};
}