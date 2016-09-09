//
//  kernels.h
//  texture_utilities
//
//  Created by Gabriele on 09/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <string>
#include <vector>
#include <image.h>

struct kernel_type
{
    std::string m_name;
    image_kernel m_kernel;
};

extern std::vector<kernel_type>  global_map_kernels;
extern std::vector<image_kernel> compile_kernels(int start_arg, int argc,const char * argv[]);