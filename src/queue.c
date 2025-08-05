#include "queue.h"

// probably my favourite implementation of a lock-free ring buffer
// I have come across, consider passing by Chirs Wellons blog that
// explains how this works: https://nullprogram.com/blog/2022/05/14/
// I highly recommend his material on arena's and hashmaps as well.


int queue_pop(queue *q)
{
  u32 r = atomic_load_explicit(q, memory_order_acquire);
  int mask = (1u<<EXP) - 1;
  int head = r     & mask;
  int tail = r>>16 & mask;
  return head == tail ? -1 : tail;
}

void queue_pop_commit(queue* q)
{
  atomic_fetch_add_explicit(q, 0x10000, memory_order_release);
}

int queue_push(queue* q)
{
  u32 r = atomic_load_explicit(q, memory_order_acquire);
  int mask = (1u<<EXP) - 1;
  int head = r     & mask;
  int tail = r>>16 & mask;
  int next = (head + 1u) & mask;
  if (r & 0x8000){/*avoid overflow on commit*/
    atomic_fetch_and_explicit(q,~0x8000, memory_order_relaxed);
  }
  return next == tail ? -1 : head;
}

void queue_push_commit(queue* q)
{
  atomic_fetch_add_explicit(q, 1, memory_order_release);
}
