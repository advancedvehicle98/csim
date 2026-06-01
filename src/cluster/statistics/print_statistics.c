#include <cluster/statistics.h>


void
cluster_print_statistics( __IN__ const statistics_t *s )
{
	_DEBUG_PRINTF( "  Общее время                  : %llu.%09lu с. (%u сим. " _TQ ")"
		   "\n        Среднее время простоя узлов  : %f сим. " _TQ " (%.0f%% от общего времени)"
		   "\n        Среднее время ожидания       : %f сим. " _TQ " (%.0f%% от общего времени)"
		   "\n        Средняя нагрузка на ядра     : %.0f%%"
		   "\n        Среднее использование памяти : %.0f%%"
		   "\n\n +++ Успешно выполненных задач : %u из %u",
				   s->total_active_time_sec, s->total_active_time_nsec, s->total_active_time_quantum,
				   s->average_stall_time, s->average_stall_time/s->total_active_time_quantum * 100,
				   s->average_wait_time, s->average_wait_time/s->total_active_time_quantum * 100,
				   s->average_cpu_load * 100,
				   s->average_memory_load * 100,
				   s->jobs_complete, s->jobs_done );
}
