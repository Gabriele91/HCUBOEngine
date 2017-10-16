//
//  time.h
//  MPI_Project
//
//  Created by Gabriele on 14/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <hcube/config.h>

namespace hcube
{
	namespace query_performance
	{
		//get time
		HCUBE_API long long get_ticks();
		//time
		HCUBE_API double get_time();
		//time
		HCUBE_API double get_time_ms();
	}
}