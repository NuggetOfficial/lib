#ifndef __include_mem
#define __include_mem

#include <stddef.h>
#include <stdint.h>

#include <sys/mman.h>

typedef char byte;
typedef ptrdiff_t size;
typedef uintptr_t uptr;

#define sizeof(x) (size)sizeof(x)
#define _Alignof(x) (size)_Alignof(x)

typedef struct {
  byte* beg;
  byte* end;
  size cap;
} arena;

void *alloc( arena* a, size objsize, size align, size num );

#define new3(a,t,n) (t *)alloc(a, sizeof(t), _Alignof(t), n)
#define new2(a,t)   (t *)alloc(a, sizeof(t), _Alignof(t), 1)
#define newx(a,b,c,d, ...) d
#define new(...) newx(__VA_ARGS__, new3, new2) (__VA_ARGS__)

arena arena_new(size cap);

#endif
