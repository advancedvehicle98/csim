#ifndef __CLUSTER_JOB_QUEUE
#define __CLUSTER_JOB_QUEUE


#include <cluster/job.h>

#include <stddef.h>


typedef struct __job_queue_t {
	char *name;
	
	size_t job_count;

	// информацией здесь оперирует планировщик
	// поля здесь отличаются от планировщика к планировщику
	void *specs;
} job_queue_t;


#endif // ! __CLUSTER_JOB_QUEUE
