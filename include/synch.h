#ifndef _SYNCH_H_
#define _SYNCH_H_

#include <pthread.h>

// locks:

#define LOCK_INITIALIZER                    \
{                                           \
    .l_lock = PTHREAD_MUTEX_INITIALIZER     \
}

typedef struct lock {
    pthread_mutex_t l_lock;
} lock_t;

lock_t *lock_create(void);
int lock_init(lock_t *l);
int lock_destroy(lock_t *l);
int lock_lock(lock_t *l);
int lock_unlock(lock_t *l);

// rw locks:

typedef struct rwlock {
    pthread_rwlock_t rwl_lock;
} rw_lock_t;

rw_lock_t *rwl_create(void);
int rwl_destroy(rw_lock_t *rwl);
int rwl_lock_shared(rw_lock_t *rwl);
int rwl_lock_exclusive(rw_lock_t *rwl);
int rwl_unlock(rw_lock_t *rwl);

// condition variables

typedef struct cv {
    pthread_cond_t cv_cond;
} cv_t;

cv_t *cv_create(void);
int cv_destroy(cv_t *cv);
int cv_wait(cv_t *cv, lock_t *l);
int cv_timedwait(cv_t *cv, lock_t *l, struct timespec *t);
int cv_signal(cv_t *cv);
int cv_broadcast(cv_t *cv);

// threads

typedef struct thread_start_ctx {
    int (*start_fn)(void *start_arg);
    void *start_arg;
} thread_start_ctx_t;

typedef struct thread {
    char *t_name;
    thread_start_ctx_t t_start_ctx;
    pthread_t t_pthread;
} thread_t;

thread_t *thread_create(const char *tname);
thread_t *thread_create_and_start(const char *tname, int (*start_fn)(void *start_arg), void *start_arg);
void thread_destroy(thread_t *t);
int thread_start(thread_t *t, int (*start_fn)(void *start_arg), void *start_arg);
int thread_wait(thread_t *t, int *ret);

#endif // _SYNCH_H_
