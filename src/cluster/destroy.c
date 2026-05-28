#include <cluster/cluster.h>

#include <stdlib.h>


static void _destroy_bus( __STATE__ bus_connection_t* bus );
static void _destroy_connection( __STATE__ interconnect_t *con );
static void _destroy_connections( __STATE__ interconnect_list_t *cons );
/* static void _destroy_job_list( __STATE__ job_list_t *jlist ); */
static void _destroy_job_sequence( __STATE__ job_moment_list_t *jseq );
static void _destroy_node( __STATE__ node_t* n );
static void _destroy_scheduler( __STATE__ scheduler_t *s );


void
cluster_destroy( __STATE__ cluster_t *c )
{
	if ( c->job_sequence ) _destroy_job_sequence( c->job_sequence );
	if ( c->connections )  _destroy_connections( c->connections );

	free( c->node_registry );

	_destroy_scheduler( c->scheduler );
}


void
_destroy_bus( __STATE__ bus_connection_t* bus )
{
	for ( int n = 0; n < bus->connected_nodes_count; ++n )
		_destroy_node( &( bus->connected_nodes[ n ] ) );

	free( bus->connected_nodes );
}


void
_destroy_connection( __STATE__ interconnect_t *con )
{
	switch ( con->type ) {
	case IC_TYPE_BUS: _destroy_bus( (bus_connection_t *) con->specs );
	}
}


void
_destroy_connections( __STATE__ interconnect_list_t *cons )
{
	interconnect_list_t *next = cons->next;

	for ( ; next; cons = next, next = cons->next ) {
		_destroy_connection( &cons->ic );
		free( cons );
	}

	free( cons );
}


/* void */
/* _destroy_job_list( __STATE__ job_list_t *jlist ) */
/* { */
/* 	job_list_t *next = jlist->next; */

/* 	for ( ; next; jlist = next, next = jlist->next ) free( jlist ); */
/* } */


void
_destroy_job_sequence( __STATE__ job_moment_list_t *jseq )
{
	job_moment_list_t *next = jseq->next;

	/* for ( ; next; jseq = next, next = jseq->next ) { */
	/* 	_destroy_job_list( jseq->moment.job_list_head ); */
	/* 	free( jseq ); */
	/* } */

	for ( ; next; jseq = next, next = jseq->next ) free( jseq );

	free( jseq );
}


void
_destroy_node( __STATE__ node_t* n )
{
	free( n->occupation.cpu );
}


void
_destroy_scheduler( __STATE__ scheduler_t *s )
{
	s->destroy( s );
}
