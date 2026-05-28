#include <cluster/cluster.h>

#include <stdio.h>


uint32_t _cluster_parse_file_wrapper( __STATE__       cluster_t   *c,
									  __IN__    const char        *f,
									  __IN__          uint32_t ( *parse_lines )( cluster_t *, FILE * ) );
