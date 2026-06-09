#ifndef __CLUSTER_NODE_H
#define __CLUSTER_NODE_H


#include <cluster/defs.h>
#include <cluster/interconnect.h>
#include <cluster/job.h>
#include <cluster/timing.h>

#include <stddef.h>


#define MAX_NODE_NAME_SIZE 32
#define FEATURE_STR_SIZE 64


#define NODE_FEATURE_CPU     ( 1UL << 0 )
#define NODE_FEATURE_CPU_VEC ( 1UL << 1 )
#define NODE_FEATURE_GPU     ( 1UL << 2 )
#define NODE_FEATURE_NPU     ( 1UL << 3 )
#define NODE_FEATURE_TPU     ( 1UL << 4 )
#define NODE_FEATURE_FPGA    ( 1UL << 5 )
#define NODE_FEATURE_ASIC    ( 1UL << 6 )


inline bool _requires_cpu    ( const feature_mask_t m ) { return m & NODE_FEATURE_CPU; }
inline bool _requires_cpu_vec( const feature_mask_t m ) { return m & NODE_FEATURE_CPU_VEC; }
inline bool _requires_gpu    ( const feature_mask_t m ) { return m & NODE_FEATURE_GPU; }
inline bool _requires_npu    ( const feature_mask_t m ) { return m & NODE_FEATURE_NPU; }
inline bool _requires_tpu    ( const feature_mask_t m ) { return m & NODE_FEATURE_TPU; }
inline bool _requires_fpga   ( const feature_mask_t m ) { return m & NODE_FEATURE_FPGA; }
inline bool _requires_asic   ( const feature_mask_t m ) { return m & NODE_FEATURE_ASIC; }

#define _FEATURES_FMT "%s%s%s%s%s%s%s"


typedef struct _node_load_stats_t {
	float average_cpu_load;
	float memory_load;
	
	quantum_t node_stall_time;
} node_load_stats_t;


// не смотря на название стурктуры, она как
// раз показывает, сколько ресурсов СВОБОДНО
typedef struct _node_occupation_t {
	size_t mem, *cpu_threads;
} node_occupation_t;


// для резервации места на узлах
typedef struct _job_reservation_t {
	quantum_t   time_before_assign;
	job_list_t *job_entry;
	struct _job_reservation_t *next, *prev;
} job_reservation_t;


typedef struct _node_t {
	char name[ MAX_NODE_NAME_SIZE ];

	// время отклика влияет на то, когда узел готов принять задачу
	// от планировщика
	quantum_t response_time; 
	
#ifdef CONFIG_USE_TIME_VARIANCE
	quantum_t response_time_variance;
#endif

	// куда узел подключен
	struct _interconnect_t *ic;

	// Количественные характеристики
	size_t mem_size;
	size_t cpu_count;
	size_t thread_count;

	// возможно потом можно сделать какое-то замедление.
	// единственном видом замедления на данный момент
	// является замедление задачи через сокращение кол-ва потоков
	// вызовом cluster_fit_job().
	float max_exec_rate, exec_rate;
	
	// функционал узла
	feature_mask_t features;
	char feature_str[ FEATURE_STR_SIZE ];

	// нагрузка
	node_load_stats_t load_stats;

	// доступность ресурсов
	node_occupation_t occupation;

	// господин просит больше указателей
	job_list_t *jobs;

	// зарезервированные задачи
	job_reservation_t *reservations;
} node_t;


// для планировщиков
typedef struct _node_list_t {
	node_t              *node;
	struct _node_list_t *next;
} node_list_t;


bool cluster_check_node_for_job( __IN__ const job_t  *j,
								 __IN__ const node_t *n );

bool cluster_check_node_for_job_as_if_empty( __IN__ const job_t  *j,
											 __IN__ const node_t *n );

void cluster_get_feature_string( __OUT__       char           str[ FEATURE_STR_SIZE ],
								 __IN__  const feature_mask_t mask );

void cluster_print_node_status( __IN__ const node_t *n );

void cluster_put_job_to_node( __STATE__ job_list_t *jlist,
							  __STATE__ node_t     *n );

void cluster_remove_job_from_node( __STATE__ job_list_t *jlist );

void cluster_reserve_job_on_node( __STATE__ job_list_t *jlist,
								  __STATE__ node_t     *n,
								  __IN__    quantum_t   t );

void cluster_update_node( __STATE__ node_t *n,
						  __STATE__ statistics_t *s );


#endif // ! __CLUSTER_NODE_H
