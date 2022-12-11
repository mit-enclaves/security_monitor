#ifndef MSGQ_H
#define MSGQ_H

#include <platform_lock.h>

#define SIZE_QUEUE 64

typedef struct queue_t {
  void *buf[SIZE_QUEUE];
  volatile int head;
  volatile int tail;
  volatile platform_lock_t lock;
} queue_t;

void init_q(queue_t *q);
bool is_empty(queue_t *q);
bool is_full(queue_t *q);
int push(queue_t *q, void *m);
int pop(queue_t *q, void **ret);

#endif // MSGQ_H
