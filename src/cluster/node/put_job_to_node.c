#include <cluster/node.h>

#include <cluster/interconnect.h>

#include <stdlib.h>


static __INLINE quantum_t _evaluate_time_before_start( const node_t *n );


// предполагается, что задачу действительно
// можно разместить на узле

void
cluster_put_job_to_node( __STATE__ job_list_t *jlist,
						 __STATE__ node_t     *n )
{
	job_t *j = &( jlist->job );

	if ( j->assigned_node ) return;
	
	j->assigned_node = n;
	// добавляем время, т.к. планировкщик перед этим
	// мог сам определить, через какое время должна запуститься задача
	j->time_before_start += _evaluate_time_before_start( n );

	n->occupation.mem -= j->mem_size;

	size_t cpu_count = j->cpu_count;
	size_t thread_count = j->thread_count;
	size_t c = 0;

	if ( ! j->cpu_count )
		while ( thread_count ) {
			size_t cpu_threads = n->occupation.cpu_threads[ c ];

			if ( thread_count <= cpu_threads ) {
				n->occupation.cpu_threads[ c ] -= thread_count;
				break;
			}

			n->occupation.cpu_threads[ c ] = 0;
			thread_count -= cpu_threads;

			++c;
		}
	else
		while ( cpu_count ) {
			while ( true ) {
				if ( thread_count <= n->occupation.cpu_threads[ c ] ) {
					n->occupation.cpu_threads[ c ] -= thread_count;
					break;
				}

				++c;
			}
			
			--cpu_count;
		}

	if ( ! n->jobs ) {
		jlist->prev_on_node = NULL;
		n->jobs = jlist;
		goto _put_job_to_node_continue;
	}

	job_list_t *t = n->jobs, *next = t->next_on_node;

	for ( ; next; t = next, next = next->next_on_node );

	t->next_on_node = jlist;
	jlist->prev_on_node = t;

 _put_job_to_node_continue:
	jlist->next_on_node = NULL;
}


quantum_t
_evaluate_time_before_start( const node_t *n )
{
	quantum_t t = n->response_time;

	quantum_t mn = n->ic->delay_min;
	quantum_t mx = n->ic->delay_max;

	if ( ! mx ) return t;
		
	t += ( rand() % ( mx - mn ) ) + mn;
	
	return t;
}
