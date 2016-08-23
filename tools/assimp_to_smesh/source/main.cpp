#include <iostream>
#include <smesh.h>
#include <filesystem.h>
extern void process_model(const std::string& path, model& out_model);
extern bool build_obb_model(const model& in_model, model& out_model);
#include <obb.h>

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
	//compute obb
	l_model.compute_obbs();
	//save
#if 1
	if (argc == 3) l_model.save(argv[2]);
	else           l_model.save(argv[2], argv[3]);
#else
	/* debug */
	model obb_model;
	build_obb_model(l_model, obb_model);
	obb_model.compute_obbs();
	if (argc == 3) obb_model.save(argv[2]);
	else           obb_model.save(argv[2], argv[3]);
#endif
	//end
	return 0;
}