#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

#include "threadpool.h"
#include "spmc_queue.h"
#include "../datastructures/dynamic_array.h"
#include "../datastructures/pair.h"

typedef struct Threadpool_t {
    usize thread_cnt;
    
    ARRAY(pthread_t) threads;
    ARRAY(SPMC_Queue *) queues;

    _Atomic(usize) enq_task_cnt;
    _Atomic(usize) act_task_cnt;
    _Atomic(usize) enq_task_idx;
    _Atomic(bool) run_flag;

    pthread_mutex_t mutex;
    pthread_cond_t tasks_available;
    pthread_cond_t tasks_finished;
} Threadpool;

static __thread Thread thread_mdata; 

void *threadpool_worker(void *args) {
    Threadpool *tp = ((PAIR(Threadpool *, Thread) *) args)->p0;
    thread_mdata   = ((PAIR(Threadpool *, Thread) *) args)->p1;
    free(args);

    DEBUG_LOG("Worker %d running", thread_mdata.id);
    Threadpool_work work;

    for (;;) {
        pthread_mutex_lock(&tp->mutex);
        while (!LOAD_EXP_ACQ(tp->enq_task_cnt) && LOAD_EXP_ACQ(tp->run_flag)) {
            pthread_cond_wait(&tp->tasks_available, &tp->mutex);
        }

        pthread_mutex_unlock(&tp->mutex);
        if (!LOAD_EXP_ACQ(tp->run_flag)) {
            break;
        }

        LOG("gu");

        for (;;) {
            for (usize i = 0; i < tp->thread_cnt; ++i) {
                usize queue_idx = (thread_mdata.id + i) % tp->queues.size;
                SPMC_Queue *queue = ARRAY_ELEMENT(tp->queues, queue_idx);
                
                if (spmc_queue_try_pop(queue, &work)) {
                    LOG("found work");

                    atomic_fetch_sub_explicit(&tp->enq_task_cnt, 1, memory_order_relaxed);
                    work.fun(work.args);
                    atomic_fetch_sub_explicit(&tp->act_task_cnt, 1, memory_order_relaxed);
                }
            }

            if (!LOAD_EXP_ACQ(tp->enq_task_cnt)) {
                if (!LOAD_EXP_ACQ(tp->act_task_cnt)) {
                    pthread_cond_broadcast(&tp->tasks_finished);
                }

                break;
            }
        }
    }

    return NULL;
}

Threadpool *threadpool_create(usize cnt) {
    Threadpool *tp = malloc(sizeof(Threadpool));
    assert(tp);

    ARRAY_INIT_EXPLICIT(tp->threads, cnt);
    ARRAY_INIT_EXPLICIT(tp->queues, cnt);
    tp->thread_cnt = cnt;

    for (usize i = 0; i < tp->thread_cnt; ++i) {
        ARRAY_PUSH_BACK(tp->queues, spmc_queue_create(128));
    }

    DEBUG_LOG("Threadpool queues init finished ");

    tp->enq_task_cnt = 0;
    tp->act_task_cnt = 0;
    tp->enq_task_idx = 0;
    tp->run_flag = true;

    pthread_mutex_init(&tp->mutex, NULL);
    pthread_cond_init(&tp->tasks_available, NULL);
    pthread_cond_init(&tp->tasks_finished, NULL);

    ARRAY_RESIZE(tp->threads, tp->thread_cnt);
    usize thread_id = 0;

    ARRAY_TYPEOF(tp->threads) *thread = NULL;
    ARRAY_FOREACH(tp->threads, thread) {
        PAIR(Threadpool *, Thread) *args = malloc(sizeof(*args));
        args->p0 = tp;
        args->p1 = (Thread) { .id = thread_id, .thread_cnt = tp->thread_cnt };

        pthread_create(thread, NULL, threadpool_worker, args);
        ++thread_id;
    }

    DEBUG_LOG("Threadpool init finished");
    return tp;
}

void threadpool_destroy(Threadpool *tp) {
    assert(tp);
    
    tp->run_flag = false;
    pthread_cond_broadcast(&tp->tasks_available);
    threadpool_wait(tp);
    
    pthread_t *thread = NULL;
    ARRAY_FOREACH(tp->threads, thread) {
        (void) pthread_join(*thread, NULL);
    }

    DEBUG_LOG("Threadpool threads destroy finished");

    SPMC_Queue **queue = NULL;
    ARRAY_FOREACH(tp->queues, queue) {
        spmc_queue_destroy(*queue);
    }

    DEBUG_LOG("Threadpool queues destroy finished");

    ARRAY_FREE(tp->threads);
    ARRAY_FREE(tp->queues);

    pthread_mutex_destroy(&tp->mutex);
    pthread_cond_destroy(&tp->tasks_available);
    pthread_cond_destroy(&tp->tasks_finished);

    free(tp);
}

void threadpool_clear_work(Threadpool *tp) {
    assert(tp);
    
    ARRAY_TYPEOF(tp->queues) *queue = NULL;
    ARRAY_FOREACH(tp->queues, queue) {
        spmc_queue_reset(*queue);
    }

    DEBUG_LOG("Queues cleared");
}

bool threadpool_no_tasks(Threadpool *tp) {
    assert(tp);
    return !LOAD_EXP_ACQ(tp->enq_task_cnt) && !LOAD_EXP_ACQ(tp->act_task_cnt);
}

void threadpool_wait(Threadpool *tp) {
    assert(tp);
    
    pthread_mutex_lock(&tp->mutex);
    while (!threadpool_no_tasks(tp)) {
        pthread_cond_wait(&tp->tasks_finished, &tp->mutex);
    }

    pthread_mutex_unlock(&tp->mutex);
}

bool threadpool_try_schedule(Threadpool *tp, thread_fun fun, void *args) {
    assert(tp);
    assert(fun);

    const Threadpool_work work = (Threadpool_work) { .fun = fun, .args = args };
    const u32 base_idx = 
        atomic_fetch_add_explicit(&tp->enq_task_idx, 1, memory_order_acquire);

    for (usize i = 0; i < tp->thread_cnt; ++i) {
        const usize queue_idx = (base_idx + i) % tp->thread_cnt;
        SPMC_Queue *queue = ARRAY_ELEMENT(tp->queues, queue_idx);

        if (spmc_queue_try_push(queue, &work)) {

            // reverse order
            atomic_fetch_add_explicit(&tp->act_task_cnt, 1, memory_order_release);
            atomic_fetch_add_explicit(&tp->enq_task_cnt, 1, memory_order_release);
            pthread_cond_signal(&tp->tasks_available);

            return true;
        }
    }

    return false;
}

void threadpool_enqueue(Threadpool *tp, thread_fun fun, void *args) {
    
    // retry 
    while (!threadpool_try_schedule(tp, fun, args)); 
}

const Thread *threadpool_mdata() {
    return &thread_mdata;
}