#include <cluster/job.h>

#include <cluster/node.h>


// возвращает true, если задача завершилась
bool
cluster_update_job( __STATE__ job_t        *j,
					__STATE__ statistics_t *s )
{
	if ( j->is_done ) return true;

	// мы уже не берём в учет задачи, которые слишком долго ждали
	if ( ! j->assigned_node ) {
		if ( ++( j->wait_time ) == j->max_wait_time )
			goto __job_done;

		return false;
	}

	// учёт задержки
	bool job_started = j->time_before_start == 0;

	if ( ! job_started ) {
		if ( ++( j->wait_time ) == j->max_wait_time )
			goto __job_done;
		
		--( j->time_before_start );
		return false;
	}

	node_t *n = j->assigned_node;
	
	j->exec_time += (quantum_t) n->exec_rate;
	
	if ( j->exec_time >= j->estimated_time ) {
		j->is_complete = true;
		++( s->jobs_complete );
		goto __job_done;
	}

	return false;

 __job_done:
	j->is_done = true;
	++( s->jobs_done );
	return true;
}
