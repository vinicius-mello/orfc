#ifndef UTL_H
#define UTL_H
#include <stddef.h>

size_t lg(size_t n);
size_t size_pow2(size_t n);

#endif

#ifdef UTL_IMPLEMENTATION

size_t lg(size_t n) {
  size_t c = 0;
  for(; n>1; n >>= 1) c += 1;
  return c;
}

size_t size_pow2(size_t n) {
  size_t c = 1<<lg(n);
  return c == n ? c : c<<1;
}

#endif
