#include <cluster/cluster.h>

#include <stdlib.h>


static void _populate_node_registry( __STATE__ cluster_t *c );

static void _reset_statistics( __OUT__ statistics_t *s );


uint32_t
cluster_init( __STATE__       cluster_t   *c,
			  __IN__    const char        *jf,
			  __IN__    const char        *cf,
			  __IN__          scheduler_t *s )
{
	if ( ! s || ! s->schedule || ! s->init || ! s->fetch ) {
		_ERROR_PUTS( "В scheduler_t не заполнены все поля" );
		return EXIT_FAILURE;
	}
	
	uint32_t status;

	c->scheduler = s;

	status = cluster_job_sequence_from_dataset( c, jf );
	
	if ( _IS_ERROR( status ) ) {
		_ERROR_PUTS( "Не удалось распарсить датасет задач" );
		return EXIT_FAILURE;
	}

	status = cluster_node_network_from_dataset( c, cf );
	
	if ( _IS_ERROR( status ) ) {
		_ERROR_PUTS( "Не удалось распарсить датасет узлов" );
		return EXIT_FAILURE;
	}

	_populate_node_registry( c );
	
	_reset_statistics( &( c->statistics ) );

	_DEBUG_PUTS( "Кластер инициализирован" );
	
	return EXIT_SUCCESS;
}


void
_populate_node_registry( __STATE__ cluster_t *c )
{
	c->node_registry = (node_t* *) malloc( sizeof( node_t * )*c->node_count );

	interconnect_list_t *cons = c->connections;
	int n = 0;

	for ( ; cons; cons = cons->next ) {
		switch ( cons->ic.type ) {
			
		case IC_TYPE_BUS: {
			bus_connection_t *bcon = (bus_connection_t *) cons->ic.specs;

			for ( int i = 0; i < bcon->connected_nodes_count; ++i ) {
				c->node_registry[ n++ ] = &bcon->connected_nodes[ i ];
				
				_DEBUG_PRINTF( "Готов узел %s...", bcon->connected_nodes[ i ].name );
			}
		} break;
			
		}
	}
}


void
_reset_statistics( __OUT__ statistics_t *s )
{
	s->average_stall_time        =
	s->average_wait_time         =
	s->average_exec_time         =
	s->total_active_time_quantum = 
	s->total_active_time_ms      = 0;

	s->jobs_done = 0;

	s->total_average_cpu_load    = 
	s->total_average_memory_load = 0;		
}
