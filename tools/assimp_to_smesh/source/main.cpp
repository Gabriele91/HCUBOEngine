#include <iostream>
#include <smesh.h>
#include <filesystem.h>
extern void process_model(const std::string& path, model& out_model);

int main(int argc,char* argv[])
{
	if(argc<3) return 1;
	//file exists?
	if (!filesystem::is_readable(argv[1]))
	{
		std::cout << "Can't read: " << argv[1] << std::endl;
		return 1;
	}
	//alloc
	model l_model;
	//load
	process_model(argv[1], l_model);
	//save
	l_model.save(argv[2]);
	//end
	return 0;
}