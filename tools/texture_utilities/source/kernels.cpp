//
//  kernels.cpp
//  texture_utilities
//
//  Created by Gabriele on 09/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <kernels.h>

std::vector<kernel_type> global_map_kernels
{
    //1
    {
        "255-r",
        [](image& thiz,image_rgba& pixel,unsigned long x,unsigned long y)
        {
            pixel.m_r = 255-pixel.m_r;
        }
    },
    //2
    {
        "255-g",
        [](image& thiz,image_rgba& pixel,unsigned long x,unsigned long y)
        {
            
            pixel.m_g = 255-pixel.m_g;
        }
    },
    //3
    {
        "255-b",
        [](image& thiz,image_rgba& pixel,unsigned long x,unsigned long y)
        {
            
            pixel.m_g = 255-pixel.m_g;
        }
    },
    //4
    {
        "255-a",
        [](image& thiz,image_rgba& pixel,unsigned long x,unsigned long y)
        {
            
            pixel.m_g = 255-pixel.m_a;
        }
    },
    //5
    {
        "swap-r-b",
        [](image& thiz,image_rgba& pixel,unsigned long x,unsigned long y)
        {
            std::swap(pixel.m_r,pixel.m_b);
        }
    },
    //6
    {
        "swap-r-g",
        [](image& thiz,image_rgba& pixel,unsigned long x,unsigned long y)
        {
            std::swap(pixel.m_r,pixel.m_g);
        }
    },
    //7
    {
        "swap-g-b",
        [](image& thiz,image_rgba& pixel,unsigned long x,unsigned long y)
        {
            std::swap(pixel.m_g,pixel.m_b);
        }
    }
};

std::vector<image_kernel> compile_kernels(int start_arg, int argc,const char * argv[])
{
    std::vector<image_kernel> output;
    for(int i=start_arg;i!=argc;++i)
    {
        for(auto& i_kernel : global_map_kernels)
        {
            if(i_kernel.m_name == std::string(argv[i]))
            {
                output.push_back(i_kernel.m_kernel);
            }
        }
    }
    return output;
}
