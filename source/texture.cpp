//
//  texture.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <texture.h>
#include <fstream>
#include <memory>
#include <cstdlib>
#include <filesystem.h>

namespace hcube
{
	texture::texture()
	{
	}

	texture::~texture()
	{
		destoy();
	}

	texture::texture(const std::string& path)

	{
		load(path);
	}

	texture::texture(const attributes& attr,
		const std::string& path)

	{
		load(attr, path);
	}

	texture::texture(const attributes& attr,
		const unsigned char* buffer,
		unsigned long width,
		unsigned long height,
		texture_format format,
		texture_type   type)
	{
		build(attr, buffer, width, height, format, type);
	}

	texture::texture(const attributes& attr,
		const std::vector< unsigned char >& buffer,
		unsigned long width,
		unsigned long height,
		texture_format format,
		texture_type   type)
	{
		build(attr, buffer, width, height, format, type);
	}

	bool texture::load(const std::string& path)
	{
		return load(texture::attributes::rgba_linear_mipmap_linear(), path);
	}

	extern "C"
	{
		unsigned char* stbi_load_from_memory
		(
			unsigned char const *buffer,
			int len,
			int *x,
			int *y,
			int *comp,
			int req_comp
		);
		void stbi_image_free(void* image);
		void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);
	}

	bool decode_tga(std::vector<unsigned char>& out_image,
					unsigned long&  image_width,
					unsigned long&  image_height,
					texture_format& image_format,
					texture_type&   image_type,
					const unsigned char* in_tga,
					size_t in_size);

	bool texture::load(const attributes& attr,
					   const std::string& path)
	{
		//get ext
		std::string ext = filesystem::get_extension(path);
		//test
		if (   ext != ".png"
			&& ext != ".jpg"
			&& ext != ".jpeg"
			&& ext != ".tga") return false;
		//read all file
		std::vector<char> data_file = filesystem::file_read_all(path);
		//fail to read?
		if (!data_file.size()) return false;
		//decode
		unsigned long image_width = 0;
		unsigned long image_height = 0;
		texture_type   image_type;
		texture_format image_format;
		std::vector<unsigned char> image;
		//png
		if (   ext == ".png" 
			|| ext == ".jpg" 
			|| ext == ".jpeg")
		{
			stbi_set_flip_vertically_on_load(true);
			//rgb/rgba attribute
			attributes attr_png = attr;
			unsigned char* data = nullptr;
			int width = 0;
			int height = 0;
			int components = 0;
			//decoder
			data = stbi_load_from_memory((const unsigned char*)data_file.data(),
				(int)data_file.size(),
				&width,
				&height,
				&components,
				0);
			if (!data) return false;
			//to ulong
			image_width = width;
			image_height = height;
			//save
			switch (components)
			{
			case 1: image_format = TF_R8;     image_type = TT_R;    break;
			case 2: image_format = TF_RG8;    image_type = TT_RG;   break;
			case 3: image_format = TF_RGB8;   image_type = TT_RGB;  break;
			case 4: image_format = TF_RGBA8;  image_type = TT_RGBA; break;
			default:
				stbi_image_free(data);
				return false;
				break;
			}
			//delete alpha channel from attributes (if needed)
			attr_png.m_type = image_type;
			attr_png.m_format = image_format;
			//copy into C++ buffer
			image.resize(width*height*components);
			std::memcpy(&image[0], data, image.size());
			//build
			bool builded = build(attr_png, image.data(), image_width, image_height, image_format, image_type);
			//free stb image
			stbi_image_free(data);
			//return
			return builded;
		}
		//tga
		else
		{
			//exit attribute
			attributes attr_tga = attr;
			//decode
			if (!decode_tga(image, image_width,
				image_height,
				image_format,
				image_type,
				(const unsigned char*)data_file.data(),
				data_file.size()))
				return false;
			//delete alpha channel from attributes (if needed)
			attr_tga.m_type = image_type;
			attr_tga.m_format = image_format;
			//build
			return build(attr_tga, image.data(), image_width, image_height, image_format, image_type);
		}
	}

	bool texture::build(const attributes& attr,
		const std::vector< unsigned char >& buffer,
		unsigned long width,
		unsigned long height,
		texture_format format,
		texture_type   type)
	{
		return build(attr, buffer.data(), width, height, format, type);
	}

	bool texture::build(const attributes& attr,
		const unsigned char* buffer,
		unsigned long width,
		unsigned long height,
		texture_format format,
		texture_type   type)
	{
		//save size
		m_width = width;
		m_height = height;
		//create texture
		m_ctx_texture =
			render::create_texture(
				{
					format,
					(unsigned int)m_width,
					(unsigned int)m_height,
					buffer,
					type,
					TTF_UNSIGNED_BYTE 
				},
				attr.m_min_filter,
				attr.m_mag_filter,
				attr.m_clamp_to_border ? TEDGE_CLAMP : TEDGE_REPEAT,
				attr.m_clamp_to_border ? TEDGE_CLAMP : TEDGE_REPEAT,
				attr.m_build_mipmap);
		//ok
		return m_ctx_texture != nullptr;
	}

	context_texture* texture::get_context_texture() const
	{
		return m_ctx_texture;
	}

	unsigned long texture::get_width() const
	{
		return m_width;
	}

	unsigned long texture::get_height() const
	{
		return m_height;
	}

	void texture::destoy()
	{
		if (m_ctx_texture) render::delete_texture(m_ctx_texture);
	}
}