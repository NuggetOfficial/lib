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

