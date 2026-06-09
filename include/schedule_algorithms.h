#ifndef __SCHEDULE_ALGORITHMS
#define __SCHEDULE_ALGORITHMS


#include <cluster/cluster.h>


#define ALGORITHMS_HELP \
	"\n\tРежимы планирования:" \
	"\n\t\tfifo        : обычный FIFO (FCFS)" \
	"\n\t\tpriority    : вытесняющее с приоритетами с EDF-очередью" \
	"\n\t\tbackfill    : backfill (HRB)" \
	"\n\t\tround_robin : round-robin"


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


// Backfill -------------------------------------------

typedef struct _backfill_moment_t {
	
} backfill_moment_t;

typedef struct _backfill_timetable_t {
	
} backfill_timetable_t;

__HOT  void schedule_backfill( __STATE__ scheduler_t *s, __IN__ node_t* *n, __IN__ const size_t n_count );
__COLD uint32_t init_backfill( __STATE__ scheduler_t *s, __IN__ node_t* *n, __IN__ const size_t n_count );
__HOT  void distribute_backfill( __STATE__ scheduler_t *s, __STATE__ job_list_t *jlist );
__COLD void destroy_backfill( __STATE__ scheduler_t *s );
__HOT  void print_backfill( __IN__ scheduler_t *s );


// Round-robin -------------------------------------------

#define ROUND_ROBIN_TIME_TO_RUN 20

typedef struct _rr_job_ll_t {
	quantum_t   stop_in;
	struct _rr_job_ll_t *next;
	struct _rr_job_ll_t *prev;
	job_list_t *entry;
} rr_job_ll_t;

typedef struct _rr_job_queues_t {
	rr_job_ll_t *running;
	rr_job_ll_t *pending;
	rr_job_ll_t *finished;
} rr_job_queues_t;

__HOT  void schedule_round_robin( __STATE__ scheduler_t *s, __IN__ node_t* *n, __IN__ const size_t n_count );
__COLD uint32_t init_round_robin( __STATE__ scheduler_t *s, __IN__ node_t* *n, __IN__ const size_t n_count );
__HOT  void distribute_round_robin( __STATE__ scheduler_t *s, __STATE__ job_list_t *jlist );
__COLD void destroy_round_robin( __STATE__ scheduler_t *s );
__HOT  void print_round_robin( __IN__ scheduler_t *s );


#endif // ! __SCHEDULE_ALGORITHMS
