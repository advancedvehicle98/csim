#include <cluster/cluster.h>
#include <cluster/internal.h>
#include <cluster/timing.h>

#include <stdlib.h>
#include <string.h>


static interconnect_list_t *_add_interconnect_to_list( __STATE__ cluster_t *c );

static interconnect_type_t _fetch_type( __STATE__ char* *l );

static uint32_t _parse_lines( __STATE__ cluster_t *c, __IN__ FILE *f );

static size_t _parse_bus( __OUT__ interconnect_list_t *ilist,
						  __IN__  FILE                *f,
						  __IN__  char                *l,
						  __IN__  char                *l_off );

static void _reset_load_stats( __STATE__ node_t *n );


uint32_t
cluster_node_network_from_dataset( __STATE__       cluster_t *c,
								   __IN__    const char      *f )
{
	c->connections = NULL;
	c->node_count = 0;
	
	return _cluster_parse_file_wrapper( c, f, _parse_lines );
}


interconnect_list_t *
_init_connections( __STATE__ cluster_t *c )
{
	interconnect_list_t *ilist = (interconnect_list_t *) malloc( sizeof( interconnect_list_t ) );

	c->connections = ilist;
	return ilist;
}

interconnect_list_t *
_add_interconnect_to_list( __STATE__ cluster_t *c )
{
	if ( ! c->connections ) return _init_connections( c );

	interconnect_list_t *ilist = c->connections;

	for ( ; ilist->next; ilist = ilist->next );

	ilist->next = (interconnect_list_t *) malloc( sizeof( interconnect_list_t ) );

	return ilist->next;
}


interconnect_type_t
_fetch_type( __STATE__ char* *l )
{
	char *e = strchr( *l, ' ' );
	interconnect_type_t type = IC_TYPE_UNKNOWN;
	
	*e = '\0';

	if      ( _STR_EQUAL( *l, BUS_CONNECTION ) ) type = IC_TYPE_BUS;
	else if ( _STR_EQUAL( *l, P2P_CONNECTION ) ) type = IC_TYPE_P2P;

	*l = e + 1;

	return type;
}


// p2p пока нет потому что хз чё с ним делать
typedef enum {
	_NODE_RESPONSE_TIME,
	_NODE_RESPONSE_TIME_VARIANCE,
	_NODE_MEM_SIZE,
	_NODE_CPU_COUNT,
	_NODE_THREAD_COUNT,
	_NODE_MAX_EXEC_RATE,
	_NODE_FEATURES,
	_NODE_TERM_COUNT_PER_LINE
} _node_term_t;

uint32_t
_parse_lines( __STATE__ cluster_t  *c,
			  __IN__    FILE       *f )
{
	size_t lineno, read_lines;
	char stack_line[ MAX_CLUSTER_FILE_LINE_SIZE ];

	lineno = 1;
	read_lines = 0;
	
	while ( true ) {
		lineno += read_lines;
		c->node_count += read_lines;
		
		char *line_start = fgets( stack_line, MAX_CLUSTER_FILE_LINE_SIZE, f );

		if ( ! line_start ) break;

		interconnect_type_t type = _fetch_type( &line_start );

		if ( type == IC_TYPE_UNKNOWN ) {
			_ERROR_PRINTF( "Неизвестный тип шины (строка %d)", lineno );
			return EXIT_FAILURE;
		}

		interconnect_list_t *il = _add_interconnect_to_list( c );
		
		il->ic.type = type;
		
		switch ( type ) {
			
		case IC_TYPE_BUS: {
			read_lines = _parse_bus( il, f, stack_line, line_start );
		} break;
			
		}

		if ( ! read_lines ) {
			_ERROR_PRINTF( "Не удалось распарсить шину (начиная со строки %d)", lineno );
			return EXIT_FAILURE;
		}
	}
	
	return EXIT_SUCCESS;
}


