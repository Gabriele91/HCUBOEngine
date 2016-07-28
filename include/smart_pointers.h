//
//  smart_pointers.h
//  OGLHCubeView
//
//  Created by Gabriele on 02/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <memory>
template < class T >
class smart_pointers : public std::enable_shared_from_this< T >
{
public:
    
    using ptr  = std::shared_ptr< T >;
    using uptr = std::unique_ptr< T >;
    using wptr = std::weak_ptr< T >;
    
    template < class... Args >
    static ptr snew(Args&&... args)
    {
        return std::make_shared< T >( args... );
    }
    
};