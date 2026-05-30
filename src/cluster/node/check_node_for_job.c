#include <cluster/node.h>


// эта функция просто показывает, можно ли задачу
// поместить в узел

// может использоваться планировщиком наравне с иными критериями
// подбора узлов

bool
cluster_check_node_for_job( __IN__ const job_t  *j,
							__IN__ const node_t *n )
{
	bool has_features = ( j->required_features & n->features ) == j->required_features;

	bool has_enough_memory = j->mem_size <= n->occupation.mem;

	bool has_available_threads;
	size_t required_thread_count = j->thread_count;

	// если cpu_count == 0, то задаче похер на наличие
	// конкретного кол-ва ядер и её потоки можно раскидать
	// на произвольное кол-во ядер

	// если cpu_count = n, то задаче нужно n ядер по несколько потоков
	if ( j->cpu_count ) {
		int available_cores = 0;
		
		for ( size_t c = 0; c < n->cpu_count; ++c )
			if ( required_thread_count <= n->occupation.cpu_threads[ c ] )
				++available_cores;
		
		has_available_threads = j->cpu_count <= available_cores;
	}
	else {
		int available_threads = 0;

		for ( size_t c = 0; c < n->cpu_count; ++c )
			available_threads += n->occupation.cpu_threads[ c ];

		has_available_threads = j->thread_count <= available_threads;
	}
	
	return has_features && has_enough_memory && has_available_threads;
}
