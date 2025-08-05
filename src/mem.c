#include "assert.h"
#include "mem.h"

void *
alloc( arena* a, size objsize, size align, size num )
{
  assert(num > 0);
  size pad = (uptr)a->end & (align - 1);
  size req = objsize*num;
  assert(req <= a->end - a->beg - pad); /* consider oom policy like longjmp */
  return a->end -= req + pad;
}

arena arena_new(size cap)
{
  arena r = {0};
  // see man (2) mmap => we have map anonymous so fd and offset
  // should be -1 and 0. map is private so no IPC.
  r.beg = mmap(0, cap, PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  assert(r.beg != MAP_FAILED);
  r.end = r.beg + cap;
  r.cap = cap;
  return r;
}
