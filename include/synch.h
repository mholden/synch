#ifndef _SYNCH_H_
#define _SYNCH_H_

#include <pthread.h>

typedef struct lock {
    pthread_mutex_t l_lock;
} lock_t;

lock_t *lock_create(void);
int lock_destroy(lock_t *l);
int lock_lock(lock_t *l);
int lock_unlock(lock_t *l);

#endif // _SYNCH_H_
