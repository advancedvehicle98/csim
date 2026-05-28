#ifndef __CLUSTER_SCHEDULER_H
#define __CLUSTER_SCHEDULER_H


#include <cluster/job.h>
#include <cluster/node.h>

#include <stddef.h>


struct _scheduler_t;
typedef struct _scheduler_t scheduler_t;

typedef void      ( *destroy_func_t )( scheduler_t * );
typedef void      ( *distribute_func_t )( scheduler_t *, job_list_t * );
typedef uint32_t  ( *init_func_t )( scheduler_t * );
typedef void      ( *schedule_func_t )( scheduler_t *, node_t **, size_t );


typedef struct _scheduler_t {
	schedule_func_t   schedule;
	init_func_t       init;
	distribute_func_t distribute;
	destroy_func_t    destroy;

	void *state;
} scheduler_t;


void cluster_place_jobs_to_queue( __STATE__ scheduler_t *s,
								  __IN__    job_list_t  *jlist );

job_list_t *cluster_schedule( __STATE__       scheduler_t *s,
							  __IN__          node_t*     *node_registry,
							  __IN__    const size_t       node_count );


#endif // ! __CLUSTER_SCHEDULER_H
