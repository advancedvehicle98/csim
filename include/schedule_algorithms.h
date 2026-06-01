#ifndef __SCHEDULE_ALGORITHMS
#define __SCHEDULE_ALGORITHMS


#include <cluster/cluster.h>


#define ALGORITHMS_HELP \
	"\n\tРежимы планирования:" \
	"\n\t\tfifo     : обычный FIFO" \
	"\n\t\tpriority : вытесняющее с приоритетами"


// FIFO ----------------------------------------------------

// sched_info выступает в качетве указателя на след. элемент

typedef struct _fifo_queue_t {
	job_list_t *h, *t, *c;
} fifo_queue_t;

__HOT  void schedule_fifo( __STATE__ scheduler_t *s, __IN__ node_t* *n, __IN__ const size_t n_count );
__COLD uint32_t init_fifo( __STATE__ scheduler_t *s, __IN__ node_t* *n, __IN__ const size_t n_count );
__HOT  void distribute_fifo( __STATE__ scheduler_t *s, __STATE__ job_list_t *jlist );
__COLD void destroy_fifo( __STATE__ scheduler_t *s );
__HOT  void print_fifo( __IN__ scheduler_t *s );


// По приоритетам -----------------------------------------

typedef struct _priority_queues_t {
	job_list_t *by_urgency; // задачи, которые могут пропасть, если их долго держать
	job_list_t *by_priority; // задачи, расставленные в порядке приоритктов
	node_list_t *node_by_speed; // узлы в порядке скорости для срочных задач
	quantum_t min_time_to_free; // время, в течение которого освободится хоть что-то
} priority_queues_t;

__HOT  void schedule_priority( __STATE__ scheduler_t *s, __IN__ node_t* *n, __IN__ const size_t n_count );
__COLD uint32_t init_priority( __STATE__ scheduler_t *s, __IN__ node_t* *n, __IN__ const size_t n_count );
__HOT  void distribute_priority( __STATE__ scheduler_t *s, __STATE__ job_list_t *jlist );
__COLD void destroy_priority( __STATE__ scheduler_t *s );
__HOT  void print_priority( __IN__ scheduler_t *s );


#endif // ! __SCHEDULE_ALGORITHMS
