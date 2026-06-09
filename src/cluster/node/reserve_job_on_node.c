#include <cluster/node.h>

#include <stdlib.h>


void
cluster_reserve_job_on_node( __STATE__ job_list_t *jlist,
							 __STATE__ node_t     *n,
							 __IN__    quantum_t   t )
{
	job_reservation_t *jres = malloc( sizeof( job_reservation_t ) );

	jres->job_entry = jlist;
	jres->time_before_assign = t;
	jres->prev = NULL;
		
	if ( ! n->reservations ) {
		jres->next = NULL;
		n->reservations = jres;
		return;
	}

	jres->next = n->reservations;
	n->reservations->prev = jres;
}
