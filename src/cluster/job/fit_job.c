#include <cluster/job.h>


// Сокращает/увеличивает число потоков на задаче, но увеличивает время исполнения

void
cluster_fit_job( __STATE__       job_t  *j,
				 __IN__    const size_t  t )
{
	size_t nominal_thread_count = j->thread_count;

	if ( j->cpu_count ) nominal_thread_count *= j->cpu_count;

	j->cpu_count = 0;
	j->thread_count = t;

	j->estimated_time *= nominal_thread_count/t;
}
