#include <cluster/cluster.h>

#include <stdlib.h>


static void _destroy_job_list( __IN__ job_list_t *jlist );
static void _destroy_job_sequence( __IN__ job_moment_list_t *jseq );


void
cluster_destroy( __STATE__ cluster_t *c )
{
	_destroy_job_sequence( c->job_sequence );
}


void
_destroy_job_list( __IN__ job_list_t *jlist )
{
	job_list_t *next = jlist->next;

	for ( ; next; jlist = next, next = jlist->next ) free( jlist );
}


void
_destroy_job_sequence( __IN__ job_moment_list_t *jseq )
{
	job_moment_list_t *next = jseq->next;

	for ( ; next; jseq = next, next = jseq->next ) {
		_destroy_job_list( jseq->moment.job_list_head );
		free( jseq );
	}
}
