//
//  main.cpp
//  OGLHCubeView
//
//  Created by Gabriele on 28/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//

#include <iostream>
#include <application.h>
#include <app_basic.h>

int main(int argc, const char * argv[])
{
    hcube::application app;
	hcube::app_basic view_points;
#if 1
	return app.execute(
		  hcube::window_size_percentage{ { 80., 80. } }//window size
		, hcube::window_mode::NOT_RESIZABLE            //mode
		, 4									           //OpenGL version
		, 1									           //OpenGL sub-version
		, "HCUBE DEMO"					               //window title
		, new hcube::app_basic) 					   //instance
		? 0 : 1;
#elif 0
	hcube::window_info a;
	return app.execute(
		  hcube::window_size_percentage{ { 100., 100. } }//window size
		, hcube::window_mode::FULLSCREEN                 //mode
		, 4									             //OpenGL version
        , 1									             //OpenGL sub-version
        , "HCUBE DEMO"					               //window title
		, new hcube::app_basic) 					     //instance
		? 0 : 1;
#else
    hcube::window_info a;
    return app.execute(  hcube::window_size_percentage{ { 80., 80. } }   //window size
                       , hcube::window_mode::FULLSCREEN                  //mode
                       , 4									             //OpenGL version
                       , 1									             //OpenGL sub-version
                       , "HCUBE DEMO"					                 //window title
                       , new hcube::app_basic) 					         //instance
                       ? 0 : 1;
#endif
}
