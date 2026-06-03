#include <cluster/job.h>

#include <cluster/node.h>


quantum_t
cluster_get_total_time_for_job( __IN__ const job_t  *j,
								__IN__ const node_t *n )
{
	return j->estimated_time/n->exec_rate
		 + n->response_time
		 + n->ic->delay_max;
}
