#include <assert.h>
#include <stdatomic.h>

#include "spmc_queue.h"
#include "../datastructures/dynamic_array.h"

typedef struct SPMC_Queue_t {
    _Atomic(usize) beg;
    _Atomic(usize) end;
    usize init;

    /* TODO */
    // is the cache alignment here really correct ? 
    alignas(CACHE_LINE_SIZE) ARRAY(Threadpool_work) buffer;
} SPMC_Queue;

SPMC_Queue *spmc_queue_create(usize cnt) {
    SPMC_Queue *queue = malloc(sizeof(*queue));
    assert(queue);

    queue->beg = 0;
    queue->end = 0;
    queue->init = cnt;

    ARRAY_INIT(queue->buffer, cnt);
    return queue;
}

void spmc_queue_destroy(SPMC_Queue *queue) {
    assert(queue);
    
    ARRAY_FREE(queue->buffer);
    free(queue);
}

void spmc_queue_reset(SPMC_Queue *queue) {
    assert(queue);
    
    queue->beg = 0;
    queue->end = 0;

    ARRAY_RESIZE(queue->buffer, queue->init);
    ARRAY_CLEAR(queue->buffer);
}

bool spmc_queue_try_pop(SPMC_Queue *queue, Threadpool_work *work) {
    usize _beg = atomic_load_explicit(&queue->beg, memory_order_relaxed);
    if (_beg == atomic_load_explicit(&queue->end, memory_order_acquire)) {
        return false;
    }

    bool succ = atomic_compare_exchange_weak_explicit(
        &queue->beg, &_beg, _beg + 1, memory_order_release, memory_order_acquire);
    
    if (succ) {
        *work = ARRAY_ELEMENT(queue->buffer, _beg);
    }

    return succ;
}

bool spmc_queue_try_push(SPMC_Queue *queue, Threadpool_work *work) {
    usize _end = atomic_load_explicit(&queue->end, memory_order_relaxed);
    if (_end + 1 == atomic_load_explicit(&queue->beg, memory_order_acquire)) {
        return false;
    }

    // storing work before inc idx
    // UB if another thread accesses the job before it has been enqueued
    ARRAY_ELEMENT(queue->buffer, _end) = *work;

    // no CAS needed because of single producer
    // also I think we can keep it relaxed because the try_pop makes all writes visible
    atomic_store_explicit(&queue->end, _end + 1, memory_order_relaxed);
    return true;
}