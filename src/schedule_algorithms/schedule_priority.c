#include <schedule_algorithms.h>

#include <stdlib.h>

// получилась смесь приоритетного планирования с EDF c вытеснением


static __PURE quantum_t _get_max_response_time( __IN__ const node_t *n );

static quantum_t _get_min_time_to_free( __IN__ node_list_t *n );

static node_t *_pick_node( __IN__ const job_t       *j,
						   __IN__ const node_list_t *nodes );

static node_t *_pick_node_for_preempting( __IN__ const job_t       *j,
										  __IN__ const node_list_t *nodes );

static job_list_t *_place_job_by_priority( __STATE__ job_list_t *qlist,
										   __STATE__ job_list_t *jlist );

static job_list_t *_place_job_by_wait_time( __STATE__ job_list_t *qlist,
											__STATE__ job_list_t *jlist );

static void _place_node_by_response_time( __STATE__       node_list_t *nlist,
										  __IN__          node_list_t *new_nlist,
										  __IN__    const quantum_t    rt );

static node_list_t *_place_node_by_speed( __STATE__ node_list_t *nlist,
										  __IN__    node_t      *n );

static bool _preempt_with_job( __STATE__ priority_queues_t *p,
							   __STATE__ node_list_t       *nodes,
							   __STATE__ job_list_t        *jlist );

static void _put_job_to_respective_queue( __STATE__ priority_queues_t *p,
										  __STATE__ job_list_t        *jlist );


void
schedule_priority( __STATE__       scheduler_t *s,
				   __UNUSED        node_t*     *_n,
				   __UNUSED  const size_t       _n_count )
{
	priority_queues_t *p = (priority_queues_t *) s->state;

	job_list_t *urgent = p->by_urgency, *rest = p->by_priority;
	
	quantum_t min_time_to_free = p->min_time_to_free;

	node_list_t *nodes = p->node_by_speed;

	job_list_t *prev_urgent = NULL;
	
	while ( urgent ) {
		job_t *j = &( urgent->job );
		
		node_t *picked_node = _pick_node( j, nodes );

		if ( picked_node ) {
			if ( ! prev_urgent )
				p->by_urgency = (job_list_t *) urgent->sched_info;
			else
				prev_urgent->sched_info = urgent->sched_info;

			urgent->sched_info = NULL;			
			cluster_put_job_to_node( urgent, picked_node );

			urgent = prev_urgent ? prev_urgent->sched_info : p->by_urgency;
			continue;
		}
		else _preempt_with_job( p, nodes, urgent );

		prev_urgent = urgent;
		urgent = (job_list_t *) urgent->sched_info;
	}

	job_list_t *prev_rest = NULL;
	
	while ( rest ) {
		job_t *j = &( rest->job );
		node_t *picked_node = _pick_node( j, nodes );

		if ( picked_node ) {
			if ( ! prev_rest )
				p->by_priority = (job_list_t *) rest->sched_info;
			else 
				prev_rest->sched_info = rest->sched_info;
			
			rest->sched_info = NULL;
			cluster_put_job_to_node( rest, picked_node );

			rest = prev_rest ? prev_rest->sched_info : p->by_priority;
			continue;
		}

		if ( j->wait_time < p->min_time_to_free ) {
			if ( ! prev_rest )
				p->by_priority = (job_list_t *) rest->sched_info;
			else 
				prev_rest->sched_info = rest->sched_info;

			job_list_t *job_to_move = rest;
			job_to_move->sched_info = NULL;

			_place_job_by_wait_time( urgent, job_to_move );

			rest = prev_rest ? prev_rest->sched_info : p->by_priority;
			continue;
		}

		rest = (job_list_t *) rest->sched_info;
		prev_rest = rest;
	}
	
	p->min_time_to_free = _get_min_time_to_free( nodes );
}


uint32_t
init_priority( __STATE__       scheduler_t *s,
			   __IN__          node_t*     *n,
			   __IN__    const size_t       n_count )
{
	s->state = malloc( sizeof( priority_queues_t ) );

	priority_queues_t *p = (priority_queues_t *) s->state;

	p->by_priority = p->by_urgency = NULL;
	
	p->min_time_to_free = 0;
	
	p->node_by_speed = (node_list_t *) malloc( sizeof( node_list_t ) );

	node_list_t *nlist = p->node_by_speed;

	nlist->node = n[ 0 ];
	nlist->next = NULL;

	for ( int i = 1; i < n_count; ++i )
		nlist = _place_node_by_speed( nlist, n[ i ] );
}


