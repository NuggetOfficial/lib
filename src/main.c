#if !(defined (__unix__) || (defined(__APPLE__) && defined(__MACH__)))
  #error "only valid on POSIX systems"
#endif

#include <stdio.h>
#include <signal.h>

#include "mem.h"
#include "assert.h"
#include "signal.h"
#include "hotload.h"


_Atomic int should_quit = 0;

void on_exit(void* user_data)
{
  unused(user_data);
  printf("shutdown\n");
  atomic_store(&should_quit, 1);
  pthread_exit(0);
}

void on_reload(void *user_data){
  libctx* lib = (libctx *)user_data;
  // important! we need to reset all the callback because 
  // there is no guarantee that the functions are at the 
  // same memory adress.
  detach(ping);
  reload(lib);
  attach(ping, lib->ping);
}



int main( void )
{
  arena perm = arena_new(512);
  sigctx *sig = new(&perm, sigctx);
  libctx *lib = new(&perm, libctx); 

  printf("%li\n",perm.end - perm.beg);
  siginit(sig);
  libinit(lib);
  // attaching callbacks to signals can be done before spawing
  // our handler thread. Can also still be done after spawing.
  // currently we only attach not deattach but for hot-loading
  // this would be required (function pointer updates)
  attach(disconnect, on_exit);
  attach(hotload, on_reload);
  attach(ping, lib->ping);


  pthread_t sigconsumer;
  if(pthread_create(&sigconsumer, 0, handler, 0)){
    perror("pthread_create");
    return 1;
  }
  pthread_t notify;
  if(pthread_create(&notify, 0, notifier, 0)){
    perror("pthread_create");
    return 1;
  }
  while (!should_quit){
    // emulate main loop
    emit(ping, lib);
    sleep(1);
  }
  // right now we always have to emit the on_exit to
  // properly close the handler thread and not block
  // on join. we can consider changing this so that
  // the on_exit cb gets called regardless but that
  // requires architectural decisions I dont want to 
  // make right now,
  // also because mainloop is not infinite this never
  // gets called, consider keyboard input thread.
  emit(disconnect, 0);

  if(pthread_join(sigconsumer, 0)!=0){
    perror("pthread_join");
    return 2;
  }

  if(pthread_join(sigconsumer, 0)!=0){
    perror("pthread_join");
    return 2;
  }

  return 0;
}

