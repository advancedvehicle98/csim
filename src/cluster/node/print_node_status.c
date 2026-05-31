#include <cluster/node.h>

#include <cluster/defs.h>


void
cluster_print_node_status( __IN__ const node_t *n )
{
	_DEBUG_PRINTF( "\n        =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=( %s )-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"
			       "\n        CPU: %.0f%% (%u ядер по %u потоков), Память: %.0f%% (%u " _MQ ")"
				   "\n        %s"
				   "\n        ------------------------------------------------------------------",
				   n->name,
				   n->load_stats.average_cpu_load * 100,
				   n->cpu_count, n->thread_count,
				   n->load_stats.memory_load * 100,
				   n->mem_size, n->feature_str );

	job_list_t *jlist = n->jobs;

	if ( ! jlist ) {
		_DEBUG_PRINTF( "        Простой (%u " _TQ")",
					   n->load_stats.node_stall_time );
		return;
	}

	for ( ; jlist; jlist = jlist->next_on_node )
		cluster_print_job_truncated( &( jlist->job ) );
}
