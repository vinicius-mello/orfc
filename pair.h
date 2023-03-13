#ifndef PAIR_H
#define PAIR_H
#include "obj.h"

void pair_init();
bool is_pair(obj o);
obj pair_new(obj o1, obj o2);
obj pair_first(obj p);
obj pair_second(obj p);
void pair_first_set(obj p, obj o);
void pair_second_set(obj p, obj o);

#endif

#ifdef PAIR_IMPLEMENTATION

void pair_free(obj p) {
  obj * pp = (obj *)p;
  unref(pp[0]);
  unref(pp[1]);
}

void pair_print(obj p) {
    printf("(");
    obj_print(pair_first(p));
    printf(", ");
    obj_print(pair_second(p));
    printf(")");
}

obj pair_str(obj p) {
  obj r;
  scope {
    r = str_c("(");
    str_push(r, pair_first(p));
    str_push(r, str_c(", "));
    str_push(r, pair_second(p));
    str_push(r, str_c(")"));
    ref(r);
  }
  obj_return(r);
}

int pair_cmpi(const void * a, const void * b) {
  obj * pa = (obj *)*(obj *)a;
  obj * pb = (obj *)*(obj *)b;
  int r = obj_cmpi(&pa[0], &pb[0]);
  if(r==0)
    r = obj_cmpi(&pa[1], &pb[1]);
  return r;
}

static int pair_type_id;
static type pair_type = {
  .free = pair_free,
  .print = pair_print,
  .str = pair_str,
  .cmpi = pair_cmpi,
  .name = "pair"
};

void pair_init() {
  pair_type_id = register_type(&pair_type);
}

int is_pair(obj o) {
  return obj_type(o) == pair_type_id;
}

obj pair_new(obj o1, obj o2) {
  obj * o;
  scope {
    o = auto_new(2*sizeof(obj), pair_type_id);
    o[0] = o1; ref(o1);
    o[1] = o2; ref(o2);
    ref(o);
  }
  obj_return(o);
}

obj pair_first(obj p) {
  assert(is_pair(p));
  obj * pp = (obj *)p;
  ref(pp[0]);
  obj_return(pp[0]);
}

obj pair_second(obj p) {
  assert(is_pair(p));
  obj * pp = (obj *)p;
  ref(pp[1]);
  obj_return(pp[1]);
}

void pair_first_set(obj p, obj o) {
  assert(is_pair(p));
  obj * pp = (obj *)p;
  unref(pp[0]);
  pp[0] = o; ref(o);
}

void pair_second_set(obj p, obj o) {
  assert(is_pair(p));
  obj * pp = (obj *)p;
  unref(pp[1]);
  pp[1] = o; ref(o);
}

#endif
