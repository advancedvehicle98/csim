#include <schedule_algorithms.h>

#include <stddef.h>
#include <stdlib.h>


static node_t *_pick_node( __IN__ const job_t   *j,
						   __IN__       node_t* *nodes,
						   __IN__ const size_t   n_count );


void
schedule_fifo( __STATE__ scheduler_t *s,
			   __IN__    node_t*     *nodes,
			   __IN__    size_t       n_count )
{
	fifo_queue_t *q = (fifo_queue_t *) s->state;
	job_list_t *c = q->c;

	// пропускаем задачи, которые уже готовы
	while ( c && c->job.is_done ) c = c->next;

	// чтобы в след. раз не проходиться по сделанным задачам
	q->c = c;
	
	while ( c ) {
		job_t *j = &c->job;

		// вдруг попалось среди неготовых задач
		if ( j->is_done ) goto _schedule_fifo_continue;

		node_t *n = _pick_node( j, nodes, n_count );

		if ( ! n ) goto _schedule_fifo_continue;

		cluster_put_job_to_node( j, n );

_schedule_fifo_continue:
		c = c->next;
	}
}


uint32_t
init_fifo( __STATE__ scheduler_t *s )
{
	// грязную работу делает distribute_fifo
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
	free( s->state );
}


node_t *
_pick_node( __IN__ const job_t   *j,
			__IN__       node_t* *nodes,
			__IN__ const size_t   n_count )
{
	for ( int i = 0; i < n_count; ++i )
		if ( cluster_check_node_for_job( j, nodes[ i ] ) )
			return nodes[ i ];

	return NULL;
}
