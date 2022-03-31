#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "synch.h"

// locks:

lock_t *lock_create(void) {
    lock_t *l;
    int err;
    
    l = malloc(sizeof(lock_t));
    if (!l)
        goto error_out;
    
    memset(l, 0, sizeof(lock_t));
    err = pthread_mutex_init(&l->l_lock, NULL);
    if (err)
        goto error_out;
    
    return l;
    
error_out:
    return NULL;
}

int lock_destroy(lock_t *l) {
    int err;
    
    err = pthread_mutex_destroy(&l->l_lock);
    if (err)
        goto error_out;
    
    free(l);
    
    return 0;
    
error_out:
    return err;
}

int lock_lock(lock_t *l) {
    return pthread_mutex_lock(&l->l_lock);
}

int lock_unlock(lock_t *l) {
    return pthread_mutex_unlock(&l->l_lock);
}

// rw locks:

rw_lock_t *rwl_create(void) {
    rw_lock_t *rwl;
    int err;
    
    rwl = malloc(sizeof(rw_lock_t));
    if (!rwl)
        goto error_out;
    
    memset(rwl, 0, sizeof(rw_lock_t));
    err = pthread_rwlock_init(&rwl->rwl_lock, NULL);
    if (err)
        goto error_out;
    
    return rwl;
    
error_out:
    return NULL;
}

int rwl_destroy(rw_lock_t *rwl) {
    int err;
    
    err = pthread_rwlock_destroy(&rwl->rwl_lock);
    if (err)
        goto error_out;
    
    free(rwl);
    
    return 0;
    
error_out:
    return err;
}

int rwl_lock_shared(rw_lock_t *rwl) {
    return pthread_rwlock_rdlock(&rwl->rwl_lock);
}

int rwl_lock_exclusive(rw_lock_t *rwl) {
    return pthread_rwlock_wrlock(&rwl->rwl_lock);
}

int rwl_unlock(rw_lock_t *rwl) {
    return pthread_rwlock_unlock(&rwl->rwl_lock);
}

// condition variables:

cv_t *cv_create(void) {
    cv_t *cv;
    int err;
    
    cv = malloc(sizeof(cv_t));
    if (!cv)
        goto error_out;
    
    memset(cv, 0, sizeof(cv_t));
    err = pthread_cond_init(&cv->cv_cond, NULL);
    if (err)
        goto error_out;
    
    return cv;
    
error_out:
    return NULL;
}

int cv_destroy(cv_t *cv) {
    int err;
    
    err = pthread_cond_destroy(&cv->cv_cond);
    if (err)
        goto error_out;
    
    free(cv);
    
    return 0;
    
error_out:
    return err;
}

int cv_wait(cv_t *cv, lock_t *l) {
    return pthread_cond_wait(&cv->cv_cond, &l->l_lock);
}

int cv_signal(cv_t *cv) {
    return pthread_cond_signal(&cv->cv_cond);
}
int cv_broadcast(cv_t *cv) {
    return pthread_cond_broadcast(&cv->cv_cond);
}
