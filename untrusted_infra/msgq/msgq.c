#include "msgq.h"
#include <stddef.h>

void init_q(queue_t *q) {
  q->head = 0;
  q->tail = 0;
  platform_lock_release(&q->lock);
}

bool is_empty(queue_t *q) {
  return q->tail == q->head;
}

bool is_full(queue_t *q) {
  return q->tail == ((q->head + 1) % SIZE_QUEUE);
}

int push(queue_t *q, void *m) {
  while(!platform_lock_acquire(&q->lock)) {};

  if(is_full(q)) {
    platform_lock_release(&q->lock);
    return 1; 
  }
  
  q->buf[q->tail] = m;
  q->tail = (q->tail + SIZE_QUEUE - 1) % SIZE_QUEUE;

  platform_lock_release(&q->lock);

  return 0;
}

int pop(queue_t *q, void **ret) {
  while(!platform_lock_acquire(&q->lock)) {};
  
  if(is_empty(q)) {
    platform_lock_release(&q->lock);
    *ret = NULL;
    return 1; 
  }
  
  *ret =  q->buf[q->head];
  q->head = (q->head + SIZE_QUEUE - 1) % SIZE_QUEUE;

  platform_lock_release(&q->lock);

  return 0;
}

