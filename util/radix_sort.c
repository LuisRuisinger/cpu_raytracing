#include "radix_sort.h"
#include "datastructures/bitmap.h"
#include "threading/thread.h"
#include "threading/threadpool.h"

#define BITS_PER_ITERATION 8 

typedef struct Radix_Args_t {
    u8 *src;
    u8 *cpy;

    u32 shift;
    u32 len;
    u32 *cnt;
    u32 *idx;
} Radix_Args;

#define RADIX_MASK \
    ((1ULL << BYTE_SIZE) - 1)

#define RADIX_BUFFER_SIZE \
    (1ULL << BYTE_SIZE)

#define RND_POW_2(_n, _p) \
    ((_n) + (_p) - 1) & ~((_p) - 1)

#define TRESHOLD 1024

void *sort_range(void *args) {
    Radix_Args *r_args = args;
    memset(r_args->cnt, 0, r_args->cnt);

    for (usize i = 0; i < r_args->len; ++i) {
        u32 bucket = (r_args->src[i] >> r_args->shift) & RADIX_MASK; 
        ++r_args->cnt[bucket];
    }

    usize next_index = 0;
    for (usize i = 0; i < r_args->cnt; ++i) {
        r_args->idx[i] = next_index;
        next_index += r_args->cnt[i];
    }

    for (usize i = 0; i < r_args->len; ++i) {
        u32 bucket = (r_args->src[i] >> r_args->shift) & RADIX_MASK;
        u32 _index = r_args->idx[bucket]++;
        r_args->cpy[_index] = r_args->src[i];
    }

    return NULL;
}

void radix_sort(u8 *__restrict keys, u8 *__restrict keys_cpy, usize bit_cnt, usize len);

void radix_sort_parallel(u8 *__restrict src, u8 *__restrict cpy, usize bit_cnt, usize len) {
    if (len < TRESHOLD) {
        radix_sort(src, cpy, bit_cnt, len);
        return;
    }

    usize byte_cnt = bit_cnt / BYTE_SIZE;   
    usize thread_cnt = MAX(2, hardware_concurrency());
    Threadpool *tp = threadpool_create(thread_cnt);

    usize args_size = RND_POW_2(thread_cnt * sizeof(Radix_Args), byte_cnt);
    usize cnt_size  = RADIX_BUFFER_SIZE * thread_cnt * byte_cnt;
    usize idx_size  = RADIX_BUFFER_SIZE * thread_cnt * byte_cnt;

    u8 *mem = malloc(args_size + cnt_size + idx_size);

    usize upper_bound_addr = (usize) src + (byte_cnt * len);
    usize batch = RND_POW_2(len, thread_cnt);
    for (usize i = 0; i < byte_cnt; ++i) {
        for (usize j = 0; j < thread_cnt; ++j) {
            Radix_Args *args = (Radix_Args *) mem + j;
            args->len = MIN(upper_bound_addr - (usize) args->src, batch);
            args->src = (u8 *) src + (byte_cnt * batch * j);
            args->cpy = (u8 *) cpy + (byte_cnt * batch * j);

            usize base_off_cnt = args_size;
            usize base_off_idx = args_size + cnt_size;
            args->cnt = (u8 *) mem + base_off_cnt + (batch * j);
            args->idx = (u8 *) mem + base_off_idx + (batch * j);

            args->shift = i * BYTE_SIZE;
            threadpool_enqueue(tp, sort_range, args);
        }

        threadpool_wait(tp);

        usize next_idx = 0;
        for (usize j = 0; j < RADIX_BUFFER_SIZE; ++j) {
            for (usize k = 0; k < thread_cnt; ++k) {
                Radix_Args *args = (Radix_Args *) mem + k;

                args->idx[j] = next_idx;
                next_idx += args->cnt[j];
            }
        }

        for (usize j = 0; j < thread_cnt; ++j) {
            Radix_Args *args = (Radix_Args *) mem + j;
            args->len = MIN(upper_bound_addr - (usize) args->src, batch);
            
            // swap without temporary 
            // kinda useless with modern compilers
            args->src = (u8 *) ((usize) args->src ^ (usize) args->cpy);
            args->cpy = (u8 *) ((usize) args->src ^ (usize) args->cpy);
            args->src = (u8 *) ((usize) args->src ^ (usize) args->cpy);

            usize base_off_cnt = args_size;
            usize base_off_idx = args_size + cnt_size;
            args->cnt = (u8 *) mem + base_off_cnt + (batch * j);
            args->idx = (u8 *) mem + base_off_idx + (batch * j);

            args->shift = i * BYTE_SIZE;
            threadpool_enqueue(tp, sort_range, args);
        }

        threadpool_wait(tp);
    }

    free(mem);
    threadpool_destroy(tp);
}