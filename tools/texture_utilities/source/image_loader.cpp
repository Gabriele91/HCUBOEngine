//
//  image_loader.cpp
//  texture_utilities
//
//  Created by Gabriele on 09/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//

#include <stdio.h>
#include <vector>
#include <string>
#include <image.h>
#include <filesystem.h>

enum texture_type
{
    TT_R,
    TT_RG,
    TT_RGB,
    TT_RGBA,
    TT_DEPTH,
    TT_DEPTH_STENCIL
};

enum texture_format
{
    ////////////////////
    //RGBA
    TF_RGBA8,
    //uint
    TF_RGBA16UI,
    TF_RGBA32UI,
    //int
    TF_RGBA16I,
    TF_RGBA32I,
    //float
    TF_RGBA16F,
    TF_RGBA32F,
    ////////////////////
    //RGB
    TF_RGB8,
    //uint
    TF_RGB16UI,
    TF_RGB32UI,
    //int
    TF_RGB16I,
    TF_RGB32I,
    //float
    TF_RGB16F,
    TF_RGB32F,
    ////////////////////
    //RG
    TF_RG8,
    //uint
    TF_RG16UI,
    TF_RG32UI,
    //int
    TF_RG16I,
    TF_RG32I,
    //float
    TF_RG16F,
    TF_RG32F,
    ////////////////////
    //R
    TF_R8,
    //uint
    TF_R16UI,
    TF_R32UI,
    //int
    TF_R16I,
    TF_R32I,
    //float
    TF_R16F,
    TF_R32F,
    ////////////////////
    TF_RGB565,
    TF_RGB5A1,
    TF_RGBA4,
    ////////////////////
    TF_DEPTH16_STENCIL8,
    TF_DEPTH24_STENCIL8,
    TF_DEPTH32_STENCIL8,
    TF_DEPTH_COMPONENT16,
    TF_DEPTH_COMPONENT24,
    TF_DEPTH_COMPONENT32,
};


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

namespace hcube
{
    bool decode_tga(std::vector<unsigned char>& out_image,
                    unsigned long&  image_width,
                    unsigned long&  image_height,
                    texture_format& image_format,
                    texture_type&   image_type,
                    const unsigned char* in_tga,
                    size_t in_size);
}

std::shared_ptr<image> load_image(const std::string& path)
{
    //get ext
    std::string ext = hcube::filesystem::get_extension(path);
    //test
    if (   ext != ".png"
        && ext != ".jpg"
        && ext != ".jpeg"
        && ext != ".tga") return nullptr;
    //read all file
    std::vector<char> data_file = hcube::filesystem::file_read_all(path);
    //fail to read?
    if (!data_file.size()) return nullptr;
    //png
    if (   ext == ".png"
        || ext == ".jpg"
        || ext == ".jpeg")
    {
        stbi_set_flip_vertically_on_load(false);
        //rgb/rgba attribute
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
        if (!data) return nullptr;
        //save
        std::shared_ptr< image > o_image = nullptr;
        //save
        switch (components)
        {
            case 1: o_image = image::from_r(data, width, height);   break;
            case 2: o_image = image::from_rg(data, width, height);   break;
            case 3: o_image = image::from_rgb(data, width, height);   break;
            case 4: o_image = image::from_rgba(data, width, height);   break;
            default:  break;
        }
        //
        stbi_image_free(data);

        return o_image;
    }
    //tga
    else
    {
        //decode
        unsigned long image_width = 0;
        unsigned long image_height = 0;
        texture_type   image_type;
        texture_format image_format;
        std::vector<unsigned char> image_data;
        //decode
        if (!hcube::decode_tga(image_data,
                               image_width,
                               image_height,
                               image_format,
                               image_type,
                               (const unsigned char*)data_file.data(),
                               data_file.size()))
            return nullptr;
        //save
        std::shared_ptr< image > o_image = nullptr;
        //types
        switch (image_format)
        {
            case TF_R8:     o_image = image::from_r(image_data.data(), image_width, image_height);        break;
			case TF_RGB565: o_image = image::from_rgb565(image_data.data(), image_width, image_height);   break;
			case TF_RGB5A1: o_image = image::from_rgb5a1(image_data.data(), image_width, image_height);   break;
            case TF_RGB8:   o_image = image::from_rgb(image_data.data(), image_width, image_height);      break;
            case TF_RGBA8:  o_image = image::from_rgba(image_data.data(), image_width, image_height);     break;
            default: break;
        }
        return o_image;
    }
}
