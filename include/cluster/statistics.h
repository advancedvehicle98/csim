#ifndef __CLUSTER_STATISTICS_H
#define __CLUSTER_STATISTICS_H


#include <cluster/timing.h>

#include <stddef.h>
#include <stdint.h>


typedef struct __statistics_t {
	quantum_t average_stall_time;
	quantum_t average_wait_time;
	quantum_t average_exec_time;

	quantum_t total_active_time_quantum;
	uint64_t  total_active_time_ms;

	size_t jobs_done;     // кол-во выполненных задач (неважно с ошибкой или без)
	size_t jobs_complete; // кол-во _успешно_ завершённых задач

	float total_average_cpu_load;
	float total_average_memory_load;
} statistics_t;


#endif // ! __CLUSTER_STATISTICS_H
