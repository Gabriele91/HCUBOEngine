//
//  filesystem.h
//  HCube
//
//  Created by Gabriele on 07/07/16.
//  Copyright � 2016 Gabriele. All rights reserved.
//
#pragma once
#include <string>
#include <vector>
#include <hcube/config.h>

namespace hcube
{

	namespace filesystem
	{
		HCUBE_API std::string working_dir();
		HCUBE_API std::string home_dir();

		HCUBE_API bool is_directory(const std::string& directorypath);
		HCUBE_API bool is_file(const std::string& filepath);
		HCUBE_API bool is_readable(const std::string& filepath);
		HCUBE_API bool is_writable(const std::string& filepath);
		//utilities files
		HCUBE_API std::string get_directory(const std::string& filepath);
		HCUBE_API std::string get_filename(const std::string& filepath);
		HCUBE_API std::string get_basename(const std::string& filepath);
		HCUBE_API std::string get_extension(const std::string& filepath);
		HCUBE_API std::vector<char> file_read_all(const std::string& filepath);
		HCUBE_API std::string text_file_read_all(const std::string& filepath);

		//utilities path
		struct HCUBE_API path_operation
		{
			bool m_success;
			std::string m_path;
		};
		HCUBE_API path_operation get_fullpath(const std::string& relative);
		HCUBE_API path_operation get_relative_to(const std::string& base, const std::string& path);
		//utilities directories
		struct HCUBE_API folder_list
		{
			bool m_success;
			std::vector < std::string > m_fields;
		};
		using files_list = folder_list;
		using directories_list = folder_list;

		HCUBE_API files_list get_files(const std::string& directorypath);
		HCUBE_API directories_list get_sub_directories(const std::string& directorypath);
	};
}