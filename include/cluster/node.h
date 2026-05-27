#ifndef __CLUSTER_NODE_H
#define __CLUSTER_NODE_H


#include <cluster/timing.h>

#include <stddef.h>


struct __job_t;
typedef struct __job_t job_t;


typedef uint64_t feature_mask_t;


#define NODE_FEATURE_CPU     ( 1UL << 0 )
#define NODE_FEATURE_CPU_VEC ( 1UL << 1 )
#define NODE_FEATURE_GPU     ( 1UL << 2 )
#define NODE_FEATURE_NPU     ( 1UL << 3 )
#define NODE_FEATURE_TPU     ( 1UL << 4 )
#define NODE_FEATURE_FPGA    ( 1UL << 5 )
#define NODE_FEATURE_ASIC    ( 1UL << 6 )


typedef struct __node_load_stats_t {
	float     average_cpu_load;
	float     average_memory_load;
	quantum_t average_stall_time;
} node_load_stats_t;


typedef struct __node_t {
	char *name;

	// время отклика влияет на то, когда узел готов принять задачу
	// от планировщика
	quantum_t response_time; 
	
#ifdef CONFIG_USE_TIME_VARIANCE
	quantum_t response_time_variance;
#endif

	// Количественные характеристики
	size_t mem_size;
	size_t cpu_count;
	size_t thread_count;

	float max_exec_rate, exec_rate;
	
	// функционал узла
	feature_mask_t features;

	// нагрузка
	node_load_stats_t load_stats;

	// кто и кем занят
	// т.е. в иерархическом порядке:
	// ядро процессора -> поток
	job_t **occupation;
} node_t;


#endif // ! __CLUSTER_NODE_H
