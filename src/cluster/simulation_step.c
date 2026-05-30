#include <cluster/cluster.h>

#include <stdlib.h>


uint32_t
cluster_simulation_step( __STATE__ cluster_t       *c,
						 __IN__    const quantum_t  t)
{
	job_moment_list_t *jm = c->job_sequence;
	
	for ( ; jm; jm = jm->next ) {
		bool is_everything_done = true;
		
		// по идее можно сделать ещё один указатель, который
		// будет обновляться после каждой итерации и заставлять
		// на каждом шаге вести учёт с нужного момента
		// но эт на потом
		if ( jm->is_everything_done ) continue;
		
		if ( jm->moment.time >= t ) break;
		
		job_list_t *jl = jm->moment.job_list_head;

		for ( ; jl; jl = jl->next )
			is_everything_done &= cluster_update_job( jl, &c->statistics );

		jm->is_everything_done = is_everything_done;
	}

	for ( int n = 0; n < c->node_count; ++n )
		cluster_update_node( c->node_registry[ n ], &c->statistics );
	
	return EXIT_SUCCESS;
}
