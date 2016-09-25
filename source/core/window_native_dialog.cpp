//
//  window_open_file.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 07/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <hcube/core/native_dialog.h>
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <string>
#include <vector>
#include <assert.h>


namespace hcube
{
	namespace native_dialog
	{

		open_file_output window_open_file(GLFWwindow* window,
										  const std::string& title,
										  const std::string& path,
										  const std::vector< std::string >& allowed_file_types)
		{

			assert(allowed_file_types.size() % 2 == 0);
			//alloc win32 string
			std::string allowed_file_types_win32;
			//compute string
			for (size_t i = 0; i != allowed_file_types.size(); ++i)
			{
				if ((i + 1) & 0x1)
					allowed_file_types_win32 += allowed_file_types[i] + '\0';
				else
					allowed_file_types_win32 += allowed_file_types[i] + '\0';
			}

			OPENFILENAMEA save_dialog;
			ZeroMemory(&save_dialog, sizeof(OPENFILENAMEA));
			char output_file_path[MAX_PATH] = "";
			save_dialog.lStructSize = sizeof(OPENFILENAMEA);
			save_dialog.hwndOwner = glfwGetWin32Window(window);
			save_dialog.lpstrFilter = allowed_file_types_win32.c_str();
			save_dialog.lpstrTitle = title.c_str();
			save_dialog.lpstrFile = (char*)output_file_path;
			save_dialog.nMaxFile = MAX_PATH;
			save_dialog.Flags = OFN_EXPLORER |
				OFN_PATHMUSTEXIST |
				OFN_HIDEREADONLY |
				OFN_NOCHANGEDIR |
				OFN_OVERWRITEPROMPT;
			//execute
			if (GetOpenFileNameA(&save_dialog))
			{
				return  open_file_output{ true, output_file_path };
			}
			//fail
			return open_file_output{ false , "" };
		}

	};
}