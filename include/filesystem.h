#pragma once
//
//  filesystem.h
//  HCube
//
//  Created by Gabriele on 07/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <string>

namespace filesystem
{
	std::string working_dir();
	std::string home_dir();
	bool is_directory(const std::string& directory);
	bool is_file(const std::string& filepath);
	bool is_readable(const std::string& filepath);
	bool is_writable(const std::string& filepath);
};