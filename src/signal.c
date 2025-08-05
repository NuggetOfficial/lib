#include "signal.h"
#include "assert.h"

static sigctx* ctx = 0;
void siginit(sigctx* l_ctx){
  // singleton intialization of signal system to keep
  // syntax clean.
  // having our signal system as a singleton is in-
  // teresting because it allows us to opt in or out
  // of different memory allocation strategies. E.g
  // linear memory would allows us to hexdump our ctx
  // for post-mortum analysis etc.
  if (!ctx){
    ctx = l_ctx;
  }
}

void attach(signal s, callback_fn f)
{
  ctx->tables[s][ctx->sgs[s].attached++] = f;
}
void detach(signal s)
{
  memset(ctx->tables[s], 0, sizeof(vtable));
  ctx->sgs[s].attached = 0;
}

void emit(signal s, void* user_data)
{
  for (int i=0; i<ctx->sgs[s].attached; ++i){
    int idx = queue_push(&ctx->q);
    if (idx == -1){
      // consider actual full policy, could be e.g
      // emission turns into direct consumption or
      // spwawn a new thread with a new thread local
      // queue
      pthread_exit(0); /* !panic*/

    }
    ctx->tasks[idx].f = ctx->tables[s][i];    
    ctx->tasks[idx].user_data = user_data;
    queue_push_commit(&ctx->q);
  }
}

void* handler(void* user_data){
  unused(user_data);
  for (;;){
    int idx = queue_pop(&ctx->q);
    if (idx == -1){
      // we could also sleep here but I like the idea
      // of letting the kernel do its thing. I have a
      // feeling its a lot smarter than me. also we 
      // dont care about failure in our case
      sched_yield();
      continue;
    }
    ctx->tasks[idx].f(ctx->tasks[idx].user_data);    
    queue_pop_commit(&ctx->q);
  }
}
