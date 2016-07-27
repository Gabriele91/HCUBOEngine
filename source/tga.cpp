//
//  tga.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <vector>
#include <OpenGL4.h>
#include <struct_attribute.h>

enum tga_image_type
{
	TGA_NONE,
	TGA_INDEXED,
	TGA_RGB,
	TGA_GREY,
	TGA_RLE = 8
};

PACKED(struct tga_header
{
	unsigned char  m_identsize;          // size of ID field that follows 18 byte header (0 usually)
	unsigned char  m_colourmaptype;      // type of colour map 0=none, 1=has palette
	unsigned char  m_imagetype;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

	short m_colourmapstart;            // first colour map entry in palette
	short m_colourmaplength;           // number of colours in palette
	unsigned char  m_colourmapbits;    // number of bits per palette entry 15,16,24,32

	short m_xstart;                    // image x origin
	short m_ystart;				       // image y origin
	short m_width;				       // image width in pixels
	short m_height;					   // image height in pixels
	unsigned char  m_bits;             // image bits per pixel 8,16,24,32
	unsigned char  m_descriptor;       // image descriptor bits (vh flip bits)
});


static void rga_swap_r_and_b(unsigned char* bytes,
							 unsigned long image_bytes_pixel,
							 unsigned long width,
							 unsigned long height)
{
	size_t image_size = width * height;
	//24/32 image
	for (unsigned int i = 0; i != image_size; ++i)
	{
		unsigned char tmp;
		tmp = bytes[i * image_bytes_pixel + 0];
		      bytes[i * image_bytes_pixel + 0] = bytes[i * image_bytes_pixel + 2];
		      bytes[i * image_bytes_pixel + 2] = tmp;
	}
}

bool decode_tga(std::vector<unsigned char>& out_image,
		   	    unsigned long& image_width,
			    unsigned long& image_height,
			    GLenum&	      open_gl_type,
			    const unsigned char* in_tga,
			    size_t in_size)
{
	//ptr to data
	tga_header*		     header = (tga_header*)in_tga;
	const unsigned char* data = in_tga + sizeof(tga_header);
	//only rgb supported
	if (header->m_imagetype != TGA_RGB) return false;
	//return size
	image_width  = header->m_width;
	image_height = header->m_height;
	//opengl type
	switch (header->m_bits)
	{
		case 8:   open_gl_type = GL_R8;     break;
		case 16:  open_gl_type = GL_RGB565; break;
		case 24:  open_gl_type = GL_RGB;    break;
		case 32:  open_gl_type = GL_RGBA;   break;
		default: return false; break;
	}
	//alloc
	out_image.resize(in_size - sizeof(tga_header));
	//copy data
	std::memcpy(&out_image[0], data, out_image.size());
	//swap
	rga_swap_r_and_b(out_image.data(), header->m_bits/8, image_width, image_height);
	//success
	return true;
}