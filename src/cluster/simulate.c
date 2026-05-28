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
		_DEBUG_PRINTF( "Время: %d q =============================="
					   "==========================================", t );

		// для динамической планировки
		if ( jseq && t == jseq->moment.time ) {
			_DEBUG_PUTS( "\tНовые задачи:" );
			// здесь уже s становится владельцем jseq->moment.job_list_head
			// т.е. s берёт на себя ответственность за очистку
			// памяти от задач
			s->distribute( s, jseq->moment.job_list_head );
			jseq = jseq->next;
		}

		s->schedule( s, node_registry, node_count );
				
		// ---- ДЛЯ ДЕБАГА ---------------
		c->statistics.jobs_done = c->job_count;
		// ----------------------
		
		++t;
	}
	
	return EXIT_SUCCESS;
}
