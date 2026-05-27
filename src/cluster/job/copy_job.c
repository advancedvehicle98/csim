#include <cluster/job.h>

#include <assert.h>
#include <string.h>


void
cluster_copy_job( __OUT__       job_t *dest,
				  __IN__  const job_t *src )
{
	assert( dest );
	assert( src );
	assert( memcpy( dest, src, sizeof( job_t ) ) == dest );
}
