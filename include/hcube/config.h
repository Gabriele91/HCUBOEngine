#pragma once

#if defined(HCUBE_LIBRARY_EXPORT) 
	// inside DLL
	#define HCUBE_API   __declspec(dllexport)
	#define HCUBE_RENDER_API   __declspec(dllexport)
#elif defined(HCUBE_LIBRARY_IMPORT) 
	// outside DLL
	#define HCUBE_API   __declspec(dllimport)
	#define HCUBE_RENDER_API   __declspec(dllimport)
#else 
	// static
	#define HCUBE_API
	#define HCUBE_RENDER_API
#endif