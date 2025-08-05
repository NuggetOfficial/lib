#ifndef __include_queue
#define __include_queue

#include <stdatomic.h>

typedef unsigned int u32;
typedef _Atomic u32 queue;

// sets the maximum queue size in powers of 2
#define EXP 5

int queue_pop(queue* q);
void queue_pop_commit(queue* q);

int queue_push(queue* q);
void queue_push_commit(queue* q);

#endif
