#include "hotload.h"
#include "signal.h"
#include "mem.h"
#include "assert.h"

void reload(libctx *lib)
{
  if (lib->h != NULL){
    dlclose(lib->h);
    lib->h = 0;
    lib->ping = 0;
  }
  
  lib->h=dlopen(libname, RTLD_NOW);
  assert(lib->h);

  if(!(lib->ping=dlsym(lib->h, "_ping"))){
    perror("dlsym");
    printf("%s\n", dlerror());
    assert(0);
  }
}

static libctx *ctx = 0;
void libinit(libctx *l_ctx)
{ 
  // see signal.c for explanation
  if(!ctx){
    ctx = l_ctx;
  }
  ctx->state.msg = "hello world from plugin!";
  reload(ctx);
}

void* notifier(void* user_data)
{
  unused(user_data);
  int fd = inotify_init();
  if (fd<0){
    perror("inotify_init");
    assert(0);
  }
  if(inotify_add_watch(fd, "lib", IN_CLOSE_WRITE) < 0){
    perror("inotify_add_watch");
    assert(0);
  }
  char buf[256];
  for(;;){
    size len = read(fd, buf, sizeof(buf));
    if (len < 0){
      perror("read");
      close(fd);
      assert(0);
    }
    
    struct inotify_event *metadata = (struct inotify_event *)buf;
    if ((metadata->mask & IN_CLOSE_WRITE)){
      emit(hotload, ctx);
    }
  }
  close(fd);
}