void
distribute_priority( __STATE__ scheduler_t *s,
					 __STATE__ job_list_t  *jlist )
{
	priority_queues_t *q = (priority_queues_t *) s->state;

	if ( ! q->by_priority ) {
		q->by_priority = jlist;
		jlist->sched_info = NULL;
		jlist = jlist->next;
	}
	
	for ( ; jlist ; jlist = jlist->next )
		_put_job_to_respective_queue( s->state, jlist );
}


void
destroy_priority( __STATE__ scheduler_t *s )
{
	priority_queues_t *p = (priority_queues_t *) s->state;	
	node_list_t *c, *n;

	c = p->node_by_speed;
	n = c->next;

	for ( ; n; c = n, n = n->next ) free( c );
	free( c );
	
	free( s->state );
}


void
print_priority( __IN__ scheduler_t *s )
{
	priority_queues_t *q = (priority_queues_t *) s->state;

	job_list_t *p = q->by_priority;

	if ( ! p ) return;

	job_list_t *u = q->by_urgency;

	_DEBUG_PUTS( "\n\n-------------------------------------------------------------------\n"
			   "                             СРОЧНАЯ ОЧЕРЕДЬ\n"
			   "-------------------------------------------------------------------" );

	if ( ! u )
		_DEBUG_PUTS( "  Нет задач в очереди" );
	else for ( ; u; u = (job_list_t *) u->sched_info )
		cluster_print_job_truncated( &( u->job ) );

	_DEBUG_PUTS( "\n\n-------------------------------------------------------------------\n"
			   "                             ПРИОРИТЕТНАЯ\n"
			   "-------------------------------------------------------------------" );

	for ( ; p; p = (job_list_t *) p->sched_info )
		cluster_print_job_truncated( &( p->job ) );
}


