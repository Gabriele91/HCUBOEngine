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
    load(attr,path);
}

texture::texture(const attributes& attr,
                 const unsigned char* buffer,
                 unsigned long width,
                 unsigned long height,
	             GLenum  type)
{
    build(attr,buffer,width,height,type);
}

texture::texture(const attributes& attr,
                 const std::vector< unsigned char >& buffer,
                 unsigned long width,
                 unsigned long height,
	             GLenum  type)
{
    build(attr,buffer,width,height,type);
}

bool texture::load(const std::string& path)
{
    return load(texture::attributes::rgba_linear_mipmap(),
                path);
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
                unsigned long& image_width,
                unsigned long& image_height,
                GLenum&	      open_gl_type,
                const unsigned char* in_tga,
                size_t in_size);

bool texture::load(const attributes& attr,
                   const std::string& path)
{
	//get ext
	std::string ext = filesystem::get_extension(path);
	//test
	if (ext != ".png" && ext != ".tga") return false;
	//read all file
	std::vector<char> data_file = filesystem::file_read_all(path);
	//fail to read?
    if(!data_file.size()) return false;
    //decode
    unsigned long image_width  =0;
    unsigned long image_height =0;
	GLenum	      image_type;
    std::vector<unsigned char> image;
	//png
	if(ext == ".png")
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
        if(!data) return false;
        //to ulong
        image_width  = width;
        image_height = height;
        //save
        switch (components)
        {
            case 1: image_type = GL_R8;  break;
            case 2: image_type = GL_RG8; break;
            case 3: image_type = GL_RGB; break;
            case 4: image_type = GL_RGBA; break;
            default:
                stbi_image_free(data);
                return false;
            break;
        }
        //delete alpha channel from attributes (if needed)
        attr_png.m_type_image = image_type;
        //copyt into C++ buffer
        image.resize(width*height*components);
        std::memcpy(&image[0],data, image.size());
		//build
		bool builded = build(attr_png, image.data(), image_width, image_height, image_type);
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
		if (!decode_tga(image, image_width, image_height, image_type,
			            (const unsigned char*)data_file.data(), data_file.size()))
			return false;
		//delete alpha channel from attributes (if needed)
		attr_tga.m_type_image = image_type;
		//build
		return build(attr_tga, image.data(), image_width, image_height, image_type);
	}
}

bool texture::build(const attributes& attr,
                    const std::vector< unsigned char >& buffer,
                    unsigned long width,
                    unsigned long height,
	                GLenum  type)
{
    return build(attr,buffer.data(),width,height, type);
}

bool texture::build(const attributes& attr,
                    const unsigned char* buffer,
                    unsigned long width,
                    unsigned long height,
	                GLenum  type)
{
    //get last texture bind
    GLint tex_2d_bound_id;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &tex_2d_bound_id);
    //create texture
    glGenTextures(1, (GLuint*)&m_texture_id);
	glBindTexture(GL_TEXTURE_2D, m_texture_id);
    //save size
    m_width  = width;
    m_height = height;
    //set filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, attr.m_clamp_to_border ? GL_CLAMP_TO_BORDER : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, attr.m_clamp_to_border ? GL_CLAMP_TO_BORDER : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, attr.m_mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, attr.m_min_filter);
    //send byte info
    glTexImage2D(GL_TEXTURE_2D,
                 0,
				 type,
                 (GLsizei)width,
                 (GLsizei)height,
                 0,
                 attr.m_type_image,
                 GL_UNSIGNED_BYTE,
                 buffer);
    // Generate mipmaps, by the way.
    if(attr.m_build_mipmap) glGenerateMipmap(GL_TEXTURE_2D);
    //unbind texture
    glBindTexture(GL_TEXTURE_2D, tex_2d_bound_id);
    //ok
    return true;
}

unsigned int texture::get_id() const
{
    return m_texture_id;
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
    glDeleteTextures(1, (GLuint*)&m_texture_id);
    m_texture_id = 0; 
}