#include <schedule_algorithms.h>

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>


// https://repositorio.unican.es/xmlui/handle/10902/39674


#define _next( LIST ) LIST->sched_info
// потому что лень делать лишнюю структуру
// эти макросы ЗАПРЕЩАЕТСЯ применять на тех задачах,
// которым присвоен узел, т.е. это только для навигации по очередям
// (в частности по кандидатам для бэкфила в bcan в schedule_backfill)
#define _prev( LIST )           LIST->prev_on_node
#define _next_candidate( LIST ) LIST->next_on_node


job_list_t *_get_backfill_candidates( __STATE__       job_list_t *jl,
									  __STATE__       node_t*    *nodes,
									  __IN__    const size_t      node_count );

job_list_t *_place_by_priority( __STATE__ job_list_t *jlist,
								__STATE__ job_list_t *j );

job_list_t *_place_by_wait_time( __STATE__ job_list_t *jlist,
								 __STATE__ job_list_t *j );

job_list_t *_remove_job_from_queue( __STATE__ job_list_t *prev,
									__STATE__ job_list_t *jl );

bool _try_allocate_first_job( __STATE__       job_list_t *jl,
							  __STATE__       node_t*    *nodes,
							  __IN__    const size_t      node_count );

job_list_t *_try_backfill_jobs( __STATE__       job_list_t *bcan,
								__STATE__       node_t*    *nodes,
								__IN__    const size_t      node_count );


void
schedule_backfill( __STATE__       scheduler_t *s,
				   __IN__          node_t*     *n,
				   __IN__    const size_t       n_count )
{
	job_list_t *queue = s->state, *prev = NULL;

	while ( queue ) {
		if ( _try_allocate_first_job( queue, n, n_count ) ) {
			// возвращает следующую задачу
			queue = _remove_job_from_queue( prev, queue );
			if ( ! prev ) s->state = queue;
			continue;
		}
		
		prev = queue;
		queue = _next( queue );
	}

	queue = s->state;

	if ( ! queue ) return;

	job_list_t *bcan = _get_backfill_candidates( queue, n, n_count );
	job_list_t *new_queue = NULL;
	
	if ( bcan ) new_queue = _try_backfill_jobs( bcan, n, n_count );

	if ( new_queue ) s->state = new_queue;
}


uint32_t
init_backfill( __STATE__       scheduler_t *s,
			   __IN__          node_t*     *n,
			   __IN__    const size_t       n_count )
{
	// s->state потом будет указывать на "голову очереди"
	s->state = NULL;
	return EXIT_SUCCESS;
}


void
distribute_backfill( __STATE__ scheduler_t *s,
					 __STATE__ job_list_t  *jlist )
{
	// здесь тупо сортируем задачи сначала по времени ожидания,
	// а потом по приоритетам

	job_list_t *head = s->state;
	
	if ( ! head ) {
		head = jlist;

		if ( jlist->job.cpu_count ) {
			jlist->job.thread_count *= jlist->job.cpu_count;
			jlist->job.cpu_count = 0;
		}
		_next( jlist ) = NULL;
		
		jlist = jlist->next;
	}

	for ( ; jlist; jlist = jlist->next ) {
		// придётся игнорировать кол-во процов
		job_t *j = &( jlist->job );

		if ( j->cpu_count ) {
			j->thread_count *= j->cpu_count;
			j->cpu_count = 0;
		}
		_next( jlist ) = NULL;
			
		head = _place_by_wait_time( head, jlist );
	}

	s->state = head;
}


void
destroy_backfill( __STATE__ scheduler_t *s )
{
	
}


void
print_backfill( __IN__ scheduler_t *s )
{
	if ( ! s->state ) return;

	job_list_t *jlist = s->state;

	_DEBUG_PUTS( "\n\n-------------------------------------------------------------------\n"
			   "                                ОЧЕРЕДЬ\n"
			   "-------------------------------------------------------------------" );

	for ( ; jlist; jlist = _next( jlist ) )
		cluster_print_job_truncated( &( jlist->job ) );
}


