#ifndef CPU_RAYTRACING_THREADPOOL_H
#define CPU_RAYTRACING_THREADPOOL_H

#include <stdbool.h>

#include "thread.h"
#include "../defines.h"

typedef struct Threadpool_t Threadpool;

Threadpool *threadpool_create(usize); 

void threadpool_destroy(Threadpool *tp);
void threadpool_enqueue(Threadpool *tp, thread_fun fun, void *args);
void threadpool_wait(Threadpool *tp);
void threadpool_clear_work(Threadpool *tp);
bool threadpool_no_tasks(Threadpool *tp);

#endif //CPU_RAYTRACING_THREADPOOL_H