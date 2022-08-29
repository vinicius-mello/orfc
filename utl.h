#ifndef UTL_H
#define UTL_H
#include <stddef.h>

#define concat(a, b) a##b
#define macro_var(name) concat(name, __LINE__)
#define defer(start, end) for(           \
	int macro_var(_i_) = (start, 0); \
	!macro_var(_i_);                 \
	(macro_var(_i_)+=1), end)        \

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
