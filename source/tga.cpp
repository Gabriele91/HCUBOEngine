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
#include <memory>
#include <cstring>

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

static void decoder_rle(unsigned long width,
                        unsigned long height,
                        unsigned long image_bytes_pixel,
                        const unsigned char* buffer_in,
                        std::vector<unsigned char>& out_image)
{
    //bytes
    const unsigned char* c_ptr = buffer_in;
    //new alloc
    out_image.resize(width * height * image_bytes_pixel);
    //ptr to buffer out
    const unsigned char* buffer_out = out_image.data();
    //length of chunk
    size_t i=0;
    //length of chunk
    unsigned char length=0;
    // Decode
    while(i < out_image.size())
    {
        // 1XXX XXXX RLE chunk [ LEN | PIXEL ]
        if(*c_ptr & 0x80)
        {
            //length
            length = *c_ptr - 127;
            //pixel
            ++c_ptr;
            //copy
            for(; length; ++i, --length)
                std::memcpy((void*)&buffer_out[i],c_ptr,image_bytes_pixel);
            //next chunk
            c_ptr += image_bytes_pixel;
        }
        // RAW chunk  [ LEN | PIXEL | PIXEL |... ]
        else
        {
            //length
            length = *c_ptr + 1;
            //pixel
            ++c_ptr;
            //copy
            std::memcpy((void*)&buffer_out[i],c_ptr,image_bytes_pixel*length);
            //offset
            c_ptr += image_bytes_pixel*length;
            //length to 0
            length = 0;
        }
    }
}

static void rga_swap_r_and_b_16(unsigned char* bytes,
                                unsigned long width,
                                unsigned long height)
{
    size_t image_size = width * height;
    //24/32 image
    for (unsigned int i = 0; i != image_size; ++i)
    {
        // R R R R R G G G bytes[i*2+0]
        // G G G B B B B B bytes[i*2+1]
        unsigned char tmp_r = bytes[i*2+0];
        //                          R/B                                   G
        bytes[i*2+0] = ((bytes[i*2+1] << 3) & 0b11111000) | (bytes[i*2+0] & 0b00000111);
        bytes[i*2+1] = ((tmp_r        >> 3) & 0b00011111) | (bytes[i*2+1] & 0b11100000);
    }
}

static void rga_swap_r_and_b_24_32(unsigned char* bytes,
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
    if (header->m_imagetype != TGA_RGB &&
        header->m_imagetype != TGA_RLE) return false;
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
	//copy data
	if(header->m_imagetype == TGA_RLE)
    {
        decoder_rle(image_width, image_height, header->m_bits/8, data, out_image);
    }
    else
    {
        //resize
        out_image.resize(in_size - sizeof(tga_header));
        //copy raw data
        std::memcpy(&out_image[0], data, out_image.size());
    }
    //swap r/b
    switch (header->m_bits)
    {
        //2 bytes
        case 16:
            rga_swap_r_and_b_16(out_image.data(),image_width, image_height);
        break;
        //3/4 bytes
        case 24:
        case 32:
            rga_swap_r_and_b_24_32(out_image.data(), header->m_bits/8, image_width, image_height);
        break;
        default:
        break;
    }
	//success
	return true;
}