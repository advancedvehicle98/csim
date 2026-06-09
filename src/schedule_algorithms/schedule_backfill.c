#include <schedule_algorithms.h>

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>


// https://repositorio.unican.es/xmlui/handle/10902/39674


#define _next( LIST ) LIST->sched_info


static job_list_t *_backfill( __STATE__       job_list_t *q,
							  __STATE__       node_t*    *nodes,
							  __IN__    const size_t      node_count );

static job_list_t *_place_by_priority( __STATE__ job_list_t *jlist,
									   __STATE__ job_list_t *j );

static job_list_t *_place_by_wait_time( __STATE__ job_list_t *jlist,
										__STATE__ job_list_t *j );

static job_list_t *_remove_job_from_queue( __STATE__ job_list_t *prev,
										   __STATE__ job_list_t *jl );

static bool _try_allocate_first_job( __STATE__       job_list_t *jl,
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

	/* s->state = new_queue; */
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
_backfill( __STATE__       job_list_t *q,
		   __STATE__       node_t*    *nodes,
		   __IN__    const size_t      node_count )
{

}


job_list_t *
_place_by_priority( __STATE__ job_list_t *jlist,
					__STATE__ job_list_t *jl )
{
	job_t *j = &( jl->job );

	if ( j->priority <= jlist->job.priority ) {
		_next( jl ) = jlist;
		return jl;
	}

	job_list_t *current, *end;

	current = _next( jlist );
	end = jlist;
	
	for ( ; current; current = _next( current ) ) {
		if ( j->priority <= current->job.priority ) {
			_next( end ) = jl;
			_next( jl ) = current;
			return jlist;
		}
		end = current;
	}

	_next( end ) = jl;
	_next( jl ) = NULL;

	return jlist;
}


job_list_t *
_place_by_wait_time( __STATE__ job_list_t *jlist,
					 __STATE__ job_list_t *jl )
{
	job_t *j = &( jl->job );

	if ( j->wait_time < jlist->job.wait_time ) {
		_next( jl ) = jlist;
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

	return jlist;
}


job_list_t *
_remove_job_from_queue( __STATE__ job_list_t *prev,
						__STATE__ job_list_t *jl )
{
	job_list_t *end = _next( jl );

	_next( jl ) = NULL;
	
	// ЗДЕСЬ НЕЛЬЗЯ ПЕРЕЗАПИСЫВАТЬ _prev ИЛИ _next_candidate
	
	if ( ! prev ) return end;

	_next( prev ) = end;
	
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
