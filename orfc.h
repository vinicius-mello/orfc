#ifndef ORFC_H
#define ORFC_H

#include "utl.h"
#include "obj.h"
#include "pair.h"
#include "array.h"
#include "str.h"
#include "num.h"
#include "ht.h"
#include "file.h"

#define get(a, k) _Generic((k), int: array_get, \
                              obj: ht_get)(a, k)
#define set(a, k, v) _Generic((k), int: array_set, \
                              obj: ht_set)(a, k, v)
#define num(v) _Generic((v), int: num_int, double: num_double)(v)

inline int len(obj a) {
  if(is_str(a)) return str_len(a);
  if(is_array(a)) return array_len(a);
  return 0;
} 

inline void print(obj a) {
  obj_print(a);
}

inline void println(obj a) {
  obj_println(a);
}

inline obj str(obj a) {
  return str_obj(a);
}

void orfc_init();

#endif
