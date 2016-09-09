//
//  main.cpp
//  texture_utilities
//
//  Created by Gabriele on 09/09/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//

#include <iostream>
#include <image.h>
#include <kernels.h>
#include <filesystem.h>

extern "C" int stbi_write_png(char const *filename, int w, int h, int comp, const void  *data, int stride_in_bytes);
extern std::shared_ptr<image> load_image(const std::string& image_path);

#define MESSAGE(x) std::cout << x << std::endl;
#define MESSATE_AND_EXIT(x, y){ std::cout << x << std::endl; return y; }

int main(int argc, const char * argv[])
{
    //show help
    if(   argc <= 1
       || std::string(argv[1]) == "--help"
       || std::string(argv[1]) == "-h")
    {
        std::cout << "texture_tools in_image_path out_image_path [options] [--kernels [kernels]]\n\n";
        std::cout << "options:\n"
                     "\t--flip-vertical\n"
                     "\t--flip-horizontal\n";
        std::cout << "\n";
        std::cout << "kernels:\n";
        for(auto& i_kernel : global_map_kernels)
        {
            std::cout<<"\t"<< i_kernel.m_name << "\n";
        }
        std::cout << "\n";
        return 0;
    }
    //only 2 args?
    if(argc <= 2) MESSATE_AND_EXIT("Too less arguments",-1)
    //get paths
    std::string
        in_path ( argv[1] ),
        out_path( argv[2] );
    //test in file
    if(!hcube::filesystem::is_file(in_path)) MESSATE_AND_EXIT( "Error: " << in_path <<  " isn't a file", -2 );
    //test out file
    if(hcube::filesystem::is_file(out_path)) MESSATE_AND_EXIT( "Error: " << out_path  << " olready exist", -3);
    //options
    bool do_flip_vertical = false;
    bool do_flip_horizontal = false;
    std::vector<image_kernel> do_kernels;
    //get args
    for(int i = 3; i < argc; ++i)
    {
        //to str
        std::string arg(argv[i]);
        //flip
        if(arg == "--flip-vertical")    do_flip_vertical   = true;
        if(arg == "--flip-horizontal")  do_flip_horizontal = true;
        //compile
        if(arg == "--kernels")          do_kernels = compile_kernels(i+1,argc, argv);
    }
    //load image
    auto in_image = load_image(in_path);
    //test image
    if(!in_image) MESSATE_AND_EXIT( "Error to open: " << in_path, -2 );
    //execute
    if(do_flip_vertical)   in_image->flip_vertical();
    if(do_flip_horizontal) in_image->flip_horizontal();
    //applay kernels
    for(auto& i_kernel : do_kernels) in_image->apply_kernel(i_kernel);
    //save output
    if(!stbi_write_png(out_path.c_str(),
                       (int)in_image->get_width(),
                       (int)in_image->get_height(),
                       4,
                       in_image->to_rgba().data(),
                       0)
    )
    MESSATE_AND_EXIT( "Error to save: " << out_path, -3 );
    //end
    return 0;
}
