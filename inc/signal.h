#ifndef __include_signal
#define __include_signal

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "queue.h"

typedef unsigned short u16;

typedef enum {
  ping,
  disconnect,
  hotload,
  n_signals
} signal;

typedef void (*callback_fn)(void* user_data);

typedef struct {
  // meta data header for signals, we
  // could add more info here if required
  u16 attached;
} signature;


typedef struct {
  callback_fn f;
  void* user_data;
} task;

// limits 8 callbacks per signal
// consider more mem on demand
// approach
typedef callback_fn vtable[8];

typedef struct {
  // atomic ring buffer
  queue q;
  task tasks[1<<EXP];
  
  // singal handler
  signature sgs[n_signals];
  vtable tables[n_signals];

} sigctx;

void siginit(sigctx* l_ctx);
void attach(signal s, callback_fn f);
void detach(signal s);
void emit(signal s, void *user_data);
void* handler(void *user_data);

#endif
