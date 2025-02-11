#include <pthread.h>
#include <errno.h>

#include "thread.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <sysinfoapi.h>
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
    #if defined(__APPLE__)
        #include <sys/sysctl.h>
    #else
        #include <unistd.h>
    #endif
#endif

u32 hardware_concurrency() {
    i32 max_threads = 0;

#if defined(_WIN32) || defined(_WIN64)
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    max_threads = sysInfo.dwNumberOfProcessors;

#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
#if defined(__APPLE__)
    i32 tmp[2] = { CTL_HW, HW_NCPU };
    usize len = sizeof(max_threads);

    if (sysctl(tmp, 2, &max_threads, &len, NULL, 0) != 0) {
        return 0;
    }

#else
    max_threads = sysconf(_SC_NPROCESSORS_ONLN);

#endif
#endif

    return max_threads > 0 ? max_threads : 0;
}

#if defined(__APPLE__)
i32 pthread_barrier_init(
    pthread_barrier_t *__restrict bar, const pthread_barrierattr_t *__restrict attr, u32 cnt) {
    if (cnt == 0) {
        return EINVAL;
    }

    pthread_condattr_t condattr;
    pthread_condattr_init(&condattr);

    i32 ret;
    if (attr) {
        i32 pshared;

        if (ret = pthread_barrierattr_getpshared(attr, &pshared)) {
            return ret;
        }

        if (ret = pthread_condattr_setpshared(&condattr, pshared)) {
            return ret;
        }
    }

    if (ret = pthread_mutex_init(&bar->mutex, attr)) {
        return ret;
    }

    if (ret = pthread_cond_init(&bar->cond, &condattr)) {
        pthread_mutex_destroy(&bar->mutex);
        return ret;
    }

    bar->cnt = cnt;
    bar->lft = cnt;
    bar->rnd = 0;

    return 0;
}

i32 pthread_barrier_destroy(pthread_barrier_t *bar) {
    if (bar->cnt == 0) {
        return EINVAL;
    }

    bar->cnt = 0;
    return pthread_mutex_destroy(&bar->mutex) | pthread_cond_destroy(&bar->cond);
}

i32 pthread_barrier_wait(pthread_barrier_t *bar) {
    pthread_mutex_lock(&bar->mutex);

    if (--bar->lft) {
        u32 rnd = bar->rnd;

        do {
            pthread_cond_wait(&bar->cond, &bar->mutex);
        }
        while (rnd == bar->rnd);
        pthread_mutex_unlock(&bar->mutex);

        return 0;
    }
    else {
        ++bar->rnd;
        bar->lft = bar->cnt;

        pthread_cond_broadcast(&bar->cond);
        pthread_mutex_unlock(&bar->mutex);

        return -1;
    }
}

i32 pthread_barrierattr_init(pthread_barrierattr_t *attr) {
    return pthread_mutexattr_init(attr);
}

i32 pthread_barrierattr_destroy(pthread_barrierattr_t *attr) {
    return pthread_mutexattr_destroy(attr);
}

i32 pthread_barrierattr_getpshared(
    const pthread_barrierattr_t *__restrict attr, i32 *__restrict pshared) {
    return pthread_mutexattr_getpshared(attr, pshared);
}

i32 pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, i32 pshared ) {
    return pthread_barrierattr_setpshared(attr, pshared);
}

#endif