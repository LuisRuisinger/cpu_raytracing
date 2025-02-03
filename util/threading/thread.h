#ifndef SOFTWARE_RATYTRACING_THREAD_H
#define SOFTWARE_RATYTRACING_THREAD_H

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

#endif //SOFTWARE_RATYTRACING_THREAD_H