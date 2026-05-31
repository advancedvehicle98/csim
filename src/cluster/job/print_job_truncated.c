#include <cluster/job.h>

#include <cluster/defs.h>



void
cluster_print_job_truncated( __IN__ const job_t *j )
{
	if ( ! j->assigned_node ) {
		_DEBUG_PRINTF( "    %s : ожидание: %u/%u " _TQ " | %u " _TQ " | %u потоков | %u " _MQ " памяти | приоритет: %u",
					   j->name, j->wait_time, j->max_wait_time,
					   j->estimated_time,
					   j->cpu_count ? j->cpu_count * j->thread_count : j->thread_count,
					   j->mem_size, j->priority );
		return;
	}
	
	if ( j->time_before_start ) {
		_DEBUG_PRINTF( "    %s : время до начала: %u " _TQ, j->name, j->time_before_start );
		return;
	}
	
	_DEBUG_PRINTF( "    %s : %u " _TQ " / %u " _TQ " | %u потоков | %u " _MQ " памяти | приоритет: %u",
				   j->name, j->exec_time, j->estimated_time,
				   j->cpu_count ? j->cpu_count * j->thread_count : j->thread_count,
				   j->mem_size, j->priority );
}
