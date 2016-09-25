//  filesystem.cpp
//  HCube
//
//  Created by Gabriele on 07/07/16.
//  Copyright � 2016 Gabriele. All rights reserved.
//
#include <hcube/core/filesystem.h>
#include <sstream>
#include <cstdlib>
#ifdef _WIN32
	#include <windows.h>
	#include <direct.h>
	#include <io.h>
	#include <Shlwapi.h>
	#define getcwd _getcwd
	#define access _access
	#define F_OK 00
	#define R_OK 04
	#define W_OK 02
#else
	#include <sys/types.h>
    #include <sys/stat.h>
    #include <dirent.h>
	#include <unistd.h>
#endif

namespace hcube
{
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

		bool is_file(const std::string& filepath)
		{
			return  access(filepath.c_str(), F_OK) != -1 && !is_directory(filepath);
		}

		bool is_readable(const std::string& filepath)
		{
			return  access(filepath.c_str(), R_OK) != -1;
		}

		bool is_writable(const std::string& filepath)
		{
			return  access(filepath.c_str(), W_OK) != -1;
		}


		//utilities files
		std::string get_directory(const std::string& filepath)
		{
			//do it?
			if (is_directory(filepath)) return filepath;
			//get separetor
			auto separetor = filepath.find_last_of("\\/");
			//return sub path
			return filepath.substr(0, separetor);
		}

		std::string get_filename(const std::string& filepath)
		{
			//copy path
			std::string filename = filepath;
			//get separetor
			auto separetor = filename.find_last_of("\\/");
			//remove rest of path
			if (separetor != std::string::npos) filename.erase(0, separetor + 1);
			//return
			return filename;
		}

		std::string get_basename(const std::string& filepath)
		{
			//copy path
			std::string filename = get_filename(filepath);
			//get dot
			auto dot = filename.rfind(".");
			//extract
			if (dot != std::string::npos) filename.erase(dot);
			//return name
			return filename;
		}

		std::string get_extension(const std::string& filepath)
		{
			//copy path
			std::string filename = get_filename(filepath);
			//get dot
			auto dot = filename.rfind(".");
			//extract
			if (dot != std::string::npos) return filename.substr(dot);
			//return 0
			return "";
		}

		std::vector<char> file_read_all(const std::string& filepath)
		{
			std::vector<char> out;
			/////////////////////////////////////////////////////////////////////
			FILE* file = fopen(filepath.c_str(), "rb");
			//bad case
			if (!file) return out;
			/////////////////////////////////////////////////////////////////////
			std::fseek(file, 0, SEEK_END);
			size_t size = std::ftell(file);
			std::fseek(file, 0, SEEK_SET);
			/////////////////////////////////////////////////////////////////////
			out.resize(size, 0);
			std::fread(&out[0], size, 1, file);
			/////////////////////////////////////////////////////////////////////
			std::fclose(file);
			//return
			return out;
		}

		std::string text_file_read_all(const std::string &filepath)
		{
			std::string out;
			/////////////////////////////////////////////////////////////////////
			FILE* file = fopen(filepath.c_str(), "rb");
			//bad case
			if (!file) return "";
			/////////////////////////////////////////////////////////////////////
			std::fseek(file, 0, SEEK_END);
			size_t size = std::ftell(file);
			std::fseek(file, 0, SEEK_SET);
			/////////////////////////////////////////////////////////////////////
			out.resize(size);
			std::fread(&out[0], size, 1, file);
			/////////////////////////////////////////////////////////////////////
			std::fclose(file);
			//return
			return out;
		}


		path_operation get_fullpath(const std::string& relative)
		{
#ifdef _WIN32
			char fullpath[MAX_PATH];
			if (GetFullPathNameA(relative.c_str(), MAX_PATH, fullpath, 0) == 0) return path_operation{ false, "" };
			return path_operation{ true, std::string(fullpath) };
#else
			char realname[_POSIX_PATH_MAX];
			if (realpath(relative.c_str(), realname) == 0) return path_operation{ false, "" };
			return path_operation{ true, std::string(realname) };
#endif
		}

		std::vector<std::string> split(std::string str, const std::string& delimiter)
		{
			//temps
			size_t pos = 0;
			std::vector<std::string> tokens;
			//search
			while ((pos = str.find(delimiter)) != std::string::npos)
			{
				tokens.push_back(str.substr(0, pos));
				str.erase(0, pos + delimiter.length());
			}
			//add last
			if (str.size()) tokens.push_back(str);
			//return
			return tokens;
		}

