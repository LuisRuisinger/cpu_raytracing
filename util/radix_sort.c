#include <assert.h>

#include "radix_sort.h"
#include "datastructures/bitmap.h"
#include "threading/thread.h"
#include "threading/threadpool.h"
#include "fmt.h"

typedef struct Radix_Args_t {
    u64 *src;
    u64 *cpy;
    
    usize *cnt;
    usize *idx;
    usize len;
    usize shift;
} Radix_Args;

void bucket(void *args) {
    Radix_Args *r_args = args;

    for (usize i = 0; i < r_args->len; ++i) {
        u64 bucket = (r_args->src[i] >> r_args->shift) & RADIX_MASK;
        u64 _index = r_args->idx[bucket]++;
        r_args->cpy[_index] = r_args->src[i];
    }
}

void sort_range(void *args) {
    Radix_Args *r_args = args;
    memset(r_args->cnt, 0, RADIX_BUFFER_SIZE * sizeof(usize));

    for (usize i = 0; i < r_args->len; ++i) {
        u64 bucket = (r_args->src[i] >> r_args->shift) & RADIX_MASK; 
        ++r_args->cnt[bucket];
    }

    usize next_index = 0;
    for (usize i = 0; i < RADIX_BUFFER_SIZE; ++i) {
        r_args->idx[i] = next_index;
        next_index += r_args->cnt[i];
    }

    bucket(args);
}

/* TODO */
// fix
// make into type dependant macro
void radix_sort_parallel(u64 *__restrict src, u64 *__restrict cpy, usize len) { 
    usize thread_cnt = 1;
    usize upper_bound_addr = (usize) (src + len);
    usize batch = RND_POW_2(len, thread_cnt) / thread_cnt;

    // setup stage
    usize *cnt = malloc(RADIX_BUFFER_SIZE * thread_cnt * sizeof(usize));
    usize *idx = malloc(RADIX_BUFFER_SIZE * thread_cnt * sizeof(usize));
    Radix_Args args[thread_cnt];

    for (usize j = 0; j < thread_cnt; ++j) {
        usize rem = (upper_bound_addr - (usize) &src[j * batch]) / sizeof(usize);
            
        args[j].len = MIN(rem, batch);
        args[j].cnt = &cnt[j * RADIX_BUFFER_SIZE];
        args[j].idx = &idx[j * RADIX_BUFFER_SIZE];
    }

    // sort stage
    Threadpool *tp = threadpool_create(thread_cnt);

    for (usize i = 0; i < sizeof(u64); ++i) {
        for (usize j = 0; j < thread_cnt; ++j) {
            args[j].src = &src[j * batch];
            args[j].cpy = &cpy[j * batch];
            args[j].shift = i * BYTE_SIZE;

            threadpool_enqueue(tp, sort_range, &args[j]);
        }

        threadpool_wait(tp);
        threadpool_clear_work(tp);

        usize next_idx = 0;
        for (usize j = 0; j < RADIX_BUFFER_SIZE; ++j) {
            for (usize k = 0; k < thread_cnt; ++k) {
                args[k].idx[j] = next_idx;
                next_idx += args[k].cnt[j];
            }
        }

        for (usize j = 0; j < thread_cnt; ++j) {
            args[j].src = args[j].cpy;
            args[j].cpy = src;

            threadpool_enqueue(tp, bucket, &args[j]);
        }

        threadpool_wait(tp);
        threadpool_clear_work(tp);
    }

    free(cnt);
    free(idx);
    threadpool_destroy(tp);
}