#ifndef HT_H
#define HT_H
#include "obj.h"

void ht_init();
int is_ht(obj t);
obj ht_new(size_t capacity);
obj ht_get(obj t, obj k);
void ht_set(obj t, obj k, obj v);
void ht_del(obj t, obj k);

#endif

#ifdef HT_IMPLEMENTATION
#include "str.h"

typedef struct ht_kv {
  obj key;
  obj value;
} ht_kv;

typedef struct ht_data {
  size_t capacity;
  size_t occupation;
  ht_kv * data;
} ht_data;

ht_kv * ht_get_bucket(obj t, int i) {
  ht_data * ad = t;
  return &ad->data[i];
}

size_t ht_capacity(obj t) {
  ht_data * ad = t;
  return ad->capacity;
}

void ht_free(obj t) {
  for(int i=0; i<ht_capacity(t); ++i) {
    ht_kv * kv = ht_get_bucket(t, i);
    unref(kv->key);
    unref(kv->value);
  }
  ht_data * ad = t;
  unref(ad->data);
}

void ht_ref(obj t) {
  for(int i=0; i<ht_capacity(t); ++i) {
    ht_kv * kv = ht_get_bucket(t, i);
    ref(kv->key);
    ref(kv->value);
  }
  ht_data * ad = t;
  ref(ad->data);
}

void ht_print(obj t) {
  printf("{");
  int f = 0;
  for(int i=0; i<ht_capacity(t); ++i) {
    ht_kv * kv = ht_get_bucket(t, i);
    if(!is_nil(kv->key)) {
      auto_begin();
      if(f) printf(",");
      f = 1;
      obj_print(str_obj(kv->key));
      printf(" : ");
      obj_print(kv->value);
      auto_end();
    }
  }
  printf("}");
}

obj ht_str(obj t) {
  auto_begin();
  obj r = str_c("{");
  obj comma = str_c(",");
  obj colon = str_c(" : ");
  int f = 0;
  for(int i=0; i<ht_capacity(t); ++i) {
    auto_begin();
    ht_kv * kv = ht_get_bucket(t, i);
    if(!is_nil(kv->key)) {
      if(f) str_push(r, comma);
      f = 1;
      str_push(r, str_obj(kv->key));
      str_push(r, colon);
      str_push(r, str_obj(kv->value));
    }
    auto_end();
  }
  str_push(r, str_c("}"));
  auto_end_return(r);
}

int ht_cmpi(const void * a, const void * b) {
  return (*(obj *)a)-(*(obj *)b);
}

static int ht_type_id;
static type ht_type = {
  .free = ht_free,
  .print = ht_print,
  .str = ht_str,
  .cmpi = ht_cmpi,
  .name = "ht"
};

void ht_init() {
  ht_type_id = register_type(&ht_type);
}

int is_ht(obj t) {
  return obj_type(t) == ht_type_id;
}

obj ht_new(size_t capacity) {
  auto_begin();
  ht_data * ad = auto_new(sizeof(ht_data), ht_type_id);
  capacity = size_pow2(capacity);
  ad->capacity = capacity;
  ad->occupation = 0;
  ad->data = raw_new(ad->capacity*sizeof(ht_kv));
  for(int i=0; i<ad->capacity; ++i) {
    ad->data[i].key = nil; ref(nil);
    ad->data[i].value = nil; ref(nil);
  }
  ref(ad->data);
  auto_end_return(ad);
}

size_t djb2(unsigned char *str)
{
  size_t hash = 5381;
  int c;
  while (c = *str++)
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

int ht_find(obj t, obj k) {
  assert(is_ht(t) && is_str(k));
  size_t cap = ht_capacity(t);
  int i = djb2(c_str(k)) % cap;
  ht_kv * kv = ht_get_bucket(t, i);
  while(!is_nil(kv->key)) {
    if(str_cmp(k, kv->key)==0) {
      return i;
    }
    i = (i+1)%cap;
    kv = ht_get_bucket(t, i);
  }
  return i;
}

obj ht_get(obj t, obj k) {
  assert(is_ht(t) && is_str(k));
  ht_kv * kv = ht_get_bucket(t, ht_find(t, k));
  ref(kv->value);
  auto_push(kv->value);
  return kv->value;
}

void ht_set(obj t, obj k, obj v) {
  ht_data * ad = t;
  assert(is_ht(t) && is_str(k));
  ht_kv * kv = ht_get_bucket(t, ht_find(t, k));
  unref(kv->key);
  unref(kv->value);
  kv->key = k; ref(k);
  kv->value = v; ref(v);
  ad->occupation++;
  size_t cap = ad->capacity;
  if(ad->occupation>cap/2) {
    auto_begin();
    obj t2 = ht_new(2*cap);
    for(int i=0; i<cap; ++i) {
      auto_begin();
      ht_kv * kv = ht_get_bucket(t, i);
      if(!is_nil(kv->key)) 
        ht_set(t2, kv->key, kv->value);
      auto_end();
    }
    ht_ref(t2);
    ht_free(t);
    ht_data * at = t2;
    ad->data = at->data;
    ad->capacity = at->capacity;
    ad->occupation = at->occupation;
    auto_end();
  }
}

void ht_del(obj t, obj k) {
  assert(is_ht(t) && is_str(k));
  size_t cap = ht_capacity(t);
  int i = ht_find(t, k);
  ht_kv * kv = ht_get_bucket(t, i);
  if(!is_nil(kv->key)) {
    unref(kv->key);
    unref(kv->value);
    kv->key = nil; ref(nil);
    kv->value = nil; ref(nil);
    i = (i+1)%cap;
    kv = ht_get_bucket(t, i);
    while(!is_nil(kv->key)) {
      obj key = kv->key; ref(key);
      obj value = kv->value; ref(value);
      ht_del(t, key);
      ht_set(t, key, value);
      unref(key);
      unref(value);
      i = (i+1)%cap;
      kv = ht_get_bucket(t, i);
    }
  }
}

#endif
