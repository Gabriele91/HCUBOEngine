//
//  resource.h
//  HCubo
//
//  Created by Gabriele on 12/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <memory>

class resource{};
using resource_ptr  = std::shared_ptr< resource >;
using resource_uptr = std::unique_ptr< resource >;
using resource_wptr = std::weak_ptr< resource >;