#include <cluster/cluster.h>

#include <stdlib.h>


uint32_t
cluster_simulate( __STATE__ cluster_t *c )
{
	_DEBUG_PUTS( "+++ Начало симуляции +++" );

	quantum_t t = 0;
	
	job_moment_list_t   *jseq          =  c->job_sequence;
	interconnect_list_t *cons          =  c->connections;
	scheduler_t         *s             =  c->scheduler;
	statistics_t        *stats         = &c->statistics;
	node_t*             *node_registry =  c->node_registry;
	size_t               node_count    =  c->node_count;

	while ( c->statistics.jobs_done < c->job_count ) {
		_DEBUG_PRINTF( "\n\n===========================================================================\n"
					       "-------------------------( Время: %d " _TQ " )-----------------------------------\n"
					       "===========================================================================\n", t );

		// для динамической планировки
		if ( jseq && t == jseq->moment.time ) {
			job_list_t *new_jlist = jseq->moment.job_list_head;
			
			_DEBUG_PUTS( "\tНовые задачи:" );
			cluster_print_job_list( new_jlist );
			
			s->distribute( s, new_jlist );
			jseq = jseq->next;
		}

		s->schedule( s, node_registry, node_count );

		cluster_simulation_step( c, t++ );

		_DEBUG_PUTS( "-------------------------------------------------------------------\n"
			   "                                      УЗЛЫ\n"
			   "      -------------------------------------------------------------------" );
		
		for ( int n = 0; n < node_count; ++n )
			cluster_print_node_status( node_registry[ n ] );
	}
	
	return EXIT_SUCCESS;
}
