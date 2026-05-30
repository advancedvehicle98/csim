#include <cluster/node.h>

#include <cluster/interconnect.h>

#include <stdlib.h>


static quantum_t _evaluate_time_before_start( const node_t *n );


// предполагается, что задачу действительно
// можно разместить на узле

void
cluster_put_job_to_node( __STATE__ job_t  *j,
						 __STATE__ node_t *n )
{
	j->assigned_node = n;
	j->time_before_start = _evaluate_time_before_start( n );

	n->occupation.mem -= j->mem_size;

	size_t cpu_count = j->cpu_count;
	size_t thread_count = j->thread_count;
	size_t c = 0;

	if ( ! j->cpu_count )
		while ( thread_count ) {
			size_t cpu_threads = n->occupation.cpu_threads[ c ];

			if ( thread_count <= cpu_threads ) {
				n->occupation.cpu_threads[ c ] -= thread_count;
				return;
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
}


quantum_t
_evaluate_time_before_start( const node_t *n )
{
	quantum_t t = n->response_time;

	switch ( n->ic->type ) {
		
	case IC_TYPE_BUS: {
		quantum_t mn = n->ic->delay_min;
		quantum_t mx = n->ic->delay_max;
		
		t += ( rand() % ( mx - mn ) ) + mn;
	} break;
		
	}
	
	return t;
}