size_t
_parse_bus( __OUT__ interconnect_list_t *ilist,
			__IN__  FILE                *f,
			__IN__  char                *l,
			__IN__  char                *l_off )
{
	ilist->ic.specs = malloc( sizeof( bus_connection_t ) );

	bus_connection_t *bcon = (bus_connection_t *) ilist->ic.specs;
	char *next_space, *line_start;
	size_t node_count;

	line_start = l_off;
	
	next_space = strchr( line_start, ' ' );
	*next_space = '\0';
	ilist->ic.delay_min = strtoq( line_start, &next_space, 10 );
	line_start = next_space + 1;
	
	next_space = strchr( line_start, ' ' );
	*next_space = '\0';
	ilist->ic.delay_max = strtoq( line_start, &next_space, 10 );
	line_start = next_space + 1;
	
	next_space = strchr( line_start, '\n' );
	*next_space = '\0';
	node_count = strtos( line_start, &next_space, 10 );
	line_start = next_space + 1;

	bcon->connected_nodes = (node_t *) malloc( sizeof( node_t )*node_count );
	bcon->connected_nodes_count = node_count;

	node_t *n = bcon->connected_nodes;

	for ( int i = 0; i < node_count; ++i ) {
		_node_term_t term = _NODE_RESPONSE_TIME;
		int cpu_count;

		line_start = fgets( l, MAX_CLUSTER_FILE_LINE_SIZE, f );
		next_space = strchr( line_start, ' ' );
		*next_space = '\0';

		strcpy( (char * restrict) &n[ i ].name, line_start );

		while ( term < _NODE_TERM_COUNT_PER_LINE ) {
			line_start = next_space + 1;
			next_space = strchr( line_start, ' ' );
			
			if ( next_space ) *next_space = '\0';

			switch ( term ) {

			case _NODE_RESPONSE_TIME: {
				n[ i ].response_time = strtoq( line_start, &next_space, 10 );
			} break;

			case _NODE_RESPONSE_TIME_VARIANCE: {
				n[ i ].response_time_variance = strtoq( line_start, &next_space, 10 );
			} break;

			case _NODE_MEM_SIZE: {
				n[ i ].mem_size = strtos( line_start, &next_space, 10 );
			} break;

			case _NODE_CPU_COUNT: {
				cpu_count = strtos( line_start, &next_space, 10 );
			} break;

			case _NODE_THREAD_COUNT: {
				n[ i ].thread_count = strtos( line_start, &next_space, 10 );
			} break;

			case _NODE_MAX_EXEC_RATE: {
				n[ i ].max_exec_rate = strtof( line_start, &next_space );
			} break;

			case _NODE_FEATURES: {
				n[ i ].features = strtoll( line_start, &next_space, 10 );
			} break;
				
			}

			++term;
		}

		n[ i ].cpu_count = cpu_count;
		n[ i ].exec_rate = n[ i ].max_exec_rate;
		n[ i ].ic        = (interconnect_t *) bcon;

		_reset_load_stats( &( n[ i ] ) );

		n[ i ].occupation.mem = n[ i ].mem_size;
		n[ i ].occupation.cpu_threads = (size_t *) calloc( cpu_count, sizeof( size_t ) );

		for ( size_t c = 0; c < cpu_count; ++c )
			n[ i ].occupation.cpu_threads[ c ] = n[ i ].thread_count;
	}
	
	return node_count;
}


void
_reset_load_stats( __STATE__ node_t *n )
{
	n->load_stats.average_cpu_load    =
	n->load_stats.average_memory_load = 0.0f;

	n->load_stats.average_cpu_stall_time =
	n->load_stats.node_stall_time        = 0;
}


static void *
_start_thread( void *args )
{
	_parse_file_thread_arg_t *a = (_parse_file_thread_arg_t *) args;
	*( a->s ) = cluster_node_network_from_dataset( (cluster_t *) a->c, (const char *) a->f );
	return NULL;
}

void
cluster_node_network_from_dataset_start( __OUT__         uint32_t  *s,
										 __OUT__         pthread_t *p,
										 __STATE__       cluster_t *c,
										 __IN__    const char      *f )
{
	_parse_file_thread_arg_t args = { .s = s, .c = c, .f = f };

	pthread_create( p, NULL, _start_thread, &args );
}
