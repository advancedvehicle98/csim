#include <cluster/statistics.h>


void
cluster_print_statistics( __IN__ const statistics_t *s )
{
	_DEBUG_PRINTF( "  Общее время                  : %llu.%09lu с. (%u сим. " _TQ ")"
		   "\n        Среднее время простоя узлов  : %f сим. " _TQ
		   "\n        Среднее время ожидания       : %f сим. " _TQ
		   "\n        Средняя нагрузка на ядра     : %f%%"
		   "\n        Среднее использование памяти : %f%%"
		   "\n\n +++ Успешно выполненных задач : %u из %u",
				   s->total_active_time_sec, s->total_active_time_nsec,
				   s->average_stall_time,
				   s->total_active_time_quantum,
				   s->average_wait_time,
				   s->average_cpu_load * 100,
				   s->average_memory_load * 100,
				   s->jobs_complete, s->jobs_done );
}
