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
typedef void      ( *schedule_func_t )( scheduler_t *, node_t* *, size_t );
typedef void      ( *print_func_t )( scheduler_t * );


typedef struct _scheduler_t {
	schedule_func_t   schedule;
	init_func_t       init;
	distribute_func_t distribute;
	destroy_func_t    destroy;
	print_func_t      print;

	void *state;
} scheduler_t;


#endif // ! __CLUSTER_SCHEDULER_H
