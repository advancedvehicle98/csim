#ifndef __CLUSTER_STATISTICS_H
#define __CLUSTER_STATISTICS_H


#include <cluster/timing.h>

#include <stddef.h>
#include <stdint.h>


// sc - это sample count
// штобы не шибко длинно было

typedef struct __statistics_t {
	// среднее время простоя узлов
	float average_stall_time;
	size_t average_stall_time_sc;

	// среднее время ожидания задачи
	// рассчитывается после симуляции
	float average_wait_time;

	// средняя нагрузка на одно ядро узла
	float average_cpu_load;
	size_t average_cpu_load_sc;

	// средняя занятось памяти
	float average_memory_load;
	size_t average_memory_load_sc;

	quantum_t total_active_time_quantum;
	
	size_t jobs_done;     // кол-во выполненных задач (неважно с ошибкой или без)
	size_t jobs_complete; // кол-во _успешно_ завершённых задач
} statistics_t;


#endif // ! __CLUSTER_STATISTICS_H
