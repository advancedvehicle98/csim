#include <cluster/job_sequence.h>

#include <cluster/defs.h>
#include <cluster/internal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static uint32_t _parse_lines( __STATE__ cluster_t  *c, __IN__ FILE *f );


uint32_t
cluster_job_sequence_from_dataset( __STATE__ cluster_t  *c,
								   __IN__    const char *f )
{
	c->job_sequence = NULL;
	c->job_count = 0;
	
	return _cluster_parse_file_wrapper( c, f, _parse_lines );
}


typedef enum {
	_JOB_MOMENT,
	_JOB_NAME,
	_JOB_ESTIMATED_TIME,
	_JOB_MAX_EXEC_TIME,
	_JOB_MAX_WAIT_TIME,
	_JOB_TIME_VARIANCE,
	_JOB_MEM_SIZE,
	_JOB_CPU_COUNT,
	_JOB_THREAD_COUNT,
	_JOB_PRIORITY,
	_JOB_REQUIRED_FEATURES,
	_JOB_TERM_COUNT_PER_LINE
} _job_term_t;

uint32_t
_parse_lines( __STATE__ cluster_t  *c,
			  __IN__    FILE       *f )
{
	char stack_line[ MAX_JOB_FILE_LINE_SIZE ];
	
	while ( true ) {
		char *line_start = fgets( stack_line, MAX_JOB_FILE_LINE_SIZE, f );
		
		if ( ! line_start ) break;

		char *line_end = strchr( stack_line, '\n' );

		_job_term_t term = _JOB_MOMENT;
		quantum_t   moment;
		job_t       job;

		quantum_t *job_quantum_ps[] = {
			&job.estimated_time,
			&job.max_exec_time,
			&job.max_wait_time,
			&job.time_variance
		};

		size_t *job_size_ps[] = {
			&job.mem_size,
			&job.cpu_count,
			&job.thread_count
		};

		while ( term < _JOB_TERM_COUNT_PER_LINE ) {
			char *next_space = strchr( line_start, ' ' );

			if ( next_space ) *next_space = '\0';

			switch ( term ) {
				
			case _JOB_MOMENT : {
				moment = strtoq( line_start, &next_space, 10 );
			} break;
				
			case _JOB_NAME : {
				strcpy( ( char * restrict ) &job.name, line_start );
				_DEBUG_PRINTF( "Парсим %s...", job.name );
			} break;
				
			case _JOB_ESTIMATED_TIME :
			case _JOB_MAX_EXEC_TIME :
			case _JOB_MAX_WAIT_TIME :
			case _JOB_TIME_VARIANCE : {
				*( job_quantum_ps[ term-_JOB_ESTIMATED_TIME ] ) = strtoq( line_start,
																		  &next_space,
																		  10 );
			} break;
				
			case _JOB_MEM_SIZE :	
			case _JOB_CPU_COUNT :
			case _JOB_THREAD_COUNT : {
				*( job_size_ps[ term-_JOB_MEM_SIZE ] ) = strtos( line_start,
																 &next_space,
																 10 );
			} break;
				
			case _JOB_PRIORITY : {
				job.priority = strtol( line_start,
									   &next_space,
									   10 );
			} break;
				
			case _JOB_REQUIRED_FEATURES : {
				job.required_features = strtoll( line_start,
												 &next_space,
												 10 );
			} break;
				
			}

			line_start = next_space + 1;
			++term;
		}

		job.exec_time = job.wait_time = job.time_before_start = 0;
		job.assigned_node = NULL;
		job.is_done = job.is_complete = false;
		
		++( c->job_count );

		cluster_add_job_moment( c, &job, moment );
	}

	return EXIT_SUCCESS;
}


static void *
_start_thread( void *args )
{
	_parse_file_thread_arg_t *a = (_parse_file_thread_arg_t *) args;
	*( a->s ) = cluster_job_sequence_from_dataset( a->c, a->f );
	return NULL;
}

void
cluster_job_sequence_from_dataset_start( __OUT__         uint32_t  *s,
										 __OUT__         pthread_t *p,
										 __STATE__       cluster_t *c,
										 __IN__    const char      *f )
{
	_parse_file_thread_arg_t args = {
		.s = s,
		.c = c,
		.f = f
	};

	pthread_create( p, NULL, _start_thread, (void *) &args );
}
