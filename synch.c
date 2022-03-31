#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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

// threads:

thread_t *thread_create(const char *tname, int (*start_fn)(thread_ctx_t *), void *start_arg) {
    thread_t *t;
    
    t = malloc(sizeof(thread_t));
    if (!t)
        goto error_out;
    
    memset(t, 0, sizeof(thread_t));
    
    t->t_name = malloc(strlen(tname) + 1);
    if (!t->t_name)
        goto error_out;
    
    strcpy(t->t_name, tname);
    
    t->t_start_fn = start_fn;
    
    t->t_ctx = malloc(sizeof(thread_ctx_t));
    if (!t->t_ctx)
        goto error_out;
    
    memset(t->t_ctx, 0, sizeof(thread_ctx_t));
    t->t_ctx->tc_thread = t;
    t->t_ctx->tc_start_arg = start_arg;
    
    return t;
    
error_out:
    if (t) {
        if (t->t_name)
            free(t->t_name);
        free(t);
    }
    
    return NULL;
}

thread_t *thread_create_and_start(const char *tname, int (*start_fn)(thread_ctx_t *), void *start_arg) {
    thread_t *t = NULL;
    int err;
    
    t = thread_create(tname, start_fn, start_arg);
    if (!t)
        goto error_out;
    
    err = thread_start(t);
    if (err)
        goto error_out;
    
    return t;
    
error_out:
    if (t)
        free(t);
    
    return NULL;
}

void thread_destroy(thread_t *t) {
    free(t->t_ctx);
    free(t->t_name);
    free(t);
}

static void *_thread_start(void *arg) {
    thread_t *t = (thread_t *)arg;
    int err;
    
    err = t->t_start_fn(t->t_ctx);
    if (err)
        goto error_out;
    
    return NULL;
    
error_out:
    return (void *)(intptr_t)err;
}

int thread_start(thread_t *t) {
    int err;
    
    err = pthread_create(&t->t_pthread, NULL, _thread_start, (void *)t);
    if (err)
        goto error_out;
    
    return 0;
    
error_out:
    return err;
}

int thread_wait(thread_t *t, int *ret) {
    int err;
    void *tret = NULL;
    
    err = pthread_join(t->t_pthread, &tret);
    if (err)
        goto error_out;
    
    *ret = (int)(intptr_t)tret;
    
    return 0;
    
error_out:
    return err;
}
