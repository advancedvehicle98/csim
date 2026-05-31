#ifndef __SCHEDULE_ALGORITHMS
#define __SCHEDULE_ALGORITHMS


#include <cluster/cluster.h>


#define ALGORITHMS_HELP \
	"\n\tРежимы планирования:" \
	"\n\t\tfifo : обыный FIFO"


// FIFO ----------------------------------------------------

typedef struct _fifo_queue_t {
	job_list_t *h, *t, *c;
} fifo_queue_t;

__HOT  void schedule_fifo( __STATE__ scheduler_t *s, __IN__ node_t* *n, __IN__ size_t n_count );
__COLD uint32_t init_fifo( __STATE__ scheduler_t *s );
__HOT  void distribute_fifo( __STATE__ scheduler_t *s, __STATE__ job_list_t *jlist );
__COLD void destroy_fifo( __STATE__ scheduler_t *s );
__HOT  void print_fifo( __IN__ scheduler_t *s );


#endif // ! __SCHEDULE_ALGORITHMS
