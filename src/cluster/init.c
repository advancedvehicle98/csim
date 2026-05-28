#include <cluster/cluster.h>

#include <stdlib.h>


void _reset_statistics( statistics_t *s );


uint32_t cluster_init( __STATE__       cluster_t   *c,
					   __IN__    const char        *jf,
					   __IN__    const char        *cf,
					   __IN__          scheduler_t *s )
{
	if ( ! s || ! s->schedule || ! s->init || ! s->fetch ) {
		_ERROR_PUTS( "В scheduler_t не заполнены все поля" );
		return EXIT_FAILURE;
	}
	
	uint32_t status;

	c->scheduler = s;

	c->job_sequence = NULL;
	status = cluster_job_sequence_from_dataset( c, jf );
	
	if ( _IS_ERROR( status ) ) {
		_ERROR_PUTS( "Не удалось распарсить датасет задач" );
		return EXIT_FAILURE;
	}
	
	_reset_statistics( &( c->statistics ) );

	_DEBUG_PUTS( "Кластер инициализирован" );
	
	return EXIT_SUCCESS;
}


void
_reset_statistics( statistics_t *s )
{
	s->average_stall_time        =
	s->average_wait_time         =
	s->average_exec_time         =
	s->total_active_time_quantum = 
	s->total_active_time_ms      = 0;

	s->jobs_done = 0;

	s->total_average_cpu_load    = 
	s->total_average_memory_load = 0;		
}
