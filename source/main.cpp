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
    application app;
	app_basic view_points;
	return app.execute(
		window_size_percentage{ { 90., 90. } } //window size
		, false								   //resizable
		, 4									   //OpenGL version
		, 1									   //OpenGL sub-version
		, "view particles"					   //window title
		, &view_points) 					   //instance
		? 0 : 1;
}
