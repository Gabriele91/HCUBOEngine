//
//  texture.h
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <string>
#include <render.h>
#include <resource.h>
#include <smart_pointers.h>

namespace hcube
{
	class texture : public smart_pointers<texture>, public resource
	{

		context_texture* m_ctx_texture{ nullptr };
		unsigned long    m_width{ 0 };
		unsigned long    m_height{ 0 };

	public:

		struct attributes
		{
			texture_type   m_type{ TT_RGB };
			texture_format m_format{ TF_RGB8 };
			texture_min_filter_type m_min_filter{ TMIN_NEAREST };
			texture_mag_filter_type m_mag_filter{ TMAG_NEAREST };
			bool m_clamp_to_border{ false };
			bool m_build_mipmap{ false };

			bool have_alpha_channel()
			{
				return m_type == TT_RGBA;
			}

			attributes(texture_type   type_image,
				texture_format format_image,
				texture_min_filter_type  min_filter = TMIN_NEAREST,
				texture_mag_filter_type  mag_filter = TMAG_NEAREST,
				bool clamp_to_border = false,
				bool build_mipmap = false)
			{
				m_type = type_image;
				m_format = format_image;
				m_min_filter = min_filter;
				m_mag_filter = mag_filter;
				m_clamp_to_border = clamp_to_border;
				m_build_mipmap = build_mipmap;
			}

			static attributes rgba(texture_min_filter_type  min_filter = TMIN_NEAREST,
				texture_mag_filter_type  mag_filter = TMAG_NEAREST,
				bool clamp_to_border = false,
				bool build_mipmap = false)
			{
				return
				{
					TT_RGBA,
					TF_RGBA8,
					min_filter,
					mag_filter,
					clamp_to_border,
					build_mipmap
				};
			}

			static attributes rgba_linear(bool clamp_to_border = false,
				bool alpha_channel = true)
			{
				return
				{
					TT_RGBA,
					TF_RGBA8,
					TMIN_LINEAR,
					TMAG_LINEAR,
					clamp_to_border,
					false
				};
			}

			static attributes rgba_linear_mipmap_linear(bool clamp_to_border = false,
				bool alpha_channel = true)
			{
				return
				{
					TT_RGBA,
					TF_RGBA8,
					TMIN_LINEAR_MIPMAP_LINEAR,
					TMAG_LINEAR,
					clamp_to_border,
					true
				};
			}
		};

		texture();

		texture(const std::string& path);

		texture(const attributes& attr,
			const std::string& path);

		texture(const attributes& attr,
			const unsigned char* buffer,
			unsigned long  width,
			unsigned long  height,
			texture_format format,
			texture_type   type);

		texture(const attributes& attr,
			const std::vector< unsigned char >& buffer,
			unsigned long width,
			unsigned long height,
			texture_format format,
			texture_type   type);

		virtual ~texture();

		bool load(resources_manager& resources, const std::string& path)
		{
			return load(path);
		}

		bool load(const std::string& path);

		bool load(const attributes& attr,
			const std::string& path);


		bool build(const attributes& attr,
			const unsigned char* buffer,
			unsigned long width,
			unsigned long height,
			texture_format format,
			texture_type   type);

		bool build(const attributes& attr,
			const std::vector< unsigned char >& buffer,
			unsigned long width,
			unsigned long height,
			texture_format format,
			texture_type   type);

		context_texture* get_context_texture() const;
		unsigned long    get_width() const;
		unsigned long    get_height() const;

		void destoy();

	};
}