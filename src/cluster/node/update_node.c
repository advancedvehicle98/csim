#include <cluster/node.h>


static bool
_is_node_free( __IN__ const node_t *n )
{
	return n->occupation.mem == n->mem_size;
}

void
cluster_update_node( __STATE__ node_t       *n,
					 __STATE__ statistics_t *s )
{
	// в основном статистика + возможно какое-то изменение
	// частоты процов
}

