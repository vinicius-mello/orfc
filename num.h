#ifndef NUM_H
#define NUM_H
#include "obj.h"

void num_init();
int is_num(obj o);
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
  auto_begin();
  double * d = n;
  double i;
  modf(*d, &i);
  static char buf[64];
  if(*d == i) 
    sprintf(&buf[0], "%d", (int)i);
  else 
    sprintf(&buf[0], "%f", *d);
  obj r = str_c(&buf[0]);
  auto_end_return(r);
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
  auto_begin();
  double * d = auto_new(sizeof(double), num_type_id);
  *d = (double) n;
  auto_end_return(d);
}

obj num_double(double n) {
  auto_begin();
  double * d = auto_new(sizeof(double), num_type_id);
  *d = n;
  auto_end_return(d);
}

int int_num(obj n) {
  return (int)(*(double *)n);
}

double double_num(obj n) {
  return (*(double *)n);
}

void num_int_set(obj o, int n) {
  (*(double *)o) = (double) n;
}

void num_double_set(obj o, double n) {
  (*(double *)o) = n;
}

#endif
