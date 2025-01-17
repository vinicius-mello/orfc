#include "orfc.h"
#define UTL_IMPLEMENTATION
#include "utl.h"
#undef UTL_IMPLEMENTATION
#define OBJ_IMPLEMENTATION
#include "obj.h"
#undef OBJ_IMPLEMENTATION
#define PAIR_IMPLEMENTATION
#include "pair.h"
#undef PAIR_IMPLEMENTATION
#define ARRAY_IMPLEMENTATION
#include "array.h"
#undef ARRAY_IMPLEMENTATION
#define STR_IMPLEMENTATION
#include "str.h"
#undef STR_IMPLEMENTATION
#define NUM_IMPLEMENTATION
#include "num.h"
#undef NUM_IMPLEMENTATION
#define HT_IMPLEMENTATION
#include "ht.h"
#undef HT_IMPLEMENTATION
#define FILE_IMPLEMENTATION
#include "file.h"
#undef FILE_IMPLEMENTATION


int len(obj a) {
  if(is_str(a)) return str_len(a);
  if(is_array(a)) return array_len(a);
  return 0;
} 

void print(obj a) {
  obj_print(a);
}

void println(obj a) {
  obj_println(a);
}

obj str(obj a) {
  return str_obj(a);
}

void orfc_init() {
  obj_init();
  array_init();
  pair_init();
  str_init();
  num_init();
  ht_init();
  file_init();
}
