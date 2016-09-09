#pragma once

#if defined( _MSC_VER )
	#define FORCEINLINE __forceinline
	#define PACKED( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
	#define ALIGNED( size, __Declaration__ ) __declspec(align(size)) __Declaration__
#else
	#define DFORCEINLINE __attribute__ ((always_inline))
	#define PACKED( __Declaration__ ) __Declaration__ __attribute__((__packed__))
	#define ALIGNED( size, __Declaration__ ) __Declaration__ __attribute__ ((aligned(size)))
#endif