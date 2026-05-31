#include <cluster/cluster.h>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>


static void _finalize_statistics( __STATE__       cluster_t       *c,
								  __IN__    const quantum_t        t,
								  __IN__    const struct timespec *dts );


uint32_t
cluster_simulate( __STATE__ cluster_t *c )
{
	_DEBUG_PUTS( "+++ Начало симуляции +++" );

	quantum_t t = 0;
	struct timespec ts_start, ts_end;
	
	job_moment_list_t   *jseq          =  c->job_sequence;
	interconnect_list_t *cons          =  c->connections;
	scheduler_t         *s             =  c->scheduler;
	statistics_t        *stats         = &c->statistics;
	node_t*             *node_registry =  c->node_registry;
	size_t               node_count    =  c->node_count;

	clock_gettime( CLOCK_REALTIME, &ts_start );
	
	while ( c->statistics.jobs_done < c->job_count ) {
#ifdef CONFIG_OUTPUT_FRAME_BY_FRAME
		puts( "\033[H\033[J" );
#endif
		
		_DEBUG_PRINTF( "===========================================================================\n"
				 "      -------------------------( Время: %d " _TQ " )---------------------------------\n"
				 "      ===========================================================================\n", t );

		// для динамической планировки
		if ( jseq && t == jseq->moment.time ) {
			job_list_t *new_jlist = jseq->moment.job_list_head;
			
			_DEBUG_PUTS( "\tНовые задачи:" );
			cluster_print_job_list( new_jlist );
			
			s->distribute( s, new_jlist );
			jseq = jseq->next;
		}

		s->schedule( s, node_registry, node_count );

		cluster_simulation_step( c, t );

		if ( s->print ) s->print( s );

		_DEBUG_PUTS( "\n\n-------------------------------------------------------------------\n"
			   "                                УЗЛЫ\n"
			   "-------------------------------------------------------------------" );
		
		for ( int n = 0; n < node_count; ++n )
			cluster_print_node_status( node_registry[ n ] );

#ifdef CONFIG_OUTPUT_MANUAL
		getc( stdin );
		
#elifdef CONFIG_OUTPUT_AUTO

#   ifndef CONFIG_OUTPUT_AUTO_PAUSE_TIME_US
#       define CONFIG_OUTPUT_AUTO_PAUSE_TIME_US 10000
#   endif

		usleep( CONFIG_OUTPUT_AUTO_PAUSE_TIME_US );
		
#endif
		
		// ВРЕМЯ ОБНОВЛЯЕТСЯ ТОЛЬКО В САМОМ КОНЦЕ
		++t;
	}

	clock_gettime( CLOCK_REALTIME, &ts_end );

	ts_end.tv_sec  -= ts_start.tv_sec;
	ts_end.tv_nsec -= ts_start.tv_nsec;

	_finalize_statistics( c, t, &ts_end );

	_DEBUG_PUTS( "\n\n===========================================================================\n"
					 "-------------------------( Результат )-------------------------------------\n"
					 "===========================================================================" );
	cluster_print_statistics( &( c->statistics ) );
	
	return EXIT_SUCCESS;
}


void
_finalize_statistics( __STATE__       cluster_t       *c,
					  __IN__    const quantum_t        t,
					  __IN__    const struct timespec *dts )
{
	c->statistics.total_active_time_quantum = t;

	float average_wait_time = 0;
	job_moment_list_t *jseq = c->job_sequence;

	for ( ; jseq; jseq = jseq->next ) {
		job_list_t *jlist = jseq->moment.job_list_head;
		
		for ( ; jlist; jlist = jlist->_next ) 
			average_wait_time += (float) jlist->job.wait_time;
	}

	average_wait_time /= (float) c->job_count;
	c->statistics.average_wait_time = average_wait_time;

	c->statistics.total_active_time_sec  = dts->tv_sec;
	c->statistics.total_active_time_nsec = dts->tv_nsec;

	// у некоторых узлов ещё надо узнать время простоя
	size_t sc = c->statistics.average_stall_time_sc;
	size_t diff = 0;
	
	float stall_time = 0;

	for ( size_t n = 0; n < c->node_count; ++n ) {
		size_t st = c->node_registry[ n ]->load_stats.node_stall_time;
		
		if ( st ) {
			stall_time += (float) st;
			++diff;
		}
	}

	c->statistics.average_stall_time = c->statistics.average_stall_time * sc/( sc + diff )
		                             + stall_time/( sc + diff );
}
