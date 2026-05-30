#include <cluster/node.h>

#include <cluster/defs.h>


void
cluster_print_node_status( __IN__ const node_t *n )
{
	_DEBUG_PRINTF( "  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=( %s )-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"
			 "        CPU: %u%%, Память: %u%%",
				   n->name,
				   n->load_stats.average_cpu_load * 100,
				   n->load_stats.memory_load * 100 );

	job_list_t *jlist = n->jobs;

	if ( ! jlist ) return;

	for ( ; jlist; jlist = jlist->next_on_node )
		cluster_print_job_truncated( &( jlist->job ) );
}
