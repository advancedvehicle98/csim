#include <cluster/cluster.h>

#include <stdio.h>



typedef struct __parse_file_thread_arg_t {
	      uint32_t  *s;
	      cluster_t *c;
	const char      *f;
}  __attribute__(( packed )) _parse_file_thread_arg_t ;


uint32_t _cluster_parse_file_wrapper( __STATE__       cluster_t   *c,
									  __IN__    const char        *f,
									  __IN__          uint32_t ( *parse_lines )( cluster_t *, FILE * ) );
