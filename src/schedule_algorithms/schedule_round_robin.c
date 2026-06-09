#include <schedule_algorithms.h>

#include <stdlib.h>


static quantum_t _get_min_time_to_free( __IN__ node_list_t *n );

static rr_job_ll_t *_move_job_to_queue( __STATE__ rr_job_ll_t* *new_queue,
										__STATE__ rr_job_ll_t* *old_queue,
										__STATE__ rr_job_ll_t  *jll );

static __INLINE node_t *_pick_node( __IN__ const job_t   *j,
									__IN__       node_t* *nodes,
									__IN__ const size_t   n_count );

static rr_job_ll_t *_place_by_priority( __STATE__ rr_job_ll_t *jll,
										__STATE__ rr_job_ll_t *j );

static rr_job_ll_t *_place_by_wait_time( __STATE__ rr_job_ll_t *jll,
										 __STATE__ rr_job_ll_t *j );


void
schedule_round_robin( __STATE__       scheduler_t *s,
					  __IN__          node_t*     *nodes,
					  __IN__    const size_t       n_count )
{	
	rr_job_queues_t *queues = s->state;
	
	rr_job_ll_t *finished = queues->finished;
	rr_job_ll_t *running  = queues->running;
	rr_job_ll_t *pending  = queues->pending;

	rr_job_ll_t *r = running, *next = r->next;

	for ( ; r; r = next, next = next->next ? next->next : NULL ) {
		if ( --r->stop_in ) continue;

		// не убираем задачу с узла, если не велика вероятность того,
		// что она успеет выполнится в противном случае
		quantum_t wait_time_left = r->entry->job.max_wait_time
			                     - r->entry->job.wait_time;
		
		if ( wait_time_left < ROUND_ROBIN_TIME_TO_RUN ) continue;

		if ( r->entry->job.is_done ) {
			_move_job_to_queue( &finished, &running, r );
			continue;
		}

		_move_job_to_queue( &pending, &running, r );
	}

	rr_job_ll_t *p = pending;
	next = p->next;
	
	for ( ; p; p = next, next = next->next ? next->next : NULL ) {
		if ( p->entry->job.is_done ) {
			_move_job_to_queue( &finished, &pending, p );
			continue;
		}

		node_t *n = _pick_node( &( p->entry->job ), nodes, n_count );
		if ( ! n ) continue;

		cluster_put_job_to_node( p->entry, n );
		p->stop_in = ROUND_ROBIN_TIME_TO_RUN;
		_move_job_to_queue( &running, &pending, p );
	}

	queues->finished = finished;
	queues->running  = running;
	queues->pending  = pending;
}


uint32_t
init_round_robin( __STATE__       scheduler_t *s,
				  __IN__          node_t*     *n,
				  __IN__    const size_t       n_count )
{
	rr_job_queues_t *queues = s->state;

	queues = malloc( sizeof( rr_job_queues_t ) );
		
	queues->running  =
	queues->pending  = 
	queues->finished = NULL;

	s->state = queues;

	return EXIT_SUCCESS;
}


void
distribute_round_robin( __STATE__ scheduler_t *s,
						__STATE__ job_list_t  *jlist )
{
	// здесь тупо сортируем задачи сначала по времени ожидания,
	// а потом по приоритетамf
	rr_job_queues_t *queues = s->state;
	rr_job_ll_t *pending = queues->pending;
	
	if ( ! pending ) {
		pending = malloc( sizeof( rr_job_ll_t ) );

		pending->entry = jlist;
		pending->next = pending->prev = NULL;
		
		jlist = jlist->next;
	}

	for ( ; jlist; jlist = jlist->next ) {
	    rr_job_ll_t *new = malloc( sizeof( rr_job_ll_t ) );
		
		new->entry = jlist;
		new->next = new->prev = NULL;
			
		pending = _place_by_wait_time( pending, new );
	}

	queues->pending = pending;
	
	s->state = queues;
}


void
destroy_round_robin( __STATE__ scheduler_t *s )
{
	rr_job_ll_t *f = ((rr_job_queues_t *) s->state)->finished;
	rr_job_ll_t *n = f->next;

	for ( ; n; f = n, n = n->next ) free( n );
	free( f );

	free( s->state );
}


void
print_round_robin( __IN__ scheduler_t *s )
{
	if ( ! s->state ) return;

	rr_job_ll_t *jll = ((rr_job_queues_t *) s->state)->pending;

	_DEBUG_PUTS( "\n\n-------------------------------------------------------------------\n"
			   "                                ОЧЕРЕДЬ\n"
			   "-------------------------------------------------------------------" );

	for ( ; jll; jll = jll->next ) {
		job_t *j = &( jll->entry->job );	
		cluster_print_job_truncated( j );
	}
}


rr_job_ll_t *
_move_job_to_queue( __STATE__ rr_job_ll_t* *new_queue,
					__STATE__ rr_job_ll_t* *old_queue,
					__STATE__ rr_job_ll_t  *jll )
{
	if ( jll == *old_queue )
		*old_queue = jll->next;
	else {
		jll->prev->next = jll->next;
		if ( jll->next ) jll->next->prev = jll->prev;
	}

	jll->next = NULL;
	jll->prev = NULL;
	
	*new_queue = _place_by_wait_time( *new_queue, jll );
}


quantum_t
_get_min_time_to_free( __IN__ node_list_t *n )
{
	quantum_t t = 0;
	
	for ( ; n; n = n->next ) {
		job_list_t *jlist = n->node->jobs;

		for ( ; jlist; jlist = jlist->next_on_node ) {
			job_t *j = &( jlist->job );

			quantum_t time_left = j->time_before_start
				                + j->estimated_time
				                - j->exec_time;

			if ( time_left < t ) t = time_left;
		}
	}

	return t;
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


rr_job_ll_t *
_place_by_priority( __STATE__ rr_job_ll_t *queue,
					__STATE__ rr_job_ll_t *jll )
{
	job_t *j = &( jll->entry->job );

	if ( j->priority <= queue->entry->job.priority ) {
		jll->next   = queue;
		queue->prev = jll;
		
		return jll;
	}

	rr_job_ll_t *current, *end;

	current = queue->next;
	end     = queue;
	
	for ( ; current; current = current->next ) {
		if ( j->priority <= current->entry->job.priority ) {
			current->prev = jll;
			end->next = jll;
			
			jll->next = current;
			jll->prev = end;
			
			return queue;
		}
		end = current;
	}

	end->next = jll;
    jll->prev = end;

	return queue;
}


rr_job_ll_t *
_place_by_wait_time( __STATE__ rr_job_ll_t *queue,
					 __STATE__ rr_job_ll_t *jll )
{
	job_t *j = &( queue->entry->job );

	if ( j->wait_time < queue->entry->job.wait_time ) {
		jll->next   = queue;
		queue->prev = jll;
		
		return jll;
	}

	rr_job_ll_t *end, *current;

	current = queue->next;
	end     = queue;
	
	for ( ; current; current = current->next ) {
		if ( j->wait_time <= current->entry->job.wait_time ) {
			end->next = _place_by_priority( current, jll );
			return queue;
		}		
		end = current;
	}

	end->next = jll;
	jll->prev = end;

	return queue;
}
