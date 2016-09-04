#include <shadow_buffer.h>

namespace hcube
{


	bool shadow_buffer::init(const ivec2& shadow_size, bool is_cube_map)
	{
		//size
		m_width = shadow_size.x;
		m_height = shadow_size.y;
		//texture info
		texture_raw_data_information raw_data
		{
			TF_DEPTH_COMPONENT32,
			m_width,
			m_height,
			nullptr,
			TT_DEPTH,
			TTF_FLOAT
		};
		texture_gpu_data_information gpu_data
		{
			TMIN_NEAREST,
			TMAG_NEAREST,
			TEDGE_CLAMP,
			TEDGE_CLAMP,
			TEDGE_CLAMP,
			false
		};
		//depth
		if (is_cube_map)
		{
			texture_raw_data_information cube_raw_data[6]
			{
				raw_data,raw_data,raw_data,
				raw_data,raw_data,raw_data,
			};
			m_depth_texture = render::create_cube_texture(cube_raw_data, gpu_data);
		}
		else
		{
			m_depth_texture = render::create_texture(raw_data, gpu_data);
		}
		//target
		m_target = render::create_render_target
		({
			target_field{ m_depth_texture, RT_DEPTH },
		});
		//return 
		return m_target != nullptr;
	}

	void shadow_buffer::destoy()
	{
        if(m_target)
        {
            render::delete_render_target(m_target);
            render::delete_texture(m_depth_texture);
        }
	}

	void shadow_buffer::bind() const
	{
        assert(m_target);
		render::enable_render_target(m_target);
	}

	void shadow_buffer::unbind() const
    {
        assert(m_target);
		render::disable_render_target(m_target);
	}

	void shadow_buffer::set_texture_buffer_depth(size_t n_texture) const
    {
        assert(m_depth_texture);
		render::bind_texture(m_depth_texture, (int)n_texture);
	}

	void shadow_buffer::disable_depth_texture() const
    {
        assert(m_depth_texture);
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