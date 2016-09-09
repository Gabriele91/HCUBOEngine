//
//  image.h
//  texture_utilities
//
//  Created by Gabriele on 09/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <struct_attribute.h>
//image declaretion
class  image;
//pixel declaretion
PACKED(struct image_rgba
{
    unsigned char m_r;
    unsigned char m_g;
    unsigned char m_b;
    unsigned char m_a;
});

//kernel declaretion
using image_kernel = std::function< void(image& thiz, image_rgba& pixel, unsigned long x,unsigned long y) >;

//image definition
class image : public std::enable_shared_from_this<image>
{
public:
    static std::shared_ptr<image> from_r    (const unsigned char* buffer,unsigned long width,unsigned long height);
    static std::shared_ptr<image> from_rg   (const unsigned char* buffer,unsigned long width,unsigned long height);
    static std::shared_ptr<image> from_rgb  (const unsigned char* buffer,unsigned long width,unsigned long height);
    static std::shared_ptr<image> from_rgb16(const unsigned char* buffer,unsigned long width,unsigned long height);
    static std::shared_ptr<image> from_rgba (const unsigned char* buffer,unsigned long width,unsigned long height);
    
    image(){}
    
    void apply_kernel(image_kernel kernel);
    void flip_vertical();
    void flip_horizontal();
    
    unsigned long get_width() const  { return m_width; }
    unsigned long get_height() const { return m_height; }
    
    std::vector< unsigned char > to_rgb();
    std::vector< unsigned char > to_rgb16();
    std::vector< unsigned char > to_rgba();

protected:
    
    unsigned long m_width { 0 };
    unsigned long m_height{ 0 };
    std::vector < image_rgba > m_buffer;
    
};