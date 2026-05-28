#ifndef __CLUSTER_JOB_H
#define __CLUSTER_JOB_H


#include <cluster/defs.h>
#include <cluster/node.h>
#include <cluster/timing.h>

#include <stdbool.h>
#include <stddef.h>


#define MAX_JOB_FILE_LINE_SIZE 200
#define MAX_JOB_NAME_SIZE      64


typedef uint32_t priority_t;


// Похоже типо на SLURM

typedef struct __job_t {
	char name[ MAX_JOB_NAME_SIZE ];
	
	// Ограничения по времени
	quantum_t max_exec_time, estimated_time, max_wait_time;
	quantum_t time_before_start; // задержка перед началом выполнения

#ifdef CONFIG_USE_TIME_VARIANCE
	quantum_t time_variance; // отклонение от estimated_time
#endif

	// Ограничения по ресурсам
	size_t mem_size;
	size_t cpu_count;
	size_t thread_count;

	feature_mask_t required_features;

	// Доп. информация
	priority_t priority; // чем меньше значение, тем больше приоритет

	bool is_finished;
	node_t *assigned_node;
} job_t;


typedef struct __job_list_t {
	struct __job_list_t *next;
	job_t                job;
} job_list_t;


void cluster_copy_job( __OUT__ job_t *dest, __IN__ const job_t *src );


#endif // ! __CLUSTER_JOB_H
