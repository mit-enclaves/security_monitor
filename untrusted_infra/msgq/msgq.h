#ifndef MSGQ_H
#define MSGQ_H

#include <platform_lock.h>

#define SIZE_QUEUE 64

typedef struct queue_t {
  void *buf[SIZE_QUEUE];
  int head;
  int tail;
  platform_lock_t lock;
} queue_t;

bool is_empty(queue_t *q);
bool is_full(queue_t *q);
int push(queue_t *q, void *m);
int pop(queue_t *q, void **ret);

#endif // MSGQ_H
