#ifndef __SCHEDULE_ALGORITHMS
#define __SCHEDULE_ALGORITHMS


#include <cluster/cluster.h>


// итого каждый алгоритм должен делать 3 вещи:
// 1. init_*     - инициализация очереди
// 2. shedule_*  - распределение задач по узлам
// 3. fetch_*    - брать пачку последних задач и реорганизовывать очередь

// + должна быть доп. структура, определяющая
//   как будет организована очередь
//   это типо поле specs в job_queue_t

typedef struct __fifo_job_queue_t {
	
} fifo_job_queue_t;


uint32_t schedule_fifo( cluster_t *c );
uint32_t init_fifo( job_queue_t *q );
job_t *fetch_fifo( job_queue_t *q );


#endif // ! __SCHEDULE_ALGORITHMS
