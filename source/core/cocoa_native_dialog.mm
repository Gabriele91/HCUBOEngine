//
//  cocoa_open_file.m
//  OGLHCubeView
//
//  Created by Gabriele on 07/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <hcube/core/native_dialog.h>
#include <string>
#include <vector>
#include <assert.h>

namespace hcube
{
	namespace native_dialog
	{
    
		open_file_output cocoa_open_file(GLFWwindow* window,
										 const std::string& title,
										 const std::string& path,
										 const std::vector< std::string >& allowed_file_types)
		{
			assert(allowed_file_types.size() % 2 == 0);
			//output data
			open_file_output output{ false , "" };
			//output ref
			open_file_output& ref_output = output;
			//autorelease
			@autoreleasepool
			{
				//ns array
				id ns_allowed_file_types = [NSMutableArray new];
				//remove *. and append
				for (size_t i = 0; i != allowed_file_types.size(); ++i)
				{
					if (i & 0x1)
					{
						//cpp ref
						const std::string& cpp_str = allowed_file_types[i];
						//c ref
						const char* c_str = cpp_str.c_str();
						//search
						size_t sub_find = cpp_str.find("*.");
						//find?
						if(sub_find != std::string::npos)  c_str = &cpp_str[ sub_find + 2 ];
						//append
						[ns_allowed_file_types addObject:[NSString stringWithUTF8String:c_str]];
					}
				}
				// Path url
				NSURL* path_url = [NSURL URLWithString:[NSString stringWithUTF8String:path.c_str()]];
				// Init panel
				NSOpenPanel *panel = [NSOpenPanel openPanel];
				// Configure your panel the way you want it
				[panel setTitle:[NSString stringWithUTF8String:title.c_str()]];
				[panel setCanChooseFiles:YES];
				[panel setCanChooseDirectories:NO];
				[panel setAllowsMultipleSelection:NO];
				[panel setDirectoryURL:path_url];
				[panel setAllowedFileTypes:ns_allowed_file_types];
				[panel setLevel:CGShieldingWindowLevel()];
				if ([panel runModal]==NSFileHandlingPanelOKButton)
				{
					for (NSURL *file_url in [panel URLs])
					{
						ref_output = open_file_output{ true, [file_url.path UTF8String] };
					}
				}
			}
			//restore focus
			glfwFocusWindow(window);
			//return
			return output;
		}
        
        save_file_output cocoa_save_file(GLFWwindow* window,
                                         const std::string& title,
                                         const std::string& path,
                                         const std::vector< std::string >& allowed_file_types)
        {
            assert(allowed_file_types.size() % 2 == 0);
            //output data
            save_file_output output{ false , "" };
            //output ref
            save_file_output& ref_output = output;
            //autorelease
            @autoreleasepool
            {
                //ns array
                id ns_allowed_file_types = [NSMutableArray new];
                //remove *. and append
                for (size_t i = 0; i != allowed_file_types.size(); ++i)
                {
                    if (i & 0x1)
                    {
                        //cpp ref
                        const std::string& cpp_str = allowed_file_types[i];
                        //c ref
                        const char* c_str = cpp_str.c_str();
                        //search
                        size_t sub_find = cpp_str.find("*.");
                        //find?
                        if(sub_find != std::string::npos)  c_str = &cpp_str[ sub_find + 2 ];
                        //append
                        [ns_allowed_file_types addObject:[NSString stringWithUTF8String:c_str]];
                    }
                }
                // Path url
                NSURL* path_url = [NSURL URLWithString:[NSString stringWithUTF8String:path.c_str()]];
                // Init panel
                NSSavePanel *panel = [NSSavePanel savePanel];
                // Configure your panel the way you want it
                [panel setTitle:[NSString stringWithUTF8String:title.c_str()]];
                [panel setAllowsOtherFileTypes:YES];
                [panel setExtensionHidden:YES];
                [panel setCanCreateDirectories:YES];
                [panel setDirectoryURL:path_url];
                [panel setAllowedFileTypes:ns_allowed_file_types];
                [panel setLevel:CGShieldingWindowLevel()];
                if ([panel runModal]==NSFileHandlingPanelOKButton)
                {
                    ref_output = save_file_output{ true, [[panel URL].path UTF8String] };
                }
            }
            //restore focus
            glfwFocusWindow(window);
            //return
            return output;
        }
	}
}
