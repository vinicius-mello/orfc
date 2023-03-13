#ifndef NUM_H
#define NUM_H
#include "obj.h"

void num_init();
bool is_num(obj o);
obj num_int(int n);
obj num_double(double n);
int int_num(obj n);
double double_num(obj n);
void num_int_set(obj o, int n);
void num_double_set(obj o, double n);

#endif

#ifdef NUM_IMPLEMENTATION
#include <math.h>

void num_print(obj n) {
  double * d = n;
  double i;
  modf(*d, &i);
  if(*d == i) 
    printf("%d", (int)i);
  else
    printf("%f", *d);
}

obj num_str(obj n) {
  double * d = n;
  double i;
  modf(*d, &i);
  static char buf[256];
  if(*d == i) 
    sprintf(&buf[0], "%d", (int)i);
  else 
    sprintf(&buf[0], "%f", *d);
  obj r;
  scope { 
    r = str_c(&buf[0]);
    ref(r);;
  }
  obj_return(r);
}

int num_cmpi(const void * a, const void * b) {
  double s = double_num(*(obj *)a)-double_num(*(obj *)b);
  return s>0 ? 1 : (s<0 ? -1 : 0);
}

static int num_type_id;
static type num_type = {
  .free = 0,
  .print = num_print,
  .str = num_str,
  .cmpi = num_cmpi,
  .name = "num"
};

void num_init() {
  num_type_id = register_type(&num_type);
}

int is_num(obj o) {
  return obj_type(o) == num_type_id;
}

obj num_int(int n) {
  double * d;
  scope {
    d = auto_new(sizeof(double), num_type_id);
    *d = (double) n;
    ref(d);
  }
  obj_return(d);
}

obj num_double(double n) {
  double * d;
  scope {
    d = auto_new(sizeof(double), num_type_id);
    *d = n;
    ref(d);
  }
  obj_return(d);
}

int int_num(obj n) {
  assert(is_num(n));
  return (int)(*(double *)n);
}

double double_num(obj n) {
  assert(is_num(n));
  return (*(double *)n);
}

void num_int_set(obj o, int n) {
  assert(is_num(o));
  (*(double *)o) = (double) n;
}

void num_double_set(obj o, double n) {
  assert(is_num(o));
  (*(double *)o) = n;
}

#endif
