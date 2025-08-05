#ifndef __include_assert
#define __include_assert

#include <stdio.h>

#define assert(x)\
  do{\
    if (!(x)){\
      fprintf(stderr, "(%s:%i) \e[1;31merror\e[0m: assertion %s failed", __FILE__, __LINE__, #x);\
      __builtin_trap();\
    }\
  } while (0)

#endif

