#include <cluster/job_sequence.h>

#include <stdlib.h>


static void _add_job_to_moment( __OUT__       job_moment_list_t *jseq,
								__IN__  const job_t             *j );

static job_moment_list_t *_get_less_equal_moment( __IN__       job_moment_list_t *jseq,
 												  __IN__ const quantum_t          m );
  
static void _insert_moment_after_moment( __OUT__       job_moment_list_t *jseq,
										 __IN__  const job_t             *j,
										 __IN__  const quantum_t          m );

static void _job_sequence_init( __OUT__         job_moment_list_t* *jseq,
								__IN__    const job_t              *j,
								__IN__    const quantum_t           m );


void
cluster_add_job_moment( __STATE__       cluster_t *c,
						__IN__    const job_t     *j,
						__IN__    const quantum_t  m )
{
	job_moment_list_t *jseq = c->job_sequence;

	if ( ! jseq ) {
		_job_sequence_init( &jseq, j, m );
		goto _finish_add;
	}

	jseq = _get_less_equal_moment( jseq, m );

	if ( jseq->moment.time == m ) _add_job_to_moment( jseq, j );
	else                          _insert_moment_after_moment( jseq, j, m );
		
_finish_add:
	c->job_sequence = jseq;
}


void
_add_job_to_moment( __OUT__       job_moment_list_t *jseq,
					__IN__  const job_t             *j )
{
	job_list_t *l = jseq->moment.job_list_head;

	if ( ! l ) {
		l = (job_list_t *) malloc( sizeof( job_list_t ) );
		l->next = NULL;
		cluster_copy_job( &l->job, j );

		jseq->moment.job_list_head = l;

		return;
	}

	for ( ; l->next ; l = l->next );

	l->next = (job_list_t *) malloc( sizeof( job_list_t ) );

	l->next->next = NULL;
	cluster_copy_job( &l->next->job, j );
}


job_moment_list_t *
_get_less_equal_moment( __IN__       job_moment_list_t *jseq,
						__IN__ const quantum_t          m )
{
	if ( jseq->moment.time == m ) return jseq;

	job_moment_list_t *t = jseq->next, *p = jseq;
	
	for ( ; t; t = t->next ) {
		if ( t->moment.time == m ) return t;

		if ( p->moment.time < m && m < t->moment.time ) return p;
		
		p = t;
	}

	return p;
}


void
_insert_moment_after_moment( __OUT__       job_moment_list_t *jseq,
							 __IN__  const job_t             *j,
							 __IN__  const quantum_t          m )
{
	job_moment_list_t *next = jseq->next;

	jseq->next = (job_moment_list_t *) malloc( sizeof( job_moment_list_t ) );

	job_moment_list_t *new = jseq->next;

	new->next = next;
	new->moment.time = m;

	_add_job_to_moment( new, j );
}


void
_job_sequence_init( __OUT__       job_moment_list_t* *jseq_out,
					__IN__  const job_t              *j,
					__IN__  const quantum_t           m )
{
	job_moment_list_t *jseq = (job_moment_list_t *) malloc( sizeof( job_moment_list_t ) );

	jseq->next = NULL;
		
	jseq->moment.time = m;
	jseq->moment.job_list_head = (job_list_t *) malloc( sizeof( job_list_t ) );

	jseq->moment.job_list_head->next = NULL;

	cluster_copy_job( &jseq->moment.job_list_head->job, j );
	
	*jseq_out = jseq;
}