		path_operation get_relative_to(const std::string& base, const std::string& path)
		{
			//get abs paths
			path_operation absolute_base = get_fullpath(base); if (!absolute_base.m_success) return path_operation{ false, "" };
			path_operation absolute_path = get_fullpath(path); if (!absolute_path.m_success) return path_operation{ false, "" };
#ifdef _WIN32
			char output_path[MAX_PATH];

			if (PathRelativePathToA(output_path,
				absolute_base.m_path.c_str(),
				FILE_ATTRIBUTE_DIRECTORY,
				absolute_path.m_path.c_str(),
				FILE_ATTRIBUTE_NORMAL) == 0) return path_operation{ false, "" };

			return path_operation{ true, output_path };
#else
			std::string output_path;

			//split
			std::string separetor("/");
			std::vector< std::string > base_directories = split(absolute_base.m_path, separetor);
			std::vector< std::string > path_directories = split(absolute_path.m_path, separetor);

			//max loop
			size_t max_size = std::min(base_directories.size(), path_directories.size());

			//start index
			size_t i = 0;

			//search dif
			for (i = 0; i != max_size; ++i)
			{
				if (base_directories[i] != path_directories[i]) break;
			}

			//back
			for (size_t j = 0; j < (max_size - i); ++i) output_path += "../";

			//next
			for (i = max_size; i < path_directories.size(); ++i)
			{
				output_path += path_directories[i] + "/";
			}

			//ok
			return path_operation{ true, output_path };
#endif
		}


#ifdef _WIN32
		bool is_directory(const std::string& directory)
		{
			DWORD ftyp = GetFileAttributesA(directory.c_str());
			if (ftyp == INVALID_FILE_ATTRIBUTES) return false;
			if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;
			return false;
		}

		files_list get_files(const std::string& directorypath)
		{
			//test directory
			if (!is_directory(directorypath)) return files_list{ false, std::vector<std::string>{} };
			//alloc output
			files_list output{ true, std::vector<std::string>{} };
			//redefine path
			std::string std_directorypath = directorypath + "/";
			std::string path_all = std_directorypath + "*";
			//attributes
			WIN32_FIND_DATAA ffd;
			//struct stat st;
			HANDLE hFind = FindFirstFileA(path_all.c_str(), &ffd);
			//read all
			do
			{
				if (!strcmp(ffd.cFileName, ".")) continue;
				if (!strcmp(ffd.cFileName, "..")) continue;
				//put into vector only files
				if (is_file(std_directorypath + ffd.cFileName))
				{
					output.m_fields.push_back(ffd.cFileName);
				}

			} while (FindNextFileA(hFind, &ffd) != 0);
			//return output
			return output;
		}

		directories_list get_sub_directories(const std::string& directorypath)
		{
			//test directory
			if (!is_directory(directorypath)) return directories_list{ false, std::vector<std::string>{} };
			//alloc output
			files_list output{ true, std::vector<std::string>{} };
			//redefine path
			std::string std_directorypath = directorypath + "/";
			std::string path_all = std_directorypath + "*";
			//attributes
			WIN32_FIND_DATAA ffd;
			//struct stat st;
			HANDLE hFind = FindFirstFileA(path_all.c_str(), &ffd);
			//read all
			do
			{
				if (!strcmp(ffd.cFileName, ".")) continue;
				if (!strcmp(ffd.cFileName, "..")) continue;
				//put into vector only dirs
				if (is_directory(std_directorypath + ffd.cFileName))
				{
					output.m_fields.push_back(ffd.cFileName);
				}

			} while (FindNextFileA(hFind, &ffd) != 0);
			//return output
			return output;
		}


#else
		bool is_directory(const std::string& directory)
		{
			struct stat st;
			if (stat(directory.c_str(), &st) == 0) return ((st.st_mode & S_IFDIR) != 0);
			return false;
		}

		files_list get_files(const std::string& directorypath)
		{
			//test directory
			if (!is_directory(directorypath)) return files_list{ false, std::vector<std::string>{} };
			//attributes
			DIR *directory;
			struct dirent *dirent;
			//open dir
			if ((directory = opendir(directorypath.c_str())) == NULL) return files_list{ false, std::vector<std::string>{} };
			//alloc output
			files_list output{ true, std::vector<std::string>{} };
			//redefine path
			std::string std_directorypath = directorypath + "/";
			//read all elements
			while ((dirent = readdir(directory)) != NULL)
			{
				if (!strcmp(dirent->d_name, ".")) continue;
				if (!strcmp(dirent->d_name, "..")) continue;
				//put into vector only files
				if (is_file(std_directorypath + dirent->d_name))
				{
					output.m_fields.push_back(dirent->d_name);
				}
			}
			//close directory
			closedir(directory);
			//return output
			return output;
		}

		directories_list get_sub_directories(const std::string& directorypath)
		{
			//test directory
			if (!is_directory(directorypath)) return directories_list{ false, std::vector<std::string>{} };
			//attributes
			DIR *directory;
			struct dirent *dirent;
			//open dir
			if ((directory = opendir(directorypath.c_str())) == NULL) return directories_list{ false, std::vector<std::string>{} };
			//alloc output
			directories_list output{ true, std::vector<std::string>{} };
			//redefine path
			std::string std_directorypath = directorypath + "/";
			//read all elements
			while ((dirent = readdir(directory)) != NULL)
			{
				if (!strcmp(dirent->d_name, ".")) continue;
				if (!strcmp(dirent->d_name, "..")) continue;
				//put into vector only dirs
				if (is_directory(std_directorypath + dirent->d_name))
				{
					output.m_fields.push_back(dirent->d_name);
				}
			}
			//close directory
			closedir(directory);
			//return output
			return output;
		}
#endif


	};
}