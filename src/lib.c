#include "hotload.h"
#include "assert.h"

void _ping(libctx *lib)
{
  assert(lib->state.msg);
  printf("%s\n",lib->state.msg);
}
