//  filesystem.cpp
//  HCube
//
//  Created by Gabriele on 07/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <filesystem.h>
#include <cstdlib>
#ifdef _WIN32
	#include <windows.h>
	#include <direct.h>
	#include <io.h>
	#define getcwd _getcwd
	#define access _access
	#define F_OK 00
	#define R_OK 04
	#define W_OK 02
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

namespace filesystem
{

	std::string working_dir()
	{
		char buffer[255];
		char *answer = getcwd(buffer, sizeof(buffer));
		if (answer) return answer;
		return "";
	}

	std::string home_dir()
	{
		const char *home_dir = std::getenv("HOME");
		if (home_dir) return home_dir;
		else         return "";
	}

#ifdef _WIN32
	bool is_directory(const std::string& directory)
	{
		DWORD ftyp = GetFileAttributesA(directory.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES) return false;
		if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;
		return false;
	}
#else
	bool is_directory(const std::string& directory)
	{
		struct stat st;
		if (stat(directory.c_str(), &st) == 0) return (st.st_mode & S_IFDIR != 0);
		return false;
	}
#endif

	bool is_file(const std::string& filepath)
	{
		return  access(filepath.c_str(), F_OK) != -1;
	}

	bool is_readable(const std::string& filepath) 
	{
		return  access(filepath.c_str(), R_OK) != -1;
	}

	bool is_writable(const std::string& filepath)
	{
		return  access(filepath.c_str(), W_OK) != -1;
	}

};