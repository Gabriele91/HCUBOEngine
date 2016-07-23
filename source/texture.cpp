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

static void image_y_flip(std::vector<unsigned char>& bytes,
                         unsigned long image_bytes_pixel,
                         unsigned long image_width,
                         unsigned long image_height)
{
    //line size
    size_t line_len=image_width * image_bytes_pixel * sizeof(unsigned char);
    //alloc
    unsigned char* line= new unsigned char[line_len];
    //for size
    size_t half_height=image_height / 2;
    //for all lines
    for(size_t y=0; y!=half_height; ++y)
    {
        std::memcpy(line,&bytes[line_len*y],line_len);
        std::memcpy(&bytes[line_len*y],&bytes[line_len*(image_height-y-1)],line_len);
        std::memcpy(&bytes[line_len*(image_height-y-1)],line,line_len);
    }
    //dealloc
    delete [] line;
}


bool texture::load(const attributes& attr,
                   const std::string& path)
{
    extern int decode_png(std::vector<unsigned char>& out_image,
                          unsigned long& image_width,
                          unsigned long& image_height,
                          const unsigned char* in_png,
                          size_t in_size,
                          bool convert_to_rgba32);

	bool decode_tga(std::vector<unsigned char>& out_image,
					unsigned long& image_width,
					unsigned long& image_height,
					GLenum&	      open_gl_type,
					const unsigned char* in_tga,
					size_t in_size);
	//get ext
	std::string ext = filesystem::get_extension(path);
	//test
	if (ext != ".png" && ext != ".tga") return false;
	//read all file
	std::vector<char> data_file = filesystem::file_read_all(path);
	//fail to read?
    if(!data_file.size()) return false;
    //decode
    unsigned long image_width;
    unsigned long image_height;
	GLenum	      image_type;
    std::vector<unsigned char> image;
	//png
	if(ext == ".png")
	{
		if(decode_png(image, image_width, image_height, 
			         (const unsigned char*)data_file.data(), data_file.size(),  
				     true)!=0)
		return false;
		//flip y
		image_y_flip(image, 4, image_width, image_height);
		//type
		image_type = GL_RGBA;
		//build
		return build(attr, image.data(), image_width, image_height, image_type);
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