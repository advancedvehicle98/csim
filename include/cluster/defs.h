#ifndef __CLUSTER_DEFS_H
#define __CLUSTER_DEFS_H


#include <cluster/config.h>


#define __OUT__
#define __IN__
#define __STATE__


#ifdef CONFIG_DEBUG

#   include <stdio.h>

#   define _DEBUG_PRINTF( FMT, ... ) \
	printf( "[***] " FMT "\n", __VA_ARGS__ )

#   define _DEBUG_PUTS( STR ) puts( "[***] " STR )

#else
#   define _DEBUG_PRINTF( FMT, ... ) ""
#   define _DEBUG_PUTS( STR ) ""
#endif

#define _ERROR_PRINTF( FMT, ... ) \
	printf( "[!!!] " FMT "\n", __VA_ARGS__ )

#define _ERROR_PUTS( STR ) puts( "[!!!] " STR )


#define _IS_ERROR( EXPR ) ( ( EXPR ) != 0 )


#define strtos( S, E, B ) strtol( S, E, B )


#endif // __CLUSTER_DEFS_H
