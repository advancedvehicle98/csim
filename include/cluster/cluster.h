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

#include <stddef.h>
#include <stdint.h>


typedef struct __cluster_t {
	interconnect_t              connections;
	// хз почему то job_moment_list_t он не переваривает
	struct __job_moment_list_t *job_sequence;
	scheduler_t                *scheduler;
	statistics_t                statistics;
	
	node_t                     *node_registry;
	size_t                      node_count;
} cluster_t;


uint32_t cluster_init( __STATE__       cluster_t   *c,
					   __IN__    const char        *cf,
					   __IN__    const char        *jf,
					   __IN__          scheduler_t *s );

void cluster_destroy( __STATE__ cluster_t *c );

uint32_t cluster_simulate( __STATE__ cluster_t *c );

uint32_t cluster_simulation_step( __STATE__ cluster_t *c );


#endif // ! __CLUSTER_H
