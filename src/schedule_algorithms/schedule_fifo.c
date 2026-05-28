#include <schedule_algorithms.h>

#include <stddef.h>
#include <stdlib.h>


job_t *_fetch( __STATE__ fifo_queue_t *q );


void
schedule_fifo( __STATE__ scheduler_t *s,
			   __IN__    node_t*     *n,
			   __IN__    size_t       n_count )
{

}


uint32_t
init_fifo( __STATE__ scheduler_t *s )
{
	s->state = NULL;
	return EXIT_SUCCESS;
}


void
distribute_fifo( __STATE__ scheduler_t *s,
				 __STATE__ job_list_t  *jlist )
{
	fifo_queue_t *q;
	
	if ( ! s->state ) {
		q = (fifo_queue_t *) malloc( sizeof( fifo_queue_t ) );
	    s->state = (void *) q;

		q->h = q->c = jlist;

		goto _distribute_get_tail;
	}

	q = (fifo_queue_t *) s->state;

	q->t->next = jlist;

	if ( ! q->c ) q->c = jlist;

_distribute_get_tail:
	job_list_t *t = jlist;
	for ( ; t->next; t = t->next );
		
	q->t = t;
}


void
destroy_fifo( __IN__ scheduler_t *s )
{
	fifo_queue_t *q = (fifo_queue_t *) s->state;
	job_list_t *h = q->h, *n = h->next;
	
	for ( ; n; h = n, n = h->next ) free( h );

	free( h );
	free( s->state );
}


job_t *
_fetch( __STATE__ fifo_queue_t *q )
{
	if ( ! q->c ) return NULL;
	
	job_t *j = &( q->c->job );

	q->c = q->c->next;

	return j;
}
