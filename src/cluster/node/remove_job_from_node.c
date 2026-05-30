#include <cluster/node.h>


static void _free_node_resources( __STATE__       node_t *n,
								  __IN__    const job_t  *j );


void
cluster_remove_job_from_node( __STATE__ job_list_t *jlist )
{
	node_t *node = jlist->job.assigned_node;

	if ( ! node ) return;
	
	if ( ! jlist->prev_on_node ) {
		node->jobs = jlist->next_on_node;
		goto _remove_job_from_node_fin;
	}
	
	// да наверное можно обойтись без ещё одного указателя
	// на предыдущий элемент списка, передавая его через аргумент
	// (т.к. всё равно задачи обрабатываются по порядку как правило)
	// но пока так
	job_list_t *p = jlist->prev_on_node;
	job_list_t *n = jlist->next_on_node;

	p->next_on_node = n;
	
	if ( n ) n->prev_on_node = p;

_remove_job_from_node_fin:
	jlist->job.assigned_node = NULL;

	_free_node_resources( node, &( jlist->job ) );
}


void
_free_node_resources( __STATE__       node_t *n,
					  __IN__    const job_t  *j )
{
	n->occupation.mem += j->mem_size;

	size_t c = 0;
	size_t job_cpu_count = j->cpu_count;
	size_t job_thread_count = j->thread_count;
	
	if ( job_cpu_count ) {
		while ( job_cpu_count-- )
			for ( ; c < n->cpu_count; ++c )
				if ( n->occupation.cpu_threads[ c ]
					 + job_thread_count <= n->thread_count ) {
					n->occupation.cpu_threads[ c++ ] += job_thread_count;
					break;
				}
	}
	else
		while ( job_thread_count ) {
			size_t t = n->thread_count - n->occupation.cpu_threads[ c ];

			if ( job_thread_count <= t ) {
				n->occupation.cpu_threads[ c ] += job_thread_count;
				break;
			}

			size_t diff = job_thread_count - t;
			
			job_thread_count -= diff;
			n->occupation.cpu_threads[ c ] += diff;
			
			++c;
		}
}