job_list_t *
_get_backfill_candidates( __STATE__       job_list_t *q,
						  __STATE__       node_t*    *nodes,
						  __IN__    const size_t      node_count )
{
	job_t *j = &( q->job );
	
	job_list_t *last_job = NULL;
	node_t *best_node = nodes[ 0 ];
	quantum_t best_shadow_time = ULLONG_MAX;
	
	for ( int i = 0; i < node_count; ++i ) {
		job_list_t *rjlist = nodes[ i ]->jobs;
		
		size_t future_free_threads = 0;
		
		for ( ; rjlist; rjlist = rjlist->next_on_node ) {
			job_t *rj = &( rjlist->job );
			
			future_free_threads += rj->thread_count;
			
			if ( future_free_threads > j->thread_count ) {
				quantum_t shadow_time = rj->time_before_start
					                  + rj->estimated_time
					                  - rj->exec_time;

				if ( best_shadow_time > shadow_time ) {
					best_node = nodes[ i ];
					last_job = rjlist;
					best_shadow_time = shadow_time;
				}
			}
		}
	}

	size_t thread_headr = best_node->cpu_count * best_node->thread_count - j->thread_count;
	job_list_t *bcan = NULL, *bcan_current = NULL;

	job_list_t *rjlist = last_job;

	for ( ; rjlist; rjlist = rjlist->next_on_node )
		thread_headr += rjlist->job.thread_count;
	
	q = _next( q );
	
	for ( ; q; q = _next( q ) ) {
		j = &( q->job );
		
		for ( int i = 0; i < node_count; ++i ) {
			size_t thread_occupation = 0;
			
			if ( nodes[ i ] == best_node ) {
				if ( cluster_get_total_time_for_job( j, nodes[ i ] ) < best_shadow_time
					 || j->thread_count < thread_headr )
					goto _new_backfill_candidate;
			}
			else if ( j->thread_count < thread_occupation )
				goto _new_backfill_candidate;
		}
		
		continue;

_new_backfill_candidate:
		if ( ! bcan ) {
			bcan = q;
			bcan_current = bcan;
		}
		else {
			_next_candidate( bcan_current ) = q;
			bcan_current = q;
		}

		_next_candidate( bcan ) = NULL;
	}
	
	return bcan;
}


job_list_t *
_place_by_priority( __STATE__ job_list_t *jlist,
					__STATE__ job_list_t *jl )
{
	job_t *j = &( jl->job );

	if ( j->priority <= jlist->job.priority ) {
		_next( jl ) = jlist;
		_prev( jl ) = NULL;
		return jl;
	}

	job_list_t *current, *end;

	current = _next( jlist );
	end = jlist;
	
	for ( ; current; current = _next( current ) ) {
		if ( j->priority <= current->job.priority ) {
			_next( end ) = jl;
			_prev( current ) = jl;
			
			_next( jl ) = current;
			_prev( jl ) = end;
			
			return jlist;
		}
		end = current;
	}

	_next( end ) = jl;
	
	_next( jl ) = NULL;
	_prev( jl ) = end;

	return jlist;
}


job_list_t *
_place_by_wait_time( __STATE__ job_list_t *jlist,
					 __STATE__ job_list_t *jl )
{
	job_t *j = &( jl->job );

	if ( j->wait_time < jlist->job.wait_time ) {
		_next( jl ) = jlist;
		_prev( jl ) = NULL;
		return jl;
	}

	job_list_t *end, *current;

	current = _next( jlist );
	end     = jlist;
	
	for ( ; current; current = _next( current ) ) {
		if ( j->wait_time <= current->job.wait_time ) {
			_next( end ) = _place_by_priority( current, jl );
			return jlist;
		}		
		end = current;
	}

	_next( end ) = jl;
	
	_next( jl ) = NULL;
	_prev( jl ) = end;

	return jlist;
}


job_list_t *
_remove_job_from_queue( __STATE__ job_list_t *prev,
						__STATE__ job_list_t *jl )
{
	job_list_t *end = _next( jl );

	_next( jl ) = NULL;
	
	// ЗДЕСЬ НЕЛЬЗЯ ПЕРЕЗАПИСЫВАТЬ _prev ИЛИ _next_candidate
	
	if ( ! prev ) {
		if ( end ) _prev( end ) = NULL;
		return end;
	}

	_next( prev ) = end;
	if ( end ) _prev( end ) = prev;
	
	return end;
}


bool
_try_allocate_first_job( __STATE__       job_list_t *jl,
						 __STATE__       node_t*    *nodes,
						 __IN__    const size_t      node_count )
{
	job_t *j = &( jl->job );
	
	for ( int i = 0; i < node_count; ++i )
		if ( cluster_check_node_for_job( j, nodes[ i ] ) ) {
			cluster_put_job_to_node( jl, nodes[ i ] );
			return true;
		}
	
	return false;
}


job_list_t *
_try_backfill_jobs( __STATE__       job_list_t *bcan,
					__STATE__       node_t*    *nodes,
					__IN__    const size_t      node_count )
{
	job_list_t *new_queue = NULL;
	
	while ( bcan ) {
		job_list_t *next_candidate = _next_candidate( bcan );
		job_list_t *prev = _prev( bcan );
		job_list_t *next = _next( bcan );
		
		if ( _try_allocate_first_job( bcan, nodes, node_count ) ) {
			_remove_job_from_queue( prev, bcan );
			if ( ! prev ) new_queue = next;
		}
		
		bcan = next_candidate;
	}

	return new_queue;
}
