#ifndef __CLUSTER_JOB_SEQUENCER
#define __CLUSTER_JOB_SEQUENCER


#include <cluster/cluster.h>

#include <stddef.h>
#include <stdint.h>


struct _cluster_t;
typedef struct _cluster_t cluster_t;


// для динамически назначаемых задач
typedef struct _job_moment_t {
	quantum_t   time;
	job_list_t *job_list_head;
} job_moment_t;

typedef struct _job_moment_list_t {
	job_moment_t               moment;
	struct _job_moment_list_t *next;
} job_moment_list_t;


// Инициалиирует секвенсор из файла-датасета
// --------------------------------------------------------
uint32_t cluster_job_sequence_from_dataset( __STATE__ cluster_t  *c,
											__IN__    const char *f );

void cluster_add_job_moment( __STATE__       cluster_t *c,
							 __IN__    const job_t     *j,
							 __IN__    const quantum_t  q );


#endif // ! __CLUSTER_JOB_SEQUENCER
