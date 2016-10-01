//
//  file_to_open.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 06/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <hcube/core/native_dialog.h>
#include <vector>
#include <hcube/core/filesystem.h>

namespace hcube
{
	namespace native_dialog
	{

#if defined( __APPLE__ )
		extern open_file_output cocoa_open_file
		(
			GLFWwindow* window,
			const std::string& title,
			const std::string& path,
			const std::vector< std::string >& allowed_file_types
		);
		extern save_file_output cocoa_save_file
		(
			GLFWwindow* window,
			const std::string& title,
			const std::string& path,
			const std::vector< std::string >& allowed_file_types
		);
		#define xxxx_open_file cocoa_open_file
		#define xxxx_save_file cocoa_save_file
#elif defined( _WIN32 )
		extern open_file_output window_open_file
		(
			GLFWwindow* window,
			const std::string& title,
			const std::string& path,
			const std::vector< std::string >& allowed_file_types
		);
		extern save_file_output window_save_file
		(
			GLFWwindow* window,
			const std::string& title,
			const std::string& path,
			const std::vector< std::string >& allowed_file_types
		);
		#define xxxx_open_file window_open_file
		#define xxxx_save_file window_save_file
#else
#error OS not supported
#endif

		static inline std::string get_home_or_working_dir()
		{
			std::string home = filesystem::home_dir();
			if (home.size()) return home;
			else            return filesystem::working_dir();
		}

		open_file_output open_file_dialog
		(
			GLFWwindow* window,
			const std::string& title,
			const std::vector<std::string>& types
		)
		{
			return xxxx_open_file(window, title, get_home_or_working_dir(), types);
		}

		save_file_output save_file_dialog
		(
			GLFWwindow* window,
			const std::string& title,
			const std::vector<std::string>& types
		)
		{
			return xxxx_save_file(window, title, get_home_or_working_dir(), types);
		}

	};
}