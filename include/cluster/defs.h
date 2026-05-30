#ifndef __CLUSTER_DEFS_H
#define __CLUSTER_DEFS_H


#include <cluster/config.h>

#include <stdint.h>


#define __OUT__
#define __IN__
#define __STATE__


#if defined( CONFIG_DEBUG ) || defined ( CONFIG_WITH_DEBUG_PRINT )

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


#define _STR_EQUAL( L, R ) ( strcmp( L, R ) == 0 )


#ifdef CONFIG_QUANTUM_TIME_RESOLUTION_PS
#   define Q2SEC_M 1000000000000
#   define _TQ "пс."
#elifdef CONFIG_QUANTUM_TIME_RESOLUTION_NS
#   define Q2SEC_M 1000000000
#   define _TQ "нс."
#elifdef CONFIG_QUANTUM_TIME_RESOLUTION_US
#   define Q2SEC_M 1000000
#   define _TQ "мкс."
#elifdef CONFIG_QUANTUM_TIME_RESOLUTION_MS
#   define Q2SEC_M 1000
#   define _TQ "мс."
#else
#   define Q2SEC_M 1
#   define _TQ "c."
#endif

#define Q2SEC( q ) ( (float) q/ Q2SEC_M )


#ifdef CONFIG_QUANTUM_MEMORY_RESOLUTION_B
#   define _MQ "Б."
#elifdef CONFIG_QUANTUM_MEMORY_RESOLUTION_KB
#   define _MQ "КБ."
#elifdef CONFIG_QUANTUM_MEMORY_RESOLUTION_MB
#   define _MQ "МБ."
#else
#   define _MQ "ГБ."
#endif


typedef uint64_t feature_mask_t;


#endif // __CLUSTER_DEFS_H