quantum_t
_get_max_response_time( __IN__ const node_t *n )
{
	return n->response_time
		 + n->response_time_variance // на будущее
		 + n->ic->delay_max;
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
_pick_node( __IN__ const job_t       *j,
			__IN__ const node_list_t *nodes )
{
	for ( ; nodes; nodes = nodes->next )
		if ( cluster_check_node_for_job( j, nodes->node ) )
			return nodes->node;

	return NULL;
}


job_list_t *
_place_job_by_priority( __STATE__ job_list_t *qlist,
						__STATE__ job_list_t *jlist )
{
	job_t *j = &( jlist->job );

	if ( j->priority <= qlist->job.priority ) {
		jlist->sched_info = (void *) qlist;
		return jlist;
	}

	job_list_t *plist = qlist, *nlist = (job_list_t *) qlist->sched_info;

	if ( ! nlist ) {
		plist->sched_info = (void *) jlist;
		return qlist;
	}
	
	while ( true ) {
		job_t *nj = &( nlist->job );
		
		if ( j->priority <= nj->priority ) {
			plist->sched_info = (void *) jlist;
			jlist->sched_info = (void *) nlist;
			break;
		}

		plist = nlist; nlist = (job_list_t *) nlist->sched_info;

		if ( ! nlist ) {
			plist->sched_info = (void *) jlist;
			jlist->sched_info = NULL;
			break;
		}
	}

	return qlist;
}


job_list_t *
_place_job_by_wait_time( __STATE__ job_list_t *qlist,
						 __STATE__ job_list_t *jlist )
{
	job_t *j = &( jlist->job );

	if ( j->wait_time <= qlist->job.wait_time ) {
		if ( j->wait_time == qlist->job.wait_time )
			if ( j->priority > qlist->job.priority ) {
				_place_job_by_priority( qlist, jlist );
				return qlist;
			}
		
		jlist->sched_info = (void *) qlist;
		return jlist;
	}

	job_list_t *plist = qlist, *nlist = (job_list_t *) qlist->sched_info;

	if ( ! nlist ) {
		plist->sched_info = (void *) jlist;
		return qlist;
	}
	
	while ( true ) {
		job_t *nj = &( nlist->job );
		
		if ( j->wait_time <= nj->wait_time ) {
			if ( j->wait_time == nj->wait_time )
				if ( j->priority > nj->priority ) {
					_place_job_by_priority( nlist, jlist );
					break;
				}
			
			plist->sched_info = (void *) jlist;
			jlist->sched_info = (void *) nlist;
			break;
		}

		plist = nlist; nlist = (job_list_t *) nlist->sched_info;

		if ( ! nlist ) {
			plist->sched_info = (void *) jlist;
			jlist->sched_info = NULL;
			break;
		}
	}

	return qlist;
}


void
_place_node_by_response_time( __STATE__       node_list_t *nlist,
							  __IN__          node_list_t *new_nlist,
							  __IN__    const quantum_t    rt )
{
	node_list_t *prev_nlist = nlist;
	nlist = nlist->next;
	
	for ( ; nlist && rt > _get_max_response_time( nlist->node )
		  ; prev_nlist = nlist, nlist = nlist->next );

	if ( ! nlist ) {
		prev_nlist->next = new_nlist;
		return;
	}

	new_nlist->next  = nlist;
	prev_nlist->next = new_nlist;
}


// сначала ищет по скорости
// если есть узлы равные по скорости, то потом смотрит по времени
// отклика на шине и у самого узла
node_list_t *
_place_node_by_speed( __STATE__ node_list_t *nlist,
					  __IN__    node_t      *n )
{
	float exec_rate = n->max_exec_rate;
	quantum_t response_time = _get_max_response_time( n );

	node_list_t *new_nlist = (node_list_t *) malloc( sizeof( node_list_t ) );

	new_nlist->next = NULL;
	new_nlist->node = n;
	
	if ( exec_rate >= nlist->node->max_exec_rate ) {
		if ( exec_rate == nlist->node->max_exec_rate )
			if ( response_time > _get_max_response_time( nlist->node ) ) {
				_place_node_by_response_time( nlist, new_nlist, response_time );
				return nlist;
			}

		new_nlist->next = nlist;

		return new_nlist;
	}

	node_list_t *aux_nlist = nlist->next, *prev_nlist = nlist;

	for ( ; aux_nlist && exec_rate < aux_nlist->node->max_exec_rate
		  ; prev_nlist = aux_nlist, aux_nlist = aux_nlist->next );

	if ( ! aux_nlist )
		prev_nlist->next = new_nlist;
	else if ( aux_nlist->node->max_exec_rate == exec_rate )
		_place_node_by_response_time( aux_nlist, new_nlist, response_time );
	else {
		prev_nlist->next = new_nlist;
		new_nlist->next  = aux_nlist;
	}
	
	return nlist;
}


// для этой функции надо преобразовать задачу так, чтобы
// cpu_count был нулевой (для простоты приходится так делать)
bool
_is_job_same_more_demanding( __IN__ const job_t *lj,
							 __IN__ const job_t *rj )
{
	bool for_memory = lj->mem_size >= rj->mem_size;
	
	bool for_thread_count = ( lj->cpu_count
							? lj->cpu_count*lj->thread_count
							: lj->thread_count )
		                    >= rj->thread_count;

	return for_memory && for_thread_count;
}


// если удалось куда-то впихнуть задачу, возрващает новое начало списка
// jlist, как правило, должен указывать на верхушку какой-нибудь очереди
bool
_preempt_with_job( __STATE__ priority_queues_t *p,
				   __STATE__ node_list_t       *nodes,
				   __STATE__ job_list_t        *jlist )
{
	job_t *j = &( jlist->job );

	j->thread_count *= j->cpu_count;
	j->cpu_count = 0;
	
	for ( ; nodes; nodes = nodes->next ) {
		node_t *n = nodes->node;

		if ( ! cluster_check_node_for_job_as_if_empty( j, n ) ) continue;

		// пока сделано в самом простом виде с земещением одной задачи
		// приоритета ниже
		
		// потому что думать как подбирать по нескольку задач и делать
		// это хорошо лень
		job_list_t *nlist = n->jobs;

		for ( ; nlist; nlist = nlist->next_on_node ) {
			job_t *nj = &( nlist->job );
			
			if ( _is_job_same_more_demanding( nj, j ) ) {
				cluster_remove_job_from_node( nlist );
				cluster_put_job_to_node( jlist, n );

				// мы заведомо знаем, что замещать могут только
				// задачи из очереди urgent, поэтому для простоты
				// делаем так
				p->by_urgency = (job_list_t *) p->by_urgency->sched_info;

				nlist->sched_info = NULL;
				_put_job_to_respective_queue( p, nlist );

				return true;
			}
		}
	}
	
	return false;
}


void
_put_job_to_respective_queue( __STATE__ priority_queues_t *q,
							  __STATE__ job_list_t        *jlist )
{
	job_t *j = &( jlist->job );
		
	if ( j->max_wait_time >= q->min_time_to_free ) {
		q->by_priority = _place_job_by_priority( q->by_priority, jlist );
		return;
	}
	
	if ( ! q->by_urgency ) {
		q->by_urgency = jlist;
		jlist->sched_info = NULL;
		return;
	}
			
	q->by_urgency = _place_job_by_wait_time( q->by_urgency, jlist );
}
