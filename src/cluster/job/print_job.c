#include <cluster/defs.h>
#include <cluster/job.h>
#include <cluster/node.h>

#include <stdio.h>


void
cluster_print_job( __IN__ const job_t *j )
{
	const char feature_string[ FEATURE_STR_SIZE ];

	cluster_get_feature_string( feature_string, j->required_features );
	
	
	_DEBUG_PRINTF( "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-( %s )-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"
				   "                Время исполнения:             %u " _TQ "\n"
				   "                Ожидаемое затраченное время:  %u " _TQ "\n"
				   "                Макс. допустимое время:       %u " _TQ "\n"
				   "                Время ожидания:               %u " _TQ "\n"
				   "                Макс. время ожидания:         %u " _TQ "\n"
				   "                Временная погрешность:        %u " _TQ "\n"
				   "-------------------------------------------------------------\n"
				   "                Память:                       %u " _MQ "\n"
				   "                Кол-во ядер:                  %u\n"
				   "                Кол-во потоков:               %u\n"
				   "                Приоритет:                    %u\n"
				   "                Требуемые фукнции: %s" "\n",
				   j->name, j->exec_time, j->estimated_time, j->max_exec_time,
				   j->wait_time, j->max_wait_time, j->time_variance,
				   j->mem_size, j->cpu_count, j->thread_count, j->priority,
				   feature_string );
}
