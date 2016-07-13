//  filesystem.cpp
//  HCube
//
//  Created by Gabriele on 07/07/16.
//  Copyright � 2016 Gabriele. All rights reserved.
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
    #include <dirent.h>
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
    std::string get_filename(const std::string& filepath)
    {
        //copy path
        std::string filename = filepath;
        //get separetor
        auto separetor  = filename.find_last_of("\\/");
        //remove rest of path
        if(separetor != std::string::npos) filename.erase(0, separetor + 1);
        //return
        return filename;
    }
    
    std::string get_basename(const std::string& filepath)
    {
        //copy path
        std::string filename = get_filename(filepath);
        //get dot
        auto dot  = filename.rfind(".");
        //extract
        if(dot != std::string::npos) filename.erase(dot);
        //return name
        return filename;
    }
    
    std::string get_extension(const std::string& filepath)
    {
        //copy path
        std::string filename = get_filename(filepath);
        //get dot
        auto dot  = filename.rfind(".");
        //extract
        if(dot != std::string::npos) return filename.substr(dot);
        //return 0
        return "";
    }
    
    std::vector<char> file_read_all(const std::string& filepath)
    {
        std::vector<char> out;
        /////////////////////////////////////////////////////////////////////
        FILE* file = fopen(filepath.c_str(),"rb");
        //bad case
        if(!file) return out;
        /////////////////////////////////////////////////////////////////////
        std::fseek(file, 0, SEEK_END);
        size_t size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        /////////////////////////////////////////////////////////////////////
        out.resize(size,0);
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
        FILE* file = fopen(filepath.c_str(),"rb");
        //bad case
        if(!file) return "";
        /////////////////////////////////////////////////////////////////////
        std::fseek(file, 0, SEEK_END);
        size_t size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        /////////////////////////////////////////////////////////////////////
        out.resize(size+1,'\0');
        std::fread(&out[0], size, 1, file);
        /////////////////////////////////////////////////////////////////////
        std::fclose(file);
        //return
        return out;
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
        if(!is_directory(directorypath)) return files_list{ false, std::vector<std::string>{} };
        //alloc output
        list_files files_list{ true, std::vector<std::string>{} };
        //redefine path
        std::string std_directorypath=directorypath+"/";
        std::string path_all=std_directorypath+"*";
        //attributes
        WIN32_FIND_DATA ffd;
        //struct stat st;
        HANDLE hFind = FindFirstFile(path_all.c_str(), &ffd);
        //read all
        do
        {
            if ( !strcmp( ffd.cFileName, "."   )) continue;
            if ( !strcmp( ffd.cFileName, ".."  )) continue;
            //put into vector only files
            if(is_file(std_directorypath+dirent->d_name))
            {
                output.m_fields.push_back(dirent->d_name);
            }
            
        }
        while (FindNextFile(hFind, &ffd) != 0);
        //return output
        return output;
    }
    
    directories_list get_sub_directories(const std::string& directorypath)
    {
        //test directory
        if(!is_directory(directorypath)) return directories_list{ false, std::vector<std::string>{} };
        //alloc output
        directories_list files_list{ true, std::vector<std::string>{} };
        //redefine path
        std::string std_directorypath=directorypath+"/";
        std::string path_all=std_directorypath+"*";
        //attributes
        WIN32_FIND_DATA ffd;
        //struct stat st;
        HANDLE hFind = FindFirstFile(path_all.c_str(), &ffd);
        //read all
        do
        {
            if ( !strcmp( ffd.cFileName, "."   )) continue;
            if ( !strcmp( ffd.cFileName, ".."  )) continue;
            //put into vector only dirs
            if(is_directory(std_directorypath+dirent->d_name))
            {
                output.m_fields.push_back(dirent->d_name);
            }
            
        }
        while (FindNextFile(hFind, &ffd) != 0);
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
        if(!is_directory(directorypath)) return files_list{ false, std::vector<std::string>{} };
        //attributes
        DIR *directory;
        struct dirent *dirent;
        //open dir
        if((directory = opendir(directorypath.c_str())) == NULL) return files_list{ false, std::vector<std::string>{} };
        //alloc output
        files_list output{ true, std::vector<std::string>{} };
        //redefine path
        std::string std_directorypath=directorypath+"/";
        //read all elements
        while ((dirent = readdir(directory)) != NULL)
        {
            if ( !strcmp( dirent->d_name, "."   )) continue;
            if ( !strcmp( dirent->d_name, ".."  )) continue;
            //put into vector only files
            if(is_file(std_directorypath+dirent->d_name))
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
        if(!is_directory(directorypath)) return directories_list{ false, std::vector<std::string>{} };
        //attributes
        DIR *directory;
        struct dirent *dirent;
        //open dir
        if((directory = opendir(directorypath.c_str())) == NULL) return directories_list{ false, std::vector<std::string>{} };
        //alloc output
        directories_list output{ true, std::vector<std::string>{} };
        //redefine path
        std::string std_directorypath=directorypath+"/";
        //read all elements
        while ((dirent = readdir(directory)) != NULL)
        {
            if ( !strcmp( dirent->d_name, "."   )) continue;
            if ( !strcmp( dirent->d_name, ".."  )) continue;
            //put into vector only dirs
            if(is_directory(std_directorypath+dirent->d_name))
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