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

int len(obj a);
void print(obj a);
void println(obj a);
obj str(obj a);
void orfc_init();

#endif
