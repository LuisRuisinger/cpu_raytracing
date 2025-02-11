#ifndef SOFTWARE_RATYTRACING_THREAD_H
#define SOFTWARE_RATYTRACING_THREAD_H

#include <pthread.h> 

#include "../defines.h"

#define CACHE_LINE_SIZE 64

#define LOAD_EXP_ACQ(_a) \
    atomic_load_explicit(&(_a), memory_order_acquire)

#define STORE_EXP_RLX(_a) \
    atomic_store_explicit(&(_a), memory_order_relaxed)

typedef void (*thread_fun) (void *);
typedef struct Threadpool_work_t {
    thread_fun fun;
    void *args;
} Threadpool_work;

u32 hardware_concurrency();

#if defined(__APPLE__)
typedef pthread_mutexattr_t pthread_barrierattr_t;
typedef struct pthread_barrier {
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    u32 cnt;
    u32 lft;
    u32 rnd;
} pthread_barrier_t; 

i32 pthread_barrier_init(
    pthread_barrier_t *__restrict bar, const pthread_barrierattr_t *__restrict attr, u32 cnt);
i32 pthread_barrier_destroy(pthread_barrier_t *bar);
i32 pthread_barrier_wait(pthread_barrier_t *bar); 
i32 pthread_barrierattr_init(pthread_barrierattr_t *attr);
i32 pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
i32 pthread_barrierattr_getpshared(
    const pthread_barrierattr_t *__restrict attr, i32 *__restrict pshared);
i32 pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, i32 pshared );

#endif
#endif //SOFTWARE_RATYTRACING_THREAD_H