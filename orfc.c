#include "orfc.h"
#define UTL_IMPLEMENTATION
#include "utl.h"
#undef UTL_IMPLEMENTATION
#define OBJ_IMPLEMENTATION
#include "obj.h"
#undef OBJ_IMPLEMENTATION
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

void orfc_init() {
  obj_init();
  array_init();
  str_init();
  num_init();
  ht_init();
}
