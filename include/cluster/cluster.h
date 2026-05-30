#ifndef __CLUSTER_H
#define __CLUSTER_H


#include <cluster/config.h>
#include <cluster/defs.h>
#include <cluster/interconnect.h>
#include <cluster/job.h>
#include <cluster/job_queue.h>
#include <cluster/job_sequence.h>
#include <cluster/node.h>
#include <cluster/scheduler.h>
#include <cluster/statistics.h>
#include <cluster/timing.h>

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>


typedef struct _cluster_t {
	struct _interconnect_list_t *connections;
	struct _job_moment_list_t   *job_sequence;
	struct _scheduler_t         *scheduler;
	statistics_t                 statistics;
	
	node_t* *node_registry;
	size_t   node_count, job_count;
} cluster_t;


uint32_t cluster_init( __STATE__       cluster_t   *c,
					   __IN__    const char        *cf,
					   __IN__    const char        *jf,
					   __IN__          scheduler_t *s );

void cluster_destroy( __STATE__ cluster_t *c );

uint32_t cluster_simulate( __STATE__ cluster_t *c );

uint32_t cluster_simulation_step( __STATE__ cluster_t *c, __IN__ const quantum_t t );

// тематически эта функция подходит к разделу interconnect
// но чтобы не было возни с заголовками пока лежит тут
uint32_t cluster_node_network_from_dataset( __STATE__       cluster_t *c,
											__IN__    const char      *f );

void cluster_node_network_from_dataset_start( __OUT__         uint32_t  *s,
											  __OUT__         pthread_t *p,
											  __STATE__       cluster_t *c,
											  __IN__    const char      *f );


#endif // ! __CLUSTER_H
