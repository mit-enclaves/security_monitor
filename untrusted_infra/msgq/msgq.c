#include "msgq.h"
#include <stddef.h>

void init_q(queue_t *q) {
  asm volatile("fence");
  q->head = 0;
  q->tail = 0;
  asm volatile("fence");
  platform_lock_release(&q->lock);
}

bool _is_empty(queue_t *q) {
  return q->tail == q->head;
}

bool _is_full(queue_t *q) {
  return q->tail == ((q->head + 1) % SIZE_QUEUE);
}

bool is_empty(queue_t *q) {
  while(!platform_lock_acquire(&q->lock)) {};
  bool ret = _is_empty(q);
  platform_lock_release(&q->lock);
  return ret;
}

bool is_full(queue_t *q) {
  while(!platform_lock_acquire(&q->lock)) {};
  bool ret = _is_full(q);
  platform_lock_release(&q->lock);
  return ret;
}

int push(queue_t *q, void *m) {
  while(!platform_lock_acquire(&q->lock)) {};

  if(_is_full(q)) {
    platform_lock_release(&q->lock);
    return 1; 
  }
  
  asm volatile("fence");
  q->buf[q->tail] = m;
  q->tail = (q->tail + SIZE_QUEUE - 1) % SIZE_QUEUE;
  asm volatile("fence");

  platform_lock_release(&q->lock);

  return 0;
}

int pop(queue_t *q, void **ret) {
  while(!platform_lock_acquire(&q->lock)) {};
  
  if(_is_empty(q)) {
    platform_lock_release(&q->lock);
    *ret = NULL;
    return 1; 
  }
  
  asm volatile("fence");
  *ret =  q->buf[q->head];
  q->head = (q->head + SIZE_QUEUE - 1) % SIZE_QUEUE;
  asm volatile("fence");

  platform_lock_release(&q->lock);

  return 0;
}

