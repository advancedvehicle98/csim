#ifndef __CLUSTER_SCHEDULER_H
#define __CLUSTER_SCHEDULER_H


#include <cluster/cluster.h>

#include <stddef.h>


struct _cluster_t;

typedef uint32_t ( *schedule_func_t )( struct _cluster_t * );
typedef uint32_t ( *init_func_t )( job_queue_t * );
typedef job_t *( *fetch_func_t )( job_queue_t * );


typedef struct _scheduler_t {
	schedule_func_t schedule;
	init_func_t     init;
	fetch_func_t    fetch;

	size_t       queue_count;
	job_queue_t *queues;
} scheduler_t;


#endif // ! __CLUSTER_SCHEDULER_H
