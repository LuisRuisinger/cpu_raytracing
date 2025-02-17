#include <assert.h>
#include <stdatomic.h>

#include "spmc_queue.h"
#include "../datastructures/dynamic_array.h"

typedef struct SPMC_Queue_t {
    _Atomic(usize) beg;
    _Atomic(usize) end;

    usize init;
    usize mask;  

    alignas(CACHE_LINE_SIZE) ARRAY(Threadpool_work) buffer;
} SPMC_Queue;

SPMC_Queue *spmc_queue_create(usize cnt) {
    SPMC_Queue *queue = malloc(sizeof(*queue));
    assert(queue);

    atomic_store_explicit(&queue->beg, 0, memory_order_relaxed);
    atomic_store_explicit(&queue->end, 0, memory_order_relaxed);

    queue->init = cnt;
    queue->mask = cnt - 1;  

    ARRAY_INIT(queue->buffer, cnt);
    return queue;
}

bool spmc_queue_try_pop(SPMC_Queue *queue, Threadpool_work *work) {
    assert(queue && work);
    
    usize beg = atomic_load_explicit(&queue->beg, memory_order_relaxed);
    if (beg == atomic_load_explicit(&queue->end, memory_order_acquire)) {
        return false;
    }
        
    if (atomic_compare_exchange_weak_explicit(
            &queue->beg,
            &beg,
            (beg + 1) & queue->mask,
            memory_order_acquire,
            memory_order_relaxed)) {
        *work = ARRAY_ELEMENT(queue->buffer, beg & queue->mask);
        return true;
    }

    return false;
}

bool spmc_queue_try_push(SPMC_Queue *queue, Threadpool_work *work) {
    assert(queue && work);
    
    usize end = atomic_load_explicit(&queue->end, memory_order_relaxed);
    usize next_end = (end + 1) & queue->mask;
    
    if (next_end == atomic_load_explicit(&queue->beg, memory_order_acquire)) {
        return false; 
    }
    
    ARRAY_ELEMENT(queue->buffer, end) = *work;
    
    atomic_store_explicit(&queue->end, next_end, memory_order_release);
    return true;
}

void spmc_queue_destroy(SPMC_Queue *queue) {
    assert(queue);

    ARRAY_FREE(queue->buffer);
    free(queue);
}

void spmc_queue_reset(SPMC_Queue *queue) {
    assert(queue);
    
    atomic_store_explicit(&queue->beg, 0, memory_order_relaxed);
    atomic_store_explicit(&queue->end, 0, memory_order_relaxed);
    ARRAY_RESIZE(queue->buffer, queue->init);
    ARRAY_CLEAR(queue->buffer);
}