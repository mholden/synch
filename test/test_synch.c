#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "synch.h"

typedef struct test_synch_data {
    lock_t *tsd_lock;
    int tsd_data;
} test_synch_data_t;

static void *_test_locks(void *arg) {
    test_synch_data_t *tsd = (test_synch_data_t *)arg;
    
    printf("  _test_locks: thread %p\n", pthread_self());
    
    assert(lock_lock(tsd->tsd_lock) == 0);
    
    printf("  _test_locks: thread %p in critical section\n", pthread_self());
    sleep(5);
    
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

int main(void) {
    test_locks();
    return 0;
}
