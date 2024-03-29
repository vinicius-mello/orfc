#ifndef ARRAY_H
#define ARRAY_H
#include "obj.h"

void array_init();
bool is_array(obj a);
obj array_new(size_t n, obj val);
size_t array_len(obj a);
size_t array_capacity(obj a);
obj array_get(obj a, int i);
void array_set(obj a, int i, obj o);
void array_push(obj a, obj o);
void array_sort(obj a);

#endif

#ifdef ARRAY_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>

typedef struct array_data {
  size_t capacity;
  size_t len;
  obj * data;
} array_data;

void array_free(obj a) {
  array_data * ad = a;
  for(int i=0; i<ad->len; ++i) 
    unref(ad->data[i]);
  unref(ad->data);
}

void array_print(obj a) {
  printf("[");
  for(int i=0; i<array_len(a); ++i) {
    obj_scope {
      if(i) printf(", ");
      obj_print(str_obj(array_get(a, i)));
    }
  }
  printf("]");
}

obj array_str(obj a) {
  obj r;
  obj_scope {
    r = str_c("[");
    obj comma = str_c(", ");
    for(int i=0; i<array_len(a); ++i) {
      obj_scope {
        if(i) str_push(r, comma);
        str_push(r, str_obj(array_get(a, i)));
      }
    }
    str_push(r, str_c("]"));
    ref(r);
  }
  obj_return(r);
}

int array_cmpi(const void * a, const void * b) {
  obj aa = *(obj *)a; 
  obj bb = *(obj *)b; 
  int la = array_len(aa);
  int lb = array_len(bb);
  for(int i=0; i<la && i<lb; ++i) {
    int c;
    obj_scope {
      obj ao = array_get(aa, i);
      obj bo = array_get(bb, i);
      c = obj_cmpi(&ao, &bo);
    }
    if(c) return c;
  }
  return la-lb; 
}

static int array_type_id;
static type array_type = {
  .free = array_free,
  .print = array_print,
  .str = array_str,
  .cmpi = array_cmpi,
  .name = "array"
};

void array_init() {
  array_type_id = register_type(&array_type);
}

bool is_array(obj a) {
  return obj_type(a) == array_type_id;
}

obj array_new(size_t n, obj val) {
  array_data * ad;
  obj_scope {
    ad = auto_new(sizeof(array_data), array_type_id);
    size_t cap = size_pow2(n==0 ? 1 : n);
    ad->capacity = cap;
    ad->len = n;
    ad->data = raw_new(cap*sizeof(obj));
    ref(ad->data);
    for(int i=0; i<n; ++i) {
      ad->data[i] = val; ref(val);
    }
    ref(ad);
  }
  obj_return(ad);
}

size_t array_len(obj a) {
  assert(is_array(a));
  array_data * ad = a;
  return ad->len;
}

size_t array_capacity(obj a) {
  assert(is_array(a));
  array_data * ad = a;
  return ad->capacity;
}

obj array_get(obj a, int i) {
  assert(is_array(a));
  assert(i>=0 && i<array_len(a));
  array_data * ad = a;
  obj o = ad->data[i];
  ref(o);
  obj_return(o);
}

void array_set(obj a, int i, obj o) {
  assert(is_array(a));
  assert(i>=0 && i<array_len(a));
  array_data * ad = a;
  unref(ad->data[i]);
  ad->data[i] = o; ref(o);
}

void array_push(obj a, obj o) {
  assert(is_array(a));
  obj_scope {
    array_data * ad = a;
    int nlen = ad->len+1;
    int c = ad->capacity;
    if(nlen>c) {
      int nc = 2*c;
      obj nd = raw_new(nc*sizeof(obj));
      memcpy(nd, ad->data, ad->len*sizeof(obj));
      unref(ad->data);
      ad->capacity = nc;
      ad->data = nd; ref(nd);
    } 
    ad->data[ad->len] = o; ref(o);
    ad->len = nlen;
  }
}

void array_sort(obj a) {
  assert(is_array(a));
  array_data * ad = a;
  qsort(ad->data, ad->len, sizeof(obj *), obj_cmpi);
}

#endif
