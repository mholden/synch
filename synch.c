#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "synch.h"

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
