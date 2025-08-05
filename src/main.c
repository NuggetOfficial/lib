#if !(defined (__unix__) || (defined(__APPLE__) && defined(__MACH__)))
  #error "only valid on POSIX systems"
#endif

#include <stdio.h>
#include <pthread.h> /* posix only */
#include <unistd.h>  /* posix only */

#include "queue.h"
#include "mem.h"

#define unused(x) (void)x

void cb(void* user_data)
{
  unused(user_data);
  printf("callback 1\n");
}

void cb1(void* user_data)
{
  unused(user_data);
  printf("callback 2\n");
}

void on_exit(void* user_data)
{
  unused(user_data);
  printf("shutdown\n");
  pthread_exit(0);
}

typedef enum {
  ping,
  disconnect,
  n_signals
} signal;

typedef void (*callback_fn)(void* user_data);

typedef struct {
  int attached;
} signature;


typedef struct {
  callback_fn f;
  void* user_data;
} task;

// limits 16 callbacks per signal
// consider more mem on demand
// approach
typedef callback_fn vtable[16];

typedef struct {
  // consider context permanent linear memory alocator
  // that way we can enable more on-demand memory allocation
  // arena ctxperm;

  // atomic ring buffer
  queue q;
  task tasks[1<<EXP];
  
  // singal handler
  signature sgs[n_signals];
  vtable tables[n_signals];

} sigctx;
 
// thread global context, global contexts are always a bit
// iffy, consider changing to thread local or make it un-
// avoidable to do this wrong. reason for keeping it is 
// because I really like the emit(signal, &data) syntax
sigctx ctx = {0};

void attach(signal s, callback_fn f)
{
  ctx.tables[s][ctx.sgs[s].attached++] = f;
}

void emit(signal s, void* user_data)
{
  for (int i=0; i<ctx.sgs[s].attached; ++i){
    int idx = queue_push(&ctx.q);
    if (idx == -1){
      // consider actual full policy, could be e.g
      // emission turns into direct consumption or
      // spwawn a new thread with a new thread local
      // queue
      pthread_exit(0); /* !panic*/

    }
    ctx.tasks[idx].f = ctx.tables[s][i];    
    ctx.tasks[idx].user_data = user_data;
    queue_push_commit(&ctx.q);
  }
}

void* handler(void* user_data){
  unused(user_data);
  for (;;){
    int idx = queue_pop(&ctx.q);
    if (idx == -1){
      // we could also sleep here but I like the idea
      // of letting the kernel do its thing. I have a
      // feeling its a lot smarter than me. also we 
      // dont care about failure in our case
      sched_yield();
      continue;
    }
    ctx.tasks[idx].f(ctx.tasks[idx].user_data);    
    queue_pop_commit(&ctx.q);
  }
}

int main( void )
{
  // attaching callbacks to signals can be done before spawing
  // our handler thread. Can also still be done after spawing.
  // currently we only attach not deattach but for hot-loading
  // this would be required (function pointer updates)
  attach(disconnect, on_exit);
  attach(ping, cb);
  attach(ping, cb1);


  pthread_t consumer;
  if(pthread_create(&consumer, 0, handler, 0)){
    perror("pthread_create");
    return 1;
  }
  
  // emulate main loop that triggers 3 times and then
  // exists should print 6 lines in stdout.
  emit(ping, 0);
  emit(ping, 0);
  emit(ping, 0);

  // right now we always have to emit the on_exit to
  // properly close the handler thread and not block
  // on join. we can consider changing this so that
  // the on_exit cb gets called regardless but that
  // requires architectural decisions I dont want to 
  // make right now
  emit(disconnect, 0);

  if(pthread_join(consumer, 0)!=0){
    perror("pthread_join");
    return 2;
  }

  return 0;
}
