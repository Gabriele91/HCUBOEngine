//
//  file_to_open.h
//  OGLHCubeView
//
//  Created by Gabriele on 06/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <string>
#include <GLFW/glfw3.h>

namespace native_dialog
{
    
    struct open_file_output
    {
        bool        m_success;
        std::string m_path;
    };

	std::string working_dir();
	std::string home_dir();
	open_file_output open_file_dialog(GLFWwindow* window);
};