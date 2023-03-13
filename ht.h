#ifndef HT_H
#define HT_H
#include "obj.h"

void ht_init();
bool is_ht(obj t);
obj ht_new(size_t capacity);
obj ht_get(obj t, obj k);
void ht_set(obj t, obj k, obj v);
void ht_del(obj t, obj k);

#endif

#ifdef HT_IMPLEMENTATION
#include "str.h"

typedef struct ht_data {
  size_t occupation;
  obj buckets;
} ht_data;

obj ht_bucket_get(obj t, int i) {
  ht_data * td = t;
  obj r;
  scope {
    r = array_get(td->buckets, i);
    ref(r);
  }
  obj_return(r);
}

void ht_bucket_set(obj t, int i, obj p) {
  ht_data * td = t;
  array_set(td->buckets, i, p);
}

size_t ht_capacity(obj t) {
  ht_data * td = t;
  return array_len(td->buckets);
}

double ht_load_factor(obj t) {
  ht_data * td = t;
  return ((double)td->occupation)/((double) ht_capacity(t));
}

void ht_occupation_inc(obj t) {
  ht_data * td = t;
  td->occupation++;
}

void ht_occupation_dec(obj t) {
  ht_data * td = t;
  td->occupation--;
}

void ht_free(obj t) {
  ht_data * td = t;
  unref(td->buckets);
}

size_t djb2(unsigned char *str)
{
  size_t hash = 5381;
  int c;
  while (c = *str++)
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

void ht_print(obj t) {
  printf("{ ");
  int f = 0;
  int cap = ht_capacity(t);
  for(int i=0; i<cap; ++i) {
    scope {
      obj kv = ht_bucket_get(t, i);
      if(!is_nil(kv)) {
        obj key = pair_first(kv);
        obj value = pair_second(kv);
        if(f) printf(", ");
        f = 1;
        printf("\t[%d] #%zu ", i, djb2(c_str(key)) % cap );
        obj_print(str_obj(key));
        printf(" : ");
        obj_print(str_obj(value));
      }
    }
  }
  printf("}");
}

obj ht_str(obj t) {
  obj r;
  scope {
    r = str_c("{");
    obj comma = str_c(",");
    obj colon = str_c(" : ");
    int f = 0;
    for(int i=0; i<ht_capacity(t); ++i) {
      scope {
        obj kv = ht_bucket_get(t, i);
        if(!is_nil(kv)) {
          obj key = pair_first(kv);
          obj value = pair_second(kv);
          if(f) str_push(r, comma);
          f = 1;
          str_push(r, str_obj(key));
          str_push(r, colon);
          str_push(r, str_obj(value));
        }
      }
    }
    str_push(r, str_c("}"));
    ref(r);
  }
  obj_return(r);
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
  ht_data * td;
  scope {
    td = auto_new(sizeof(ht_data), ht_type_id);
    capacity = size_pow2(capacity);
    td->occupation = 0;
    td->buckets = array_new(capacity, nil);
    ref(td->buckets);
    ref(td);
  }
  obj_return(td);
}

int ht_find(obj t, obj k) {
  assert(is_ht(t) && is_str(k));
  int i;
  scope {
    size_t cap = ht_capacity(t);
    i = djb2(c_str(k)) % cap;
    obj kv = ht_bucket_get(t, i);
    while(!is_nil(kv)) {
      obj key = pair_first(kv);
      if(str_cmp(k, key)==0) break;
      i = (i+1)%cap;
      kv = ht_bucket_get(t, i);
    }
  }
  return i;
}

obj ht_get(obj t, obj k) {
  assert(is_ht(t) && is_str(k));
  auto_begin();
  obj kv = ht_bucket_get(t, ht_find(t, k));
  if(!is_nil(kv)) {
    obj value = pair_second(kv);
    auto_end_return(value);
  }
  auto_end_return(nil);
}

void ht_pair_set(obj t, obj p) {
  assert(is_ht(t) && is_pair(p));
  scope {
    obj k = pair_first(p);
    int i = ht_find(t, k);
    obj kv = ht_bucket_get(t, i);
    if(is_nil(kv)) ht_occupation_inc(t);
    ht_bucket_set(t, i, p);
    if(ht_load_factor(t)>0.5) {
      scope {
        size_t cap = ht_capacity(t);
        obj t2 = ht_new(2*cap);
        for(int i=0; i<cap; ++i) {
          scope {
            obj kv = ht_bucket_get(t, i);
            if(!is_nil(kv)) 
              ht_pair_set(t2, kv);
          }
        }
        ht_data * td = t;
        ht_data * td2 = t2;
        unref(td->buckets);
        td->buckets = td2->buckets;
        ref(td->buckets); 
      }
    }
  }
}

void ht_set(obj t, obj k, obj v) {
  assert(is_str(k));
  scope {
    ht_pair_set(t, pair_new(k, v));
  }
}

void ht_del(obj t, obj k) {
  assert(is_ht(t) && is_str(k));
  scope {
    size_t cap = ht_capacity(t);
    int i = ht_find(t, k);
    obj kv = ht_bucket_get(t, i);
    if(!is_nil(kv)) {
      obj del = array_new(0, nil);
      do {
        array_push(del, kv);
        ht_bucket_set(t, i, nil);
        ht_occupation_dec(t);
        i = (i+1)%cap;
        kv = ht_bucket_get(t, i);
      } while(!is_nil(kv));
      for(int i=1; i<array_len(del); ++i) 
        ht_pair_set(t, array_get(del, i));
    }
  }
}

#endif
