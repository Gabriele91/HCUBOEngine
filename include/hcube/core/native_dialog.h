//
//  file_to_open.h
//  OGLHCubeView
//
//  Created by Gabriele on 06/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include <hcube/config.h>

namespace hcube
{

	namespace native_dialog
	{

		struct HCUBE_API open_file_output
		{
			bool        m_success;
			std::string m_path;
		};

		struct HCUBE_API save_file_output
		{
			bool        m_success;
			std::string m_path;
		};

		HCUBE_API  open_file_output open_file_dialog(GLFWwindow* window,
											         const std::string& title,
													 const std::vector<std::string>& types);
		HCUBE_API  save_file_output save_file_dialog(GLFWwindow* window,
													 const std::string& title,
													 const std::vector<std::string>& types);
	};
}