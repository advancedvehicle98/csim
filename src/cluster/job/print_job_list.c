#include <cluster/defs.h>
#include <cluster/job.h>


void
cluster_print_job_list( __IN__ const job_list_t *jlist )
{
	const job_list_t *jl = jlist;
	
	for ( ; jl; jl = jl->next ) cluster_print_job( &( jl->job ) );
}
