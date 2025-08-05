#ifndef __include_hotload
#define __include_hotload

#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/inotify.h>

#include "signal.h"

#define libname "./lib/libhot.so"

typedef struct {
  const char* msg;
} libstate;


typedef void *handle;
typedef void(*ping_fn)(void *state);


typedef struct {
  handle *h;
  libstate state;

  _Atomic ping_fn ping;
} libctx;


/* thread logic for doing blocked notifies on .so change*/
void *notifier(void *user_data);

void reload(libctx* ctx);
void libinit(libctx* l_ctx);

#endif
