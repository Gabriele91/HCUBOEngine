//
//  OpenGL4.h
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once

#ifdef __APPLE__
    #include <OpenGL/gl3.h>
    #include <OpenGL/glu.h>
#else
    #ifdef _WIN32
        #include <windows.h>
    #endif
	#include <GL/glew.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#ifdef _DEBUG
	#define _OPENGL_PRINT_DEBUG_ \
	{\
		GLenum gl_err = GL_NO_ERROR;\
		while ((gl_err = glGetError()) != GL_NO_ERROR)\
			std::cout << "OpenGL error: " << __LINE__<< " : " << gl_err << " : " << gluErrorString(gl_err) << std::endl;\
	}
#else
	#define _OPENGL_PRINT_DEBUG_
#endif