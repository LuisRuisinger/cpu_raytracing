#ifndef CPU_RAYTRACING_SPMC_QUEUE_H
#define CPU_RAYTRACING_SPMC_QUEUE_H

#include "../defines.h"
#include "thread.h"

typedef struct SPMC_Queue_t SPMC_Queue;

SPMC_Queue *spmc_queue_create(usize cnt);
void spmc_queue_destroy(SPMC_Queue *);
void spmc_queue_reset(SPMC_Queue *);
bool spmc_queue_try_pop(SPMC_Queue *queue, Threadpool_work *);
bool spmc_queue_try_push(SPMC_Queue *queue, Threadpool_work *);

#endif //CPU_RAYTRACING_SPMC_QUEUE_H