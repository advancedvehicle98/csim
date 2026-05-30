#ifndef __CLUSTER_JOB_H
#define __CLUSTER_JOB_H


#include <cluster/defs.h>
#include <cluster/statistics.h>
#include <cluster/timing.h>

#include <stdbool.h>
#include <stddef.h>


#define MAX_JOB_FILE_LINE_SIZE 200
#define MAX_JOB_NAME_SIZE      64


typedef uint32_t priority_t;

struct _node_t;
typedef struct _node_t node_t;


// Похоже типо на SLURM

// планировщик может только читать поля
// напрямую он ничего менять не может

typedef struct __job_t {
	char name[ MAX_JOB_NAME_SIZE ];
	
	// Ограничения по времени
	quantum_t max_exec_time, estimated_time, max_wait_time;
	quantum_t time_before_start; // задержка перед началом выполнения
	quantum_t exec_time, wait_time;

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

	bool is_done, is_complete;
	node_t *assigned_node;
} job_t;


typedef struct __job_list_t {
	// пришлось зделать по два указателя для симулятора
	// и для планировщика, т.к. не охота чтобы именно планировщик
	// рулил последовательностью задач и изменял её, мешая симулятору
	// обновлять прогресс у задач
	struct __job_list_t *_next;  // для симулятора
	struct __job_list_t *next;   // для планировщика
	struct __job_list_t *next_on_node; // для узла (для логов по крайней мере)
	struct __job_list_t *prev_on_node; // да задолбали меня эти указатели
	
	job_t                job;
} job_list_t;


void cluster_copy_job( __OUT__ job_t *dest, __IN__ const job_t *src );

void cluster_print_job( __IN__ const job_t* j );

void cluster_print_job_list( __IN__ const job_list_t *jlist );

void cluster_print_job_truncated( __IN__ const job_t *j );

bool cluster_update_job( __STATE__ job_list_t   *jlist,
						 __STATE__ statistics_t *s );


#endif // ! __CLUSTER_JOB_H
