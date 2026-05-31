#include <cluster/node.h>


static bool _is_node_free( __IN__ const node_t *n );

static void _update_average_cpu_load( __STATE__ node_t       *n,
									  __STATE__ statistics_t *s );

static void _update_average_memory_load( __STATE__ node_t       *n,
										 __STATE__ statistics_t *s );

static void _update_average_stall_time( __STATE__ node_t       *n,
										__STATE__ statistics_t *s );


void
cluster_update_node( __STATE__ node_t       *n,
					 __STATE__ statistics_t *s )
{
	// в основном статистика + возможно какое-то изменение
	// частоты процов

	_update_average_stall_time( n, s );
	_update_average_memory_load( n, s );
	_update_average_cpu_load( n, s );
}


bool
_is_node_free( __IN__ const node_t *n )
{
	return n->occupation.mem == n->mem_size;
}


void
_update_average_cpu_load( __STATE__ node_t       *n,
						  __STATE__ statistics_t *s )
{
	float cpu_load = 0;
	float thread_count = n->thread_count;

	for ( size_t c = 0; c < n->cpu_count; ++c ) {
		float free_threads = (float) n->occupation.cpu_threads[ c ];

		cpu_load += thread_count - free_threads;
	}

	cpu_load /= thread_count * n->cpu_count;

	n->load_stats.average_cpu_load = cpu_load;

	size_t sc = s->average_cpu_load_sc;

	s->average_cpu_load = s->average_cpu_load * sc/( sc + 1 )
		                + cpu_load/( sc + 1 );

	++( s->average_cpu_load_sc );
}


void
_update_average_memory_load( __STATE__ node_t       *n,
							 __STATE__ statistics_t *s )
{
	float free_memory  = (float) n->occupation.mem;
	float total_memory = (float) n->mem_size;
	
	float memory_load = ( total_memory - free_memory )/total_memory;

	n->load_stats.memory_load = memory_load;
	
	size_t sc = s->average_memory_load_sc;

	s->average_memory_load = s->average_memory_load * sc/( sc + 1 )
		                   + memory_load/( sc + 1 );

	++( s->average_memory_load_sc );
}


void
_update_average_stall_time( __STATE__ node_t       *n,
							__STATE__ statistics_t *s )
{
	if ( _is_node_free( n ) ) ++( n->load_stats.node_stall_time );
	else if ( n->load_stats.node_stall_time ) {
		size_t sc = s->average_stall_time_sc;

		s->average_stall_time = (float) s->average_stall_time * sc/( sc + 1 )
			                  + (float) n->load_stats.node_stall_time/( sc + 1 );

		++( s->average_stall_time_sc );
		
		n->load_stats.node_stall_time = 0;
	}
}
