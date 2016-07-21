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

namespace filesystem
{
	std::string working_dir();
	std::string home_dir();
    
	bool is_directory(const std::string& directorypath);
	bool is_file(const std::string& filepath);
    bool is_readable(const std::string& filepath);
    bool is_writable(const std::string& filepath);
    //utilities files
	std::string get_directory(const std::string& filepath);
    std::string get_filename(const std::string& filepath);
    std::string get_basename(const std::string& filepath);
    std::string get_extension(const std::string& filepath);
    std::vector<char> file_read_all(const std::string& filepath);
    std::string text_file_read_all(const std::string& filepath);
    
    //utilities path
    struct path_operation
    {
        bool m_success;
        std::string m_path;
    };
    path_operation get_fullpath(const std::string& relative);
    path_operation get_relative_to(const std::string& base,const std::string& path);
    //utilities directories
    struct folder_list
    {
        bool m_success;
        std::vector < std::string > m_fields;
    };
    using files_list       = folder_list;
    using directories_list = folder_list;
    
    files_list get_files(const std::string& directorypath);
    directories_list get_sub_directories(const std::string& directorypath);
};