#include <shadow_buffer.h>

namespace hcube
{


	bool shadow_buffer::init(const ivec2& shadow_size)
	{
		//size
		m_width  = shadow_size.x;
		m_height = shadow_size.y;
		//depth
		m_depth_texture = render::create_texture(
			TF_DEPTH_COMPONENT32,
			m_width,
			m_height,
			nullptr,
			TT_DEPTH,
			TTF_FLOAT,
			TMIN_NEAREST,
			TMAG_NEAREST,
			TEDGE_CLAMP,
			TEDGE_CLAMP,
			false
		);
		//target
		m_target = render::create_render_target({
			target_field{ m_depth_texture, RT_DEPTH },
		});
		//return 
		return m_target != nullptr;
	}

	void shadow_buffer::destoy()
	{
		render::delete_render_target(m_target);
		render::delete_texture(m_depth_texture);
	}

	void shadow_buffer::bind() const
	{
		render::enable_render_target(m_target);
	}

	void shadow_buffer::unbind() const
	{
		render::disable_render_target(m_target);
	}

	void shadow_buffer::set_texture_buffer_depth(size_t n_texture) const
	{
		render::bind_texture(m_depth_texture, n_texture);
	}

	void shadow_buffer::disable_depth_texture() const
	{
		render::unbind_texture(m_depth_texture);
	}

	context_texture* shadow_buffer::get_depth_texture() const
	{
		return m_depth_texture;
	}

	ivec2 shadow_buffer::get_size() const
	{
		return ivec2{ m_width,m_height };
	}
}