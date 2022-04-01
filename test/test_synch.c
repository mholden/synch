#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "synch.h"

typedef struct test_synch_data {
    lock_t *tsd_lock;
    rw_lock_t *tsd_rwlock;
    cv_t *tsd_cv;
    bool tsd_initd;
    pthread_t tsd_writer;
} test_synch_data_t;

static void *_test_locks(void *arg) {
    test_synch_data_t *tsd = (test_synch_data_t *)arg;
    
    printf("  _test_locks: thread %p\n", pthread_self());
    
    assert(lock_lock(tsd->tsd_lock) == 0);
    
    printf("  _test_locks: thread %p in critical section\n", pthread_self());
    sleep(3);
    
    assert(lock_unlock(tsd->tsd_lock) == 0);
    
    printf("  _test_locks: thread %p out of critical section\n", pthread_self());

    return NULL;
}

static void test_locks(void) {
    test_synch_data_t tsd;
    pthread_t t1, t2;
    
    printf("test_locks...\n");
    
    memset(&tsd, 0, sizeof(test_synch_data_t));
    assert(tsd.tsd_lock = lock_create());
    
    assert(pthread_create(&t1, NULL, _test_locks, &tsd) == 0);
    assert(pthread_create(&t2, NULL, _test_locks, &tsd) == 0);
    
    assert(pthread_join(t1, NULL) == 0);
    assert(pthread_join(t2, NULL) == 0);
    
    assert(lock_destroy(tsd.tsd_lock) == 0);
}

static void *_test_cvs(void *arg) {
    test_synch_data_t *tsd = (test_synch_data_t *)arg;
    
    assert(lock_lock(tsd->tsd_lock) == 0);
    
    while (!tsd->tsd_initd) { // don't start until all threads have been set up
        printf("  _test_cvs: initd isn't set yet; cv_wait'ing until it is\n");
        cv_wait(tsd->tsd_cv, tsd->tsd_lock);
    }
    
    printf("  _test_cvs: initd is set now\n");
    
    assert(lock_unlock(tsd->tsd_lock) == 0);

    return NULL;
}

static void test_cvs(void) {
    test_synch_data_t tsd;
    pthread_t t;
    
    printf("test_cvs...\n");
    
    memset(&tsd, 0, sizeof(test_synch_data_t));
    assert(tsd.tsd_lock = lock_create());
    assert(tsd.tsd_cv = cv_create());

    assert(pthread_create(&t, NULL, _test_cvs, &tsd) == 0);
    
    sleep(3);
    
    assert(lock_lock(tsd.tsd_lock) == 0);
    tsd.tsd_initd = true;
    printf("  test_cvs: setup complete\n");
    assert(lock_unlock(tsd.tsd_lock) == 0);
    cv_broadcast(tsd.tsd_cv);
    
    assert(pthread_join(t, NULL) == 0);
    
    assert(lock_destroy(tsd.tsd_lock) == 0);
    assert(cv_destroy(tsd.tsd_cv) == 0);
}

static void *_test_rwlocks(void *arg) {
    test_synch_data_t *tsd = (test_synch_data_t *)arg;
    bool is_writer = false;
    
    assert(lock_lock(tsd->tsd_lock) == 0);
   
    while (!tsd->tsd_initd) // don't start until all threads have been set up
        cv_wait(tsd->tsd_cv, tsd->tsd_lock);
    
    if (pthread_self() == tsd->tsd_writer)
        is_writer = true;
    
    assert(lock_unlock(tsd->tsd_lock) == 0);
    
    printf("  _test_rwlocks: thread %p (%s)\n", pthread_self(), is_writer ? "writer" : "reader" );
    
    if (is_writer)
        assert(rwl_lock_exclusive(tsd->tsd_rwlock) == 0);
    else
        assert(rwl_lock_shared(tsd->tsd_rwlock) == 0);
    
    printf("  _test_rwlocks: thread %p (%s) in critical section\n", pthread_self(), is_writer ? "writer" : "reader");
    sleep(3);
    
    assert(rwl_unlock(tsd->tsd_rwlock) == 0);
    
    printf("  _test_rwlocks: thread %p (%s) out of critical section\n", pthread_self(), is_writer ? "writer" : "reader");

    return NULL;
}

static void test_rwlocks(void) {
    test_synch_data_t tsd;
    pthread_t t[4];
    
    printf("test_rwlocks...\n");
    
    memset(&tsd, 0, sizeof(test_synch_data_t));
    assert(tsd.tsd_lock = lock_create());
    assert(tsd.tsd_rwlock = rwl_create());
    assert(tsd.tsd_cv = cv_create());
    
    for (int i = 0; i < 4; i++)
        assert(pthread_create(&t[i], NULL, _test_rwlocks, &tsd) == 0);
    
    assert(lock_lock(tsd.tsd_lock) == 0);
    tsd.tsd_writer = t[rand() % 4];
    tsd.tsd_initd = true; // flag thread's that all have been created
    printf("  test_rwlocks: setup complete\n");
    assert(lock_unlock(tsd.tsd_lock) == 0);
    cv_broadcast(tsd.tsd_cv);
    
    for (int i = 0; i < 4; i++)
        assert(pthread_join(t[i], NULL) == 0);
    
    assert(lock_destroy(tsd.tsd_lock) == 0);
    assert(rwl_destroy(tsd.tsd_rwlock) == 0);
    assert(cv_destroy(tsd.tsd_cv) == 0);
}

typedef struct test_threads_start_arg {
    thread_t *t;
    int ret;
} test_threads_start_arg_t;

static int _test_threads(void *arg) {
    test_threads_start_arg_t *targ = (test_threads_start_arg_t *)arg;
    thread_t *t = targ->t;
    int ret = targ->ret;
    
    ret++;
    printf("  _test_threads: thread %s returning %d\n", t->t_name, ret);
    
    return ret;
}

static void test_threads() {
    thread_t *t1, *t2;
    test_threads_start_arg_t targ1, targ2;
    int tret1 = 0, tret2 = 1;
    
    printf("test_threads...\n");
    
    assert(t1 = thread_create("test_synch_thread1"));
    
    memset(&targ1, 0, sizeof(test_threads_start_arg_t));
    targ1.t = t1;
    targ1.ret = tret1;
    assert(thread_start(t1, _test_threads, &targ1) == 0);
    
    assert((thread_wait(t1, &tret1) == 0) && (tret1 == 1));
    
    assert(t2 = thread_create("test_synch_thread2"));
    
    memset(&targ2, 0, sizeof(test_threads_start_arg_t));
    targ2.t = t2;
    targ2.ret = tret2;
    assert(thread_start(t1, _test_threads, &targ2) == 0);
    
    assert((thread_wait(t1, &tret2) == 0) && (tret2 == 2));
    
    thread_destroy(t1);
    thread_destroy(t2);
}

int main(void) {
    test_threads();
    test_locks();
    test_cvs();
    test_rwlocks();
    return 0;
}
